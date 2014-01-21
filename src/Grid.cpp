#include <stdlib.h>
#include <stdio.h>
#include <string.h>

class Grid {
	public:
		int rows, cols;

		/**
		 * Creates a Grid of a given size, initialized to zero.
		 */
		Grid Grid(int rows, int cols) {
			return calloc(sizeof(double), rows*cols);
		}

		/**
		 * Copies and returns a pointer to an existing Grid.
		 */
		Grid Grid(Grid *mat) {
			Grid matrix = &mat;
			long size = sizeof(double) * matrix.rows * matrix.cols;
			return memcpy(malloc(size), &mat, size);
		}

		/**
		 * Destroys a Grid.
		 */
		~Grid() {
		}
};
