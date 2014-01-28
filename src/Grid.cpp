#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "Grid.h"

Grid::Grid() {
	rows = 0;
	cols = 0;
	data = NULL;
}

/**
 * Creates a Grid of a given size, initialized to zero.
 */
Grid::Grid(int Rows, int Cols) {
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

/**
 * Destroys a Grid.
 */
Grid::~Grid() {
	delete data;
}

