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
#include "gnuplot_i.hpp"
using namespace std;

void printError(string msg, int errcode, long timestamp) {
	if (errcode)
		cout << msg << " \nError Code: " << errcode  << "\n";
	else
		cout << msg << " \n";
	exit(1);
}

void  printGraph(string title, int XDist, int YDist, string dataFilePath, int width, int height) {
	double  xstart = -.5,
			ystart = -.5;
	string  filetype = ".gif",
			outfile = "img/" + title + filetype,
			setOuput = "set output \'" + outfile + "\';",
			xrange,
			yrange,
			size,
			plotData,
			pallete;
	cout << "Output File: " << outfile << "\n";
	cout << "Data File: " << dataFilePath << "\n";
	if (!fexists(dataFilePath)) {
		cout << "Input Data File not Found!\n";
		throw 1;
	}
	std::stringstream ss;
	ss << "set size " << width << "," << height << ";";
	size = ss.str();
	ss.str("");
	ss.clear();

	ss << "set yrange [" << ystart << ":" << YDist - 0.5 << "];";
	yrange = ss.str();
	ss.str("");
	ss.clear();

	ss << "set xrange [" << xstart << ":" << XDist - 0.5 << "];";
	xrange = ss.str();
	ss.str("");
	ss.clear();

ss << "plot \"" << dataFilePath << "\" matrix with image;";
	plotData = ss.str();
	ss.str("");
	ss.clear();

	ss << "set palette defined (0 '#000022', 1 '#000066', 2 '#009999', 3 '#cc6600', 4 '#FF9933', 5'#FF0000', 6 '#ffffff');";
	pallete= ss.str();

	try {
		Gnuplot plots;
		plots.cmd(setOuput);
		plots.cmd("set terminal gif;");
		plots.cmd(xrange);
		plots.cmd(yrange);
		//plots.cmd(size);
		plots.cmd("set palette maxcolors 10000;");
		plots.cmd(pallete);

		cout << setOuput << "\n";
		cout << "set terminal gif\n";
		cout << xrange << "\n";
		cout << yrange << "\n";
		cout << size << "\n";
		cout << "set palette maxcolors 100\n";
		cout << pallete << "\n";
		cout << plotData << "\n";

		plots.cmd(plotData);
	}
	catch (GnuplotException ge) {
	        cout << ge.what() << endl;
	}
	cout <<"Finished writing" << "\n";
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
  ifstream ifile(filename.c_str());
  return ifile;
}
