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
namespace std {
void simulatetopographyGrid(Grid* topographyGrid, int numRows, int numCols);
void getBathy(Grid* topographyGrid, string inputFile,
              string inputFileType, size_t startRow, size_t startCol,
              size_t numRows, size_t numCols, string seriesName,
              string timestamp);
void splitVec(string *s, char delim,
              vector<string> *elems);
vector<string> split(string *s, char delim);
double zero(double x);
}  // namespace std
#endif  // SRC_BATHY_H_
