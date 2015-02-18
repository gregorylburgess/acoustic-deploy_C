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

void simulatetopographyGrid(Grid* topographyGrid, int numRows, int numCols);
void getBathy(Grid* topographyGrid, std::string inputFile, std::string inputFileType, size_t startRow, size_t startCol,
		size_t numRows, size_t numCols, std::string seriesName, std::string timestamp);
double zero(double x);


