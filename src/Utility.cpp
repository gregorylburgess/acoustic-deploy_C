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
#include "GlobalVars.h"

/**
 * Mechanism for uniform error reporting.
 * @param msg The error message to output.
 * @param errcode The error code to output.
 * @param timestamp The timestamp to output.
 */
void printError(std::string msg, int errcode, std::string timestamp) {
    if (errcode) {
        std::cout << msg << " \nError Code: " << errcode  << "\n";
    } else {
        std::cout << msg << " \n";
    }
    exit(EXIT_FAILURE);
}

/**
 * Parses a character delimited string, returning a vector of the values stored within as strings.
 * @param str The comma delimited string to parse.
 * @param rslt A pointer to the resulting vector.
 * @param delimiter The delimiting character.
 */
void parseCDString(std::vector<std::string>* rslt, std::string str,
                   char delimiter) {
    std::string token;
    std::istringstream stream(str);
    while (std::getline(stream, token, delimiter)) {
        rslt->push_back(token);
    }
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
bool fexists(std::string filename) {
    std::ifstream ifile(filename.c_str());
    return ifile;
}

/**
 * Prints a set of pairs to console.
 * @param pairs A pointer to a set of pairs.
 */
void printSet(std::set<std::pair<int, int>> *pairs) {
    for (std::set<std::pair<int, int>>::iterator it = pairs->begin();
                                it != pairs->end(); ++it) {
        std::pair<int, int>  i = *it;
        std::cout << "(" << i.first << "," << i.second << ")\n";
    }
    std::cout << "\n";
}
