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
		cout << msg << " \nError Code: " << errcode ;
	else
		cout << msg << " ";
	exit(1);
}

void  printGraph(string title, int XDist, int YDist, string dataFilePath, int width, int height, string deepHexColor, string shallowHexColor) {
	//set to -.5 so that the first cell doesn't get cut off...
	double  xstart = -.5,
			ystart = -.5;
	string  filetype = "gif",
			outfile = title + filetype,
			xrange,
			yrange,
			size,
			plotData,
			pallete;

	std::stringstream ss;
	ss << "set size " << width << "," << height << ";";
	size = ss.str();
	ss.str("");
	ss.clear();

	ss  << "set yrange [" << ystart << ":" << YDist - 0.5 << "];";
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

	ss << "set palette defined (0 '" << deepHexColor <<"', 1 '" << shallowHexColor << "');";
	pallete= ss.str();

	try {
		Gnuplot plots;
		string setOuput = "set output \"" + outfile + "\";";
		plots.cmd(setOuput);
		plots.cmd("set terminal gif;");
		plots.cmd(xrange);
		plots.cmd(yrange);
		plots.cmd(size);
		plots.cmd("set palette maxcolors 15");
		plots.cmd(pallete);
		cout << "writing...";
		plots.cmd(plotData);
	}
	catch (GnuplotException ge) {
	        cout << ge.what() << endl;
	}
	cout <<"Finished writing";
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
