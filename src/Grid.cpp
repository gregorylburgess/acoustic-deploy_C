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
	name = "";
}

/**
 * Creates a Grid of a given size, initialized to zero.
 */
Grid::Grid(int Cols, int Rows, string Name) {
	rows = Rows;
	cols = Cols;
	data = (double*) calloc(sizeof(double), rows * cols);
	name = Name;
}

/**
 * Copies and returns a pointer to an existing Grid.
 */
Grid::Grid(Grid* mat) {
	Grid matrix = *mat;
	rows = matrix.rows;
	cols = matrix.cols;
	name = matrix.name;
	long size = sizeof(double) * matrix.rows * matrix.cols;
	data = (double*) memcpy(malloc(size), matrix.data, size);
}

/**
 * Replaces NA values in the grid with the given value.
 */
void Grid::clearNA(double val) {
	double tempVal = 0;
	for (int i=0; i<rows; i++) {
		for (int j=0; j<cols; j++) {
			val = data[(i * cols) + j];
			if(std::isnan(tempVal)){
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
 * Writes the data value to a text file as a matrix (.mat).
 */
void Grid::writeMat() {
	ofstream out;
	out.open(("data/" + name + ".mat").c_str());
	for (int i=0; i<rows; i++) {
		for (int j=0; j<cols; j++) {
			out << setprecision(3)<<data[(i * cols) + j] << " ";
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
			out << setprecision(3) << j << " " << i << " " << data[(i * cols) + j] << "\r\n";
		}
		out << "\r\n";
	}

	out.close();
}

void Grid::replace(double find, double replace) {
	for (int i=0; i<rows; i++) {
		for (int j=0; j<cols; j++) {
			if (data[(i * cols) + j] == find) {
				data[(i * cols) + j] = replace;
			}
		}
	}
}

void Grid::setAll(double val) {
	for (int i=0; i<rows; i++) {
		for (int j=0; j<cols; j++) {
			data[(i * cols) + j] = val;
		}
	}
}

double Grid::min() {
	double min = data[0];
	for (int i=0; i<rows; i++) {
		for (int j=0; j<cols; j++) {
			if (data[(i * cols) + j] < min) {
				min = data[(i * cols) + j];
			}
		}
	}
	return min;
}

double Grid::max() {
	double max = data[0];
	for (int i=0; i<rows; i++) {
		for (int j=0; j<cols; j++) {
			if (data[(i * cols) + j] > max) {
				max = data[(i * cols) + j];
			}
		}
	}
	return max;
}

double Grid::sum() {
	double sum = 0;
	for (int i=0; i<rows; i++) {
		for (int j=0; j<cols; j++) {
			if (data[(i * cols) + j] > max) {
				sum +=  data[(i * cols) + j];
			}
		}
	}
	return sum;
}
