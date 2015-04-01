//============================================================================
// Name        : Utility.cpp
// Author      : Greg Burgess
// Version     : 1.0
// Copyright   : Copyright 2014
// Description : Useful utility functions.
//============================================================================

#include <stdlib.h>
#include <Dense>
#include <Core>
#include <fstream>
#include <iostream>
#include <set>
#include <string>
#include <utility>
#include <vector>
#include "Utility.h"

namespace std {
/**
 * Mechanism for uniform error reporting.
 * @param msg The error message to output.
 * @param errcode The error code to output.
 * @param timestamp The timestamp to output.
 */
void printError(string msg, int errcode, string timestamp) {
    if (errcode) {
        cout << msg << " \nError Code: " << errcode  << "\n";
    } else {
        cout << msg << " \n";
    }
    exit(EXIT_FAILURE);
}

/**
 * Creates an array containing a regular sequence of numbers.
 * @param start The starting value of the sequence.
 * @param end The ending value of the sequence.
 * @param size The size of the array.
 * @return An array containing a regular sequence of numbers.
 */
double* seq(double start, double end, int size) {
    double* array = static_cast<double*>(calloc(sizeof(double), size));
    double range = end - start;
    double increment = range / size;
    for (int i = 0; i < size; i++) {
        array[i] = start + increment * i;
    }
    return array;
}

/**
 * Checks if a file exists.
 * @param filename A relative path to the file in question.
 * @return True if the file exists, false otherwise.
 */
bool fexists(string filename) {
    ifstream ifile(filename.c_str());
    return ifile;
}

/**
 * Prints a set of pairs to console.
 * @param pairs A pointer to a set of pairs.
 */
void printSet(set<pair<int, int>> *pairs) {
    for (set<pair<int, int>>::iterator it = pairs->begin();
                                it != pairs->end(); ++it) {
        pair<int, int>  i = *it;
        cout << "(" << i.first << "," << i.second << ")\n";
    }
    cout << "\n";
}
}  // namespace std
