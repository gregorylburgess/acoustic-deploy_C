//============================================================================
// Name        : acousticDeploy_C.cpp
// Author      : Greg Burgess
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <Dense>
#include <Core>
#include <set>
#include "Utility.h"
using namespace std;


/**
 * Provides unified error reporting capabilities.
 */
void printError(string msg, int errcode, string timestamp) {
	if (errcode)
		cout << msg << " \nError Code: " << errcode  << "\n";
	else
		cout << msg << " \n";
	exit (EXIT_FAILURE);
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

bool fexists(string filename) {
  ifstream ifile (filename.c_str());
  return ifile;
}

void printSet(std::set<std::pair<int,int> > *pairs) {
	for (std::set<std::pair<int,int> >::iterator it = pairs->begin(); it != pairs->end(); ++it) {
		std::pair<int,int>  i = *it;
	    cout <<"("<< i.first <<","<< i.second<<")\n";
	}
	cout <<"\n";
}


