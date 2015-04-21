//============================================================================
// Name        : GlobalVars.cpp
// Author      : Greg Burgess
// Version     : 1.0
// Copyright   : Copyright 2014
// Description : Definitions of global objects.
//============================================================================
#include "GlobalVars.h"
#include <unordered_map>
#include <string>
std::unordered_map <std::string, std::string> acousticParams = {};
int border = 0;
bool debug = false, silent = false;
