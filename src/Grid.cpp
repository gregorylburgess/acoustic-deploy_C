//============================================================================
// Name        : Grid.cpp
// Author      : Greg Burgess
// Version     : 1.0
// Copyright   : Copyright 2014
// Description : A class definition for a Grid object.
//============================================================================
#include <math.h>
#include <netcdf.h>
#include <stdio.h>
#include <stdlib.h>
#include <Dense>
#include <Core>
#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "Grid.h"

/**
 * Default constructor.
 */
Grid::Grid() {
    rows = 0;
    cols = 0;
    name = "";
    // data is already null-initialized by Eigen
}

/**
 * Creates a Grid of a given size, initialized to zero.
 * @param Rows The number of rows the Grid should have.
 * @param Cols The number of columns the Grid should have.
 * @param name The name of the Grid.  Used when writing files.
 */
Grid::Grid(int Rows, int Cols, std::string Name) {
    rows = Rows;
    cols = Cols;
    data.resize(Rows, Cols);
    data.setZero();
    name = Name;
}

/**
 * Copies and returns a pointer to an existing Grid.
 * @param mat A pointer to the Grid to copy.
 * @param newName The name of the newly created Grid.  Used when writing files.
 */
Grid::Grid(Grid* mat, std::string newName) {
    Grid matrix = *mat;
    rows = matrix.rows;
    cols = matrix.cols;
    name = newName;
    size_t size = sizeof(double) * matrix.rows * matrix.cols;
    memcpy(&data, &matrix.data, size);
}

/**
 * Makes a Grid out of an existing Eigen matrix.
 * @param dat An existing eigen matrix.
 * @param newName The name of the newly created Grid.  Used when writing files.
 */
Grid::Grid(Eigen::MatrixXd dat, std::string newName) {
    rows = dat.rows();
    cols = dat.cols();
    name = newName;
    data = dat;
}


/**
 * A helper function that checks if a value isnan, and returns 0 if it is.
 * @param x The value to inspect.
 * @return 0 if x is a nan value, x otherwise.
 */
double nanCheck(double x) {
    if (std::isnan(x)) {
        return 0;
    } else {
        return x;
    }
}


/**
 * Replaces NA values in the Grid with zeroes. Utilizes Eigen's unaryExpr
 * operator.
 */
void Grid::clearNA() {
    // TODO This wasn't always initialized to zero.  But it fixed a warning.
    // make sure this doesn't do anything weird...
    double (*nanCheck)(double) = 0;
    data.unaryExpr(nanCheck);
}


/**
 * Prints the contents of the data array to the console.
 */
void Grid::printData() {
    std::cout << std::fixed;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            std::cout <<data(i, j) << "\t";
        }
        std::cout << "\n";
    }
}

/**
 * Replaces all instances of a value in the Grid with another value.
 * @param find The value to replace.
 * @param replace The value to substitute.
 */
void Grid::replace(double find, double replace) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (data(i, j)  ==  find) {
                data(i, j) = replace;
            }
        }
    }
}

/**
 * Sets all cells in the Grid to the given value.
 * @param val The value all cells will be set to.
 */
void Grid::setAll(double val) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            data(i, j) = val;
        }
    }
}
