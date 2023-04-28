#include <iostream>
#include <string>
#include <vector>
#include <algorithm>

#include "../include/KuhnTrainer.h"

using namespace std;

bool KuhnPoker::is_terminal(string history) {
   vector<string> terminal = {"bp", "bb", "pp", "pbp", "pbb"};
   return count(terminal.begin(), terminal.end(), history) > 0;
}

int KuhnPoker::get_payoff(string history, vector<char> cards) {
   if (history == "bp" || history == "pbp") {
      return 1;
   } else {
      // if 'bet' occurs 2 times, payoff is 2, otherwise it's only 1
      int payoff = count(history.begin(), history.end(), 'b') ? 2 : 1;
      int active_player = history.length() % 2;
      int opp = active_player ^ 1;
      if (cards[active_player] == 'K' || cards[opp] == 'J') {
         return payoff;
      } else {
         return -payoff;
      }
   }
} 

double KuhnTrainer::cfr(vector<char> cards, string history, vector<double> reach_probs, int active_player) {
   // check if the current_state is terminal to get payoffs
   if (KuhnPoker::is_terminal(history)) {
      return KuhnPoker::get_payoff(history, cards);
   }

   string infoset_str = cards[active_player] + history;
   int opp = active_player ^ 1;

   // Get InformationSet or create 1 if non-existent
   InformationSet& info_set = infoset_map[infoset_str];
   if (infoset_map.count(infoset_str) == 0) {
      info_set = infoset_map[infoset_str];
   } 
   
   vector<double> strategy = info_set.get_strategy(reach_probs[active_player]);
   vector<double> counterfactual_values(num_actions, 0.0);
   double node_util = 0.0;

   for (int a = 0; a < num_actions; a += 1) {
      double action_probability = strategy[a];
      vector<double> new_reach_probs = reach_probs;
      new_reach_probs[active_player] = reach_probs[active_player] * action_probability;

      // update the string of the next game state
      string next_history = history + actions[a];

      // counterfactual value of active_player when taking action a
      counterfactual_values[a] = -cfr(cards, next_history, new_reach_probs, opp);

      // value of the current game state is counterfactual value weighted by action probabilities
      node_util += counterfactual_values[a] * strategy[a];
   }

   for (int a = 0; a < num_actions; a += 1) {
      double regret = counterfactual_values[a] - node_util;

      // cumulative regret is the regret weighted by the probability of reaching the current game
      // state, which is reach_probs[opp]
      info_set.cumulative_regrets[a] += regret * reach_probs[opp];
   }

   return node_util;
}

double KuhnTrainer::discounted_cfr(vector<char> cards, string history, vector<double> reach_probs, int active_player, int iteration_number) {
   // check if the current_state is terminal to get payoffs
   if (KuhnPoker::is_terminal(history)) {
      return KuhnPoker::get_payoff(history, cards);
   }

   string infoset_str = cards[active_player] + history;
   int opp = active_player ^ 1;

   // Get InformationSet or create 1 if non-existent
   InformationSet& info_set = infoset_map[infoset_str];
   if (infoset_map.count(infoset_str) == 0) {
      info_set = infoset_map[infoset_str];
   } 
   
   vector<double> strategy = info_set.get_strategy(reach_probs[active_player]);
   vector<double> counterfactual_values(num_actions, 0.0);
   double node_util = 0.0;

   for (int a = 0; a < num_actions; a += 1) {
      double action_probability = strategy[a];
      vector<double> new_reach_probs = reach_probs;
      new_reach_probs[active_player] = reach_probs[active_player] * action_probability;

      // update the string of the next game state
      string next_history = history + actions[a];

      // counterfactual value of active_player when taking action a
      counterfactual_values[a] = -discounted_cfr(cards, next_history, new_reach_probs, opp, iteration_number);

      // value of the current game state is counterfactual value weighted by action probabilities
      node_util += counterfactual_values[a] * strategy[a];
   }

   for (int a = 0; a < num_actions; a += 1) {
      double regret = counterfactual_values[a] - node_util;

      // cumulative regret is the regret weighted by the probability of reaching the current game
      // state, which is reach_probs[opp]
      info_set.cumulative_regrets[a] += regret * reach_probs[opp];

      // multiply positive cumulative_regrets by alpha coeff, negative by beta coeff, according to DCFR
      // iteration_number + 1 because the iteration in train starts at 0
      if (info_set.cumulative_regrets[a] > 0) {
         auto alpha_coeff = pow(iteration_number + 1, alpha);
         alpha_coeff = alpha_coeff / (alpha_coeff + 1);
         info_set.cumulative_regrets[a] *= alpha_coeff;
      } else {
         auto beta_coeff = pow(iteration_number + 1, beta);
         beta_coeff = beta_coeff / (beta_coeff + 1);
         info_set.cumulative_regrets[a] *= beta_coeff;
      }
   }

   // multiply the strategy_sum with gamma_coeff, according to DCFR
   auto gamma_coeff = (iteration_number + 1.0) * 1.0 / (iteration_number * 1.0 + 2.0);
   gamma_coeff = pow(gamma_coeff, gamma);
   for (auto& val : info_set.strategy_sum) {
      val *= gamma_coeff;
   }

   return node_util;
}

double KuhnTrainer::train(int iter, string algorithm) {
   vector<char> cards = {'J', 'Q', 'K'};
   double util = 0;

   for (int _ = 0; _ < iter; _++) {
      // Knuth's shuffling algorithm
      for (int c1 = cards.size() - 1; c1 > 0; c1--) {
         int c2 = rand() % (c1 + 1);
         swap(cards[c1], cards[c2]);
      }
      string history = "";
      vector<double> reach_probs = {1.0, 1.0};
      if (algorithm == "cfr") {
         util += cfr(cards, history, reach_probs, 0);
      } else if (algorithm == "discounted_cfr") {
         util += discounted_cfr(cards, history, reach_probs, 0, _);
      } else {
         cout << "Algorithm not found\n";
      }
   }
   return util;
}

void KuhnTrainer::set_param(double _alpha, double _beta, double _gamma) {
   alpha = _alpha;
   beta = _beta;
   gamma = _gamma;
}
