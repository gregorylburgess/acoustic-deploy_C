//============================================================================
// Name        : Grid.h
// Author      : Greg Burgess
// Version     : 1.0
// Copyright   : Copyright 2014
// Description : A class definition for a Grid object.
//============================================================================
#ifndef SRC_GRID_H_
#define SRC_GRID_H_
#include <Dense>
#include <string>
#include "GlobalVars.h"

/**
 * Represents a matrix of data.
 */
class Grid {
 public:
    int rows, cols;
    Eigen::MatrixXd data;
    std::string name;

    Grid();
    Grid(int Rows, int Cols, std::string name);
    Grid(Grid* mat, std::string name);
    Grid(Eigen::MatrixXd dat, std::string newName);
    void clearNA();
    void printData();
    void replace(double find, double replace);
    void setAll(double val);
    double nanCheck(double x);
};
#endif  // SRC_GRID_H_
