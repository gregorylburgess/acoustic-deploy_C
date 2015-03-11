//============================================================================
// Name        : acousticDeploy_C.cpp
// Author      : Greg Burgess
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
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
