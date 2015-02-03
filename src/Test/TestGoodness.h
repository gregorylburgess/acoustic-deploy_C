#pragma once
#include <set>
#include <string>
using namespace std;

bool runGoodnessTests();
bool checkCalcPercentViz();
bool check(std::pair<double,double> *origin, std::pair<double,double> *target, std::set<std::pair<double,double>> *solution, string errorMsg);
bool positiveSteep();
bool negativeSteep();
bool positiveSlow();
bool negativeSlow();
bool positiveHorizontal();
bool negativeHorizontal();
bool positiveVertical();
bool negativeVertical();
bool duplicatePoint();
