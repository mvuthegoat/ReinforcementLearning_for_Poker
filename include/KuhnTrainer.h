#include <iostream>
#include <string>
#include <vector>
#include <unordered_map>
#include <map>

#include "InfoSet.h"

#pragma once

using namespace std;

class KuhnPoker
{
public:
   static bool is_terminal(string history);
   static int get_payoff(string history, vector<char> cards);
};

class KuhnTrainer
{
public:
   map<string, InformationSet> infoset_map;

   KuhnTrainer() : alpha{1.5}, beta{0}, gamma{2.0} {}

   double cfr(vector<char> cards, string history, vector<double> reach_probs, int active_player);
   double discounted_cfr(vector<char> cards, string history, vector<double> reach_probs, int active_player, int iteration_number);
   double train(int iterations, string algorithm);
   void set_param(double _alpha, double _beta, double _gamma);

private:
   double alpha;
   double beta;
   double gamma;
};