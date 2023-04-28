#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <numeric>
#include <math.h>

#pragma once

using namespace std;

extern const int num_actions;
extern const vector<char> actions;

class InformationSet {
public:
   vector<double> strategy_sum, cumulative_regrets;  
   InformationSet() : strategy_sum(num_actions, 0.0), cumulative_regrets(num_actions, 0.0) {}

   vector<double> normalize(std::vector<double> strategy);
   vector<double> get_strategy(double reach_probability);
   vector<double> get_average_strategy();

};
