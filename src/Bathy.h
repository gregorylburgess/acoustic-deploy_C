//============================================================================
// Name        : Bathy.h
// Author      : Greg Burgess
// Version     : 1.0
// Copyright   : Copyright 2014
// Description : Functions for reading and simulating Bathymetry.
//============================================================================
#ifndef SRC_BATHY_H_
#define SRC_BATHY_H_
#include <string>
#include <vector>
#include "Grid.h"
#include "Utility.h"
#include "GlobalVars.h"

double deValidateDepth(double x);
void simulatetopographyGrid(Grid* topographyGrid, int numRows, int numCols);
void getBathy(Grid* topographyGrid, std::string inputFile,
              std::string inputFileType, size_t startRow, size_t startCol,
              size_t numRows, size_t numCols, std::string seriesName,
              std::string timestamp);
void splitVec(std::string *s, char delim,
              std::vector<std::string> *elems);
std::vector<std::string> split(std::string *s, char delim);
double zero(double x);
#endif  // SRC_BATHY_H_
