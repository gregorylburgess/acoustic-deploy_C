/** Copyright 2015 Greg Burgess. **/
#ifndef SRC_GRAPH_H_
#define SRC_GRAPH_H_
#pragma once
#include <string>
#include <vector>
namespace std {
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
    void printContour(vector<double> *contourLevels);
    void  printContourGraph(int width, int height,
             vector<double> *contourLevels, bool logScaleGraphColoring);
    void writeDat();
    void writeMat();
};
}  // namespace std
#endif  // SRC_GRAPH_H_
