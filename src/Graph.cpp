#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include "Grid.h"
#include "Utility.h"
#include "gnuplot_i.hpp"
#include "Graph.h"
#include "GlobalVars.h"
using namespace std;



Graph::Graph(Grid* g) {
	contour = false;
	grid = g;
	contourDataFile = "data/contour.dat";
	inputDatFile = "data/" + grid->name + ".dat";
	inputMatFile = "data/" + grid->name + ".mat";
}
/**
 * Creates a data file containing contour lines, which can be overlayed onto another graph.
 * Call this BEFORE calling printContourGraph().
 * @param title The title of the data file to generate.
 * @param contourLevels An array of integers holding contour levels (depth as a negative integer).
 * @param numLevels The length of contourLevels.
 * @param inputDataFilePath The full path to the output file to create.
 *
 */
void Graph::printContour(int contourLevels[]) {
	Gnuplot plots;
	std::stringstream ss;
	string setOutput ="set table \"" + contourDataFile + "\"",
		   setContour = "set contour surface",
		   cntrparam,
		   noSurface = "set nosurface",
		   pm3d = "set pm3d map",
		   plot;

	//Print to a table instead of a gif
	plots.cmd(setOutput);
	//Enable contour lines
	plots.cmd(setContour);

	//Build a comma separated list of contour levels
	ss << "set cntrparam levels discrete ";
	int numLevels = sizeof(contourLevels)/sizeof(int);
	for(int i=0; i<numLevels-1; i++) {
		ss << contourLevels[i] << ",";
	}
	ss << contourLevels[numLevels-1];
	cntrparam = ss.str();
	plots.cmd(cntrparam);
	ss.str("");
	ss.clear();
	//Don't plot the map, just the contour lines
	plots.cmd(noSurface);
	//pm3d is required for contour lines
	plots.cmd(pm3d);
	//Plot the lines (this takes ~15s per contour level)
	ss << "splot \"" << inputDatFile << "\"";
	plot = ss.str();
	if(acousticParams["debug"] == 1) {
		cout << setOutput << "\n";
		cout << setContour << "\n";
		cout << cntrparam << "\n";
		cout << noSurface << "\n";
		cout << pm3d << "\n";
		cout << plot << "\n";
	}
	plots.cmd(plot);

}


/**
 * Prints a graph of a given file with contour data.
 * Requires that a contour file for the existing file exists.
 */
void  Graph::printContourGraph(int width, int height) {
	double  xstart = -.5,
			ystart = -.5;
	std::stringstream ss;
	string  filetype = ".gif",
			outfile = "img/" + grid->name + filetype,
			setOuput = "set output \'" + outfile + "\';",
			xrange,
			yrange,
			size,
			plotData,
			pallete;

	if(acousticParams["debug"] == 1) {
		cout << "Output File: " << outfile << "\n";
		cout << "Data File: " << inputDatFile << "\n";
	}

	if (!fexists(inputDatFile)) {
		cout << "Input Data File not Found!\n";
		throw 1;
	}

	//Set values
	ss << "set yrange [" << ystart << ":" << grid->rows - 0.5 << "];";
	yrange = ss.str();
	ss.str("");
	ss.clear();

	ss << "set xrange [" << xstart << ":" << grid->cols - 0.5 << "];";
	xrange = ss.str();
	ss.str("");
	ss.clear();

	ss << "plot \"" << inputDatFile << "\" matrix with image;";
	plotData = ss.str();
	ss.str("");
	ss.clear();

	try {
		Gnuplot plots;
		plots.cmd(setOuput);
		plots.cmd("set terminal gif;");
		plots.cmd("set term gif small;");
		plots.cmd("set style line 1 lt 1 lw 1;");
		plots.cmd(xrange);
		plots.cmd(yrange);
		//plots.cmd(size);
		if(acousticParams["debug"] == 1) {
			cout << setOuput << "\n";
			cout << "set terminal gif\n";
			cout << xrange << "\n";
			cout << yrange << "\n";
			cout << size << "\n";
			cout << pallete << "\n";
			cout << plotData << "\n";
		}
		plots.cmd(plotData);
	}
	catch (GnuplotException ge) {
	        cout << ge.what() << endl;
	}
	cout <<"Finished writing graph data files" << "\n";
}

/**
 * Writes the data value to a text file as a matrix (.mat).
 */
void Graph::writeMat() {
	ofstream out;
	int rows = grid->rows;
	int cols = grid->cols;
	out.open(("data/" + grid->name + ".mat").c_str());
	for (int i=0; i<rows; i++) {
		for (int j=0; j<cols; j++) {
			out << setprecision(3)<<grid->data(i,j) << " ";
		}
		out << "\r\n";
	}

	out.close();
}

/**
 * Writes the data value to a data file (.dat), as a list of x,y,z points.
 */
void Graph::writeDat() {
	ofstream out;
	int rows = grid->rows;
	int cols = grid->cols;
	out.open(("data/" + grid->name + ".dat").c_str());
	for (int i=0; i<rows; i++) {
		for (int j=0; j<cols; j++) {
			out << setprecision(3) << j << " " << i << " " << grid->data(i,j) << "\r\n";
		}
		out << "\r\n";
	}

	out.close();
}
