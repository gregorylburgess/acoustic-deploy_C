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
using namespace std;
Grid::Grid() {
	rows = 0;
	cols = 0;
	data = NULL;
}

/**
 * Creates a Grid of a given size, initialized to zero.
 */
Grid::Grid(int Cols, int Rows) {
	rows = Rows;
	cols = Cols;
	data = (double*) calloc(sizeof(double), rows * cols);
}

/**
 * Copies and returns a pointer to an existing Grid.
 */
Grid::Grid(Grid* mat) {
	Grid matrix = *mat;
	rows = matrix.rows;
	cols = matrix.cols;
	long size = sizeof(double) * matrix.rows * matrix.cols;
	data = (double*) memcpy(malloc(size), matrix.data, size);
}

void Grid::clearNA(double val) {
	for (int i=0; i<rows; i++) {
		for (int j=0; j<cols; j++) {
			if(isnan(data[(i * cols) + j])){
				data[(i * cols) + j] = 0;
			}
		}
		cout << "\n";
	}
}
/**
 * Prints the contents of the data array.
 */
void Grid::printData() {
	std::cout << std::fixed;
	for (int i=0; i<rows; i++) {
		for (int j=0; j<cols; j++) {
			cout << setprecision(3)<<data[(i * cols) + j] << "\t";
		}
		cout << "\n";
	}
}

/**
 * Writes the data value to a text file, inverting the rows (for GPUplot).
 */
void Grid::GNUwrite(string filename) {
	ofstream out;
	out.open(filename.c_str());
	for (int i=0; i<rows; i++) {
		for (int j=0; j<cols; j++) {
			out << setprecision(3)<<data[(i * cols) + j] << " ";
		}
		out << "\r\n";
	}

	out.close();
}
