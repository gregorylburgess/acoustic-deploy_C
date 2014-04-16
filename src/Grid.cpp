#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string.h>
#include <netcdf.h>
#include <stdlib.h>
#include <iomanip>
#include <cmath>
#include <math.h>
#include <vector>
#include "Grid.h"
#include <Dense>
#include <Core>
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


/**
 * Writes the data value to a text file as a matrix (.mat).
 */
void Grid::writeMat() {
	ofstream out;
	out.open(("data/" + name + ".mat").c_str());
	for (int i=0; i<rows; i++) {
		for (int j=0; j<cols; j++) {
			out << setprecision(3)<<data(i,j) << " ";
		}
		out << "\r\n";
	}

	out.close();
}

/**
 * Writes the data value to a data file (.dat), as a list of x,y,z points.
 */
void Grid::writeDat() {
	ofstream out;
	out.open(("data/" + name + ".dat").c_str());
	for (int i=0; i<rows; i++) {
		for (int j=0; j<cols; j++) {
			out << setprecision(3) << j << " " << i << " " << data(i,j) << "\r\n";
		}
		out << "\r\n";
	}

	out.close();
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

double Grid::min() {
	double min = data(0,0);
	for (int i=0; i<rows; i++) {
		for (int j=0; j<cols; j++) {
			if (data(i,j) < min) {
				min = data(i,j);
			}
		}
	}
	return min;
}

double Grid::max() {
	double max = data(0,0);
	for (int i=0; i<rows; i++) {
		for (int j=0; j<cols; j++) {
			if (data(i,j) > max) {
				max = data(i,j);
			}
		}
	}
	return max;
}

double Grid::sum() {
	double sum = 0;
	return sum;
}
