#include "../include/BestResponse.h"

using namespace std;

double BestResponse::get_bestResponse(KuhnTrainer cfr_strat, int br_player, int active_player, vector<char> cards, string history, double probs) {
	// check if the current_state is terminal to get payoffs
   if (KuhnPoker::is_terminal(history)) {
      return -KuhnPoker::get_payoff(history, cards);
   }
   int opp = active_player ^ 1;
   string game_state = cards[active_player] + history;

   if (active_player == br_player) {
      vector<double> vals(num_actions);
      for (int a = 0; a < num_actions; a++) {
         string next_history = history + actions[a];
         vals[a] = get_bestResponse(cfr_strat, br_player, opp, cards, next_history, probs);
      }

      // create vector of strategy for br_strat_map if non-existant
      if (br_strat_map.count(game_state) == 0) {
         br_strat_map[game_state] = vector<double> (num_actions, 0.0);
      }
      // compute the strategy weighted by probability of reaching the game_state
      for (int a = 0; a < num_actions; a++) {
         br_strat_map[game_state][a] += probs * vals[a];
      }
      double bestResponse_value = *max_element(vals.begin(), vals.end());
      return -bestResponse_value;
   } else {
      // let opponent strategy be the average_strategy at game_state that we obtain from CFR in KuhnTrainer
      vector<double> opp_strat = cfr_strat.infoset_map[game_state].get_average_strategy();
      // action_utility is the values that opponent gain if active_player plays bestResponse strategy
      // and opponent plays opp_strat probability
      vector<double> action_utility(num_actions);

      for (int a = 0; a < num_actions; a++) {
         string next_history = history + actions[a];
         double new_reach_probs = probs * opp_strat[a];
         action_utility[a] = get_bestResponse(cfr_strat, br_player, opp, cards, next_history, new_reach_probs);
      }
      double util = 0.0;
      for (int a = 0; a < num_actions; a++) {
         util += opp_strat[a] * action_utility[a];
      }
      return -util;
   }
}

double BestResponse::get_ev(map<string, vector<double>> p1_strat, map<string, vector<double>> p2_strat, int active_player, vector<char> cards, string history) {
   // check if the current_state is terminal to get payoffs
   if (KuhnPoker::is_terminal(history)) {
      return -KuhnPoker::get_payoff(history, cards);
   }
   int opp = active_player ^ 1;
   string game_state = cards[active_player] + history;

   vector<double> strategy_evs(num_actions);
   if (active_player == 0) { 
      strategy_evs = p1_strat[game_state];
   } else {
      strategy_evs = p2_strat[game_state];
   }
   vector<double> vals(num_actions);
   for (int a = 0; a < num_actions; a++) {
      string next_history = history + actions[a];
      //vals[a] = -get_ev(p1_strat, p2_strat, opp, cards, next_history);
      vals[a] = get_ev(p1_strat, p2_strat, opp, cards, next_history);
   }

   /*
   cout << "VALS at game_state " << game_state << " is: " << vals[0] << "  " << vals[1] << '\n';
   cout << "STRATEGY at game_state " << game_state << " is: " << strategy_evs[0] << "  " << strategy_evs[1] << '\n';
   */

   double ev = 0.0;
   for (int a = 0; a < num_actions; a++) {
      ev += strategy_evs[a] * vals[a];
   }
   return -ev;
}

double BestResponse::get_exploitability(KuhnTrainer cfr_strat) {
   vector<char> cards = {'J', 'K', 'Q'};
   double exploitability = 0;

   // Shuffle the cards with next_permutation, which means we assign
   // the probability of dealing 2 cards to 2 players to 1/6
   do {
      // get the br_strat_map:
      // to get the final br_strat_map, we must let the bestResponse player be both p1 and p2
      // to cover all cases --> generate the sufficient br_strat_map strategy
      double br_val_0 = get_bestResponse(cfr_strat, 0, 0, cards, "", 1.0);
      double br_val_1 = get_bestResponse(cfr_strat, 1, 0, cards, "", 1.0);
   } while (next_permutation(cards.begin(), cards.end()));

   // make opp_strat a map with the key being all game_state and value be the Nash equilibrium strategy for each game_state,
   // all of this comes from infoset_map that has already been computed by CFR in KuhnTrainer
   map<string, vector<double>> opp_strat;
   for (auto [game_state, infoset] : cfr_strat.infoset_map) {
      opp_strat[game_state] = infoset.get_average_strategy();
   }
   // let pure_br_strat_map be pure strategies for br_strat_map, which means we always choose the action that generates
   // the highest value in br_strat_map --> let the probability of choosing that strategy to 1, and everything else to 0
   map<string, vector<double>> pure_br_strat_map;
   for (auto [game_state, strat] : br_strat_map) {
      double maxval = *max_element(strat.begin(), strat.end());
      for (auto val : strat) {
         pure_br_strat_map[game_state].push_back(val == maxval ? 1.0 : 0.0);
      }
   }

   cards = {'J', 'K', 'Q'};
   do {  
      //cout << "---------------------NEW ITER: " << cards[0] << ' ' << cards[1] << " ----------------------\n";
      double ev1 = -get_ev(opp_strat, pure_br_strat_map, 0, cards, "");
      //cout << "--------------------------------------------\n";
      double ev2 = -get_ev(pure_br_strat_map, opp_strat, 0, cards, "");
      //cout << "EV1: " << ev1 << " and " << "EV2: " << -ev2 << '\n';
      exploitability += (1.0/6.0) * (ev1 - ev2); 
      // Pick 1/6 because we assume the cards are dealt with a fair probability 1/3
      // So the probability of dealing 2 cards to 2 players is always 1/3 * 1/2 = 1/6
      // TO DO: If we use range as in real solver, then the probability has to be based
      // on each player's hands' weights
   } while (next_permutation(cards.begin(), cards.end()));

   // normalize best response value
   for (auto& [game_state, strat] : br_strat_map) {
      for (auto& val : strat) {
         val /= 2.0; // 2 because a player getting dealt a hand 2 times according to the permutation
      }
   }

   return exploitability;
}  

