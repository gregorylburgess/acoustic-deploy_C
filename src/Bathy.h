//============================================================================
// Name        : acousticDeploy_C.cpp
// Author      : Greg Burgess
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#pragma once
#include "Grid.h"
#include "Utility.h"


Grid* simulatetopographyGrid(int XDist, int YDist);
Grid* getBathy(std::string inputFile, std::string inputFileType, size_t startX, size_t startY,
		size_t XDist, size_t YDist, std::string seriesName, std::string timestamp);
double zero(double x);


