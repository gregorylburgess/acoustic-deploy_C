//============================================================================
// Name        : acousticDeploy_C.cpp
// Author      : Greg Burgess
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <stdlib.h>
#include <iostream>
#include "Grid.h"
#include "Utility.h"
using namespace std;

void printError(string msg, long timestamp) {
	cout << msg << " " << timestamp;
}

double* seq(double start, double end, int size) {
	double* array = (double*) calloc(sizeof(double), size);
	double range = end - start;
	double increment = range / size;
	for (int i = 0; i < size; i++) {
		array[i] = start + increment * i;
	}
	return array;
}
