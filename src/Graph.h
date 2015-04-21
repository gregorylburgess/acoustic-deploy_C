//============================================================================
// Name        : Graph.h
// Author      : Greg Burgess
// Version     : 1.0
// Copyright   : Copyright 2014
// Description : A class definition for a Graph object.
//============================================================================
#ifndef SRC_GRAPH_H_
#define SRC_GRAPH_H_
#pragma once
#include <string>
#include <vector>
#include "GlobalVars.h"

class Graph {
 public:
    bool contour;
    // Matricies are grids of z values.
    // An element's position in the matrix gives it's x/y value.
    std::string inputMatFile;
    // An input Data file is a list of x,y,z data points where each element
    // is separated by a space.
    // One x,y,z tuple per line.
    // non-contiguous lines are separated by an empty line.
    std::string inputDatFile;
    // A contour data file is an input data file that only contains contour
    //   information.
    std::string contourDataFile;
    Grid* grid;
    explicit Graph(Grid* g);
    void printContour(std::vector<std::string> *contourLevels);
    void  printContourGraph(int width, int height,
             std::vector<std::string> *contourLevels,
             bool logScaleGraphColoring);
    void writeDat();
    void writeMat();
};
#endif  // SRC_GRAPH_H_
