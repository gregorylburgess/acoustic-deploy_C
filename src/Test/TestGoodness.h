//============================================================================
// Name        : TestGoodness.h
// Author      : Greg Burgess
// Version     : 1.0
// Copyright   : Copyright 2014
// Description : Functions for testing Goodness.cpp.
//============================================================================
#ifndef SRC_TESTGOODNESS_H_
#define SRC_TESTGOODNESS_H_
#include <set>
#include <string>
#include <Dense>
#include "../GlobalVars.h"

bool runGoodnessTests();
bool check(std::pair<double,double> *origin, std::pair<double,double> *target, std::set<std::pair<double,double>> *solution, std::string errorMsg);
bool checkCalculateGoodness();
bool checkCalcPercentViz();
bool checkCDistPartition();
bool checkDetectionGradient();
bool checkDistGradient();
bool checkNormalProb();
bool compare(double expectedValue, double receivedValue, std::string functionName);
bool compareMatrix(Eigen::MatrixXd* expectedMatirx, Eigen::MatrixXd* receivedMatrix, std::string functionName);
bool positiveSteep();
bool negativeSteep();
bool positiveSlow();
bool negativeSlow();
bool positiveHorizontal();
bool negativeHorizontal();
bool positiveVertical();
bool negativeVertical();
bool duplicatePoint();
bool testSelectTopSpots();
#endif
