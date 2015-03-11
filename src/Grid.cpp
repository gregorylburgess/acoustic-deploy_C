/** Copyright 2015 Greg Burgess. **/
#include <math.h>
#include <netcdf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <Dense>
#include <Core>
#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include "Grid.h"

namespace std {
Grid::Grid() {
    rows = 0;
    cols = 0;
    name = "";
    // data is already null-initialized by Eigen
}

/**
 * Creates a Grid of a given size, initialized to zero.
 */
Grid::Grid(int Rows, int Cols, string Name) {
    rows = Rows;
    cols = Cols;
    data.resize(Rows, Cols);
    data.setZero();
    name = Name;
}

/**
 * Copies and returns a pointer to an existing Grid.
 */
Grid::Grid(Grid* mat, string newName) {
    Grid matrix = *mat;
    rows = matrix.rows;
    cols = matrix.cols;
    name = newName;
    size_t size = sizeof(double) * matrix.rows * matrix.cols;
    memcpy(&data, &matrix.data, size);
}

/**
 * Makes a Grid out of an existing Eigen matrix.
 */
Grid::Grid(Eigen::MatrixXd dat, string newName) {
    rows = dat.rows();
    cols = dat.cols();
    name = newName;
    data = dat;
}


/**
 * A helper function that checks if a value isnan, and returns 0 if it is.
 */
double nanCheck(double x) {
    if (std::isnan(x)) {
        return 0;
    } else {
        return x;
    }
}


/**
 * Replaces NA values in the grid with the given value.
 */
void Grid::clearNA() {
    double (*nanCheck)(double);
    data.unaryExpr(nanCheck);
}


/**
 * Prints the contents of the data array.
 */
void Grid::printData() {
    std::cout << std::fixed;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            cout <<data(i, j) << "\t";
        }
        cout << "\n";
    }
}


void Grid::replace(double find, double replace) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            if (data(i, j)  ==  find) {
                data(i, j) = replace;
            }
        }
    }
}

void Grid::setAll(double val) {
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            data(i, j) = val;
        }
    }
}
}  // namespace std
