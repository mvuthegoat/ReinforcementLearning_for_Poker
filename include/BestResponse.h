#include "InfoSet.h"
#include "KuhnTrainer.h"

#pragma once

using namespace std;

class BestResponse {
public:
   map<string, vector<double>> br_strat_map;

   double get_bestResponse(KuhnTrainer cfr_strat, int br_player, int active_player, vector<char> cards, string history, double probs);
   double get_ev(map<string, vector<double>> p1_strat, map<string, vector<double>> p2_strat, int active_player, vector<char> cards, string history);
   double get_exploitability(KuhnTrainer cfr_strat);
};
