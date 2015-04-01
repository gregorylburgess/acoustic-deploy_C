#pragma once
#include <set>
#include <string>
#include <Dense>
using namespace std;

bool runGoodnessTests();
bool check(std::pair<double,double> *origin, std::pair<double,double> *target, std::set<std::pair<double,double>> *solution, string errorMsg);
bool checkCalcPercentViz();
bool checkCDistPartition();
bool checkDetectionGradient();
bool checkDistGradient();
bool checkNormalProb();
bool compare(double expectedValue, double receivedValue, string functionName);
bool compareMatrix(Eigen::MatrixXd* expectedMatirx, Eigen::MatrixXd* receivedMatrix, string functionName);
bool positiveSteep();
bool negativeSteep();
bool positiveSlow();
bool negativeSlow();
bool positiveHorizontal();
bool negativeHorizontal();
bool positiveVertical();
bool negativeVertical();
bool duplicatePoint();
