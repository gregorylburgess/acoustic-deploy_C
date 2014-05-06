#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <netcdf.h>
#include <stdlib.h>
#include <cmath>
#include <math.h>
#include <vector>
#include <Dense>
#include <Core>
#include "Grid.h"
using namespace Eigen;
using namespace std;

Grid::Grid() {
	rows = 0;
	cols = 0;
	name = "";
	//data is already null-initialized by Eigen
}

/**
 * Creates a Grid of a given size, initialized to zero.
 */
Grid::Grid(int Cols, int Rows, string Name) {
	rows = Rows;
	cols = Cols;
	data.resize(Rows,Cols);
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
	long size = sizeof(double) * matrix.rows * matrix.cols;
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
	}
	else {
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
	for (int i=0; i<rows; i++) {
		for (int j=0; j<cols; j++) {
			cout <<data(i,j) << "\t";
		}
		cout << "\n";
	}
}

void Grid::replace(double find, double replace) {
	for (int i=0; i<rows; i++) {
		for (int j=0; j<cols; j++) {
			if (data(i,j) == find) {
				data(i,j) = replace;
			}
		}
	}
}

void Grid::setAll(double val) {
	for (int i=0; i<rows; i++) {
		for (int j=0; j<cols; j++) {
			data(i,j) = val;
		}
	}
}
