#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <numeric>

#include "../include/InfoSet.h"

using namespace std;

const vector<char> actions = {'p', 'b'};
const int num_actions = 2;

vector<double> InformationSet::normalize(vector<double> strategy) {
	double normalized_sum = accumulate(strategy.begin(), strategy.end(), 0.0);
	for (int action = 0; action < num_actions; action++) {
		if (normalized_sum > 0) {
			strategy[action] /= normalized_sum;
		} else {
			strategy[action] = 1.0 / num_actions;
		}
	}
   return strategy;
}

vector<double> InformationSet::get_strategy(double reach_probability) {
   vector<double> strategy(num_actions);
   for (int action = 0; action < num_actions; action += 1) {
      strategy[action] = max(0.0, cumulative_regrets[action]);
   }
   strategy = normalize(strategy);
   for (int action = 0; action < num_actions; action += 1) {
      strategy_sum[action] += strategy[action] * reach_probability;
   }
   return strategy;
}

vector<double> InformationSet::get_average_strategy() {
   vector<double> avg_strategy = normalize(strategy_sum);
   return avg_strategy;
}

