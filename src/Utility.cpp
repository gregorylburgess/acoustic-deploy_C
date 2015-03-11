//============================================================================
// Name        : acousticDeploy_C.cpp
// Author      : Greg Burgess
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
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
 * Provides unified error reporting capabilities.
 */
void printError(string msg, int errcode, string timestamp) {
    if (errcode) {
        cout << msg << " \nError Code: " << errcode  << "\n";
    } else {
        cout << msg << " \n";
    }
    exit(EXIT_FAILURE);
}


double* seq(double start, double end, int size) {
    double* array = static_cast<double*>(calloc(sizeof(double), size));
    double range = end - start;
    double increment = range / size;
    for (int i = 0; i < size; i++) {
        array[i] = start + increment * i;
    }
    return array;
}

bool fexists(string filename) {
    ifstream ifile(filename.c_str());
    return ifile;
}

void printSet(set<pair<int, int>> *pairs) {
    for (set<pair<int, int>>::iterator it = pairs->begin();
                                it != pairs->end(); ++it) {
        pair<int, int>  i = *it;
        cout << "(" << i.first << "," << i.second << ")\n";
    }
    cout << "\n";
}
}  // namespace std
