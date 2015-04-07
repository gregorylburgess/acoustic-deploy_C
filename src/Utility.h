//============================================================================
// Name        : Utility.cpp
// Author      : Greg Burgess
// Version     : 1.0
// Copyright   : Copyright 2014
// Description : Useful utility functions.
//============================================================================

#ifndef SRC_UTILITY_H_
#define SRC_UTILITY_H_
#include <set>
#include <string>
#include <utility>
#include "GlobalVars.h"

void printError(std::string msg, int errcode, std::string timestamp);
void printGraph(std::string title, int XDist, int YDist,
                std::string dataFilePath, int width, int height);
double* seq(double start, double end, int size);
bool fexists(std::string filename);
void printSet(std::set<std::pair<int, int> > *pairs);
#endif  // SRC_UTILITY_H_
