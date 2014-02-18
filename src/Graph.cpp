#include <stdlib.h>
#include <iostream>
#include "Grid.h"
#include "Utility.h"
#include "gnuplot_i.hpp"
#include "Graph.h"
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
 * Call this BEFORE calling pringGraph().
 * @param title The title of the data file to generate.
 * @param contourLevels An array of integers holding contour levels (depth as a negative integer).
 * @param numLevels The length of contourLevels.
 * @param inputDataFilePath The full path to the output file to create.
 *
 */
void Graph::printContour(int contourLevels[], int numLevels) {
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
	for(int i=0; i<numLevels-1; i++) {
		ss << contourLevels[i] << ",";
	}
	ss << contourLevels[numLevels-1];
	cntrparam = ss.str();
	plots.cmd(cntrparam);
	ss.str("");
	ss.clear();
	//Don't plot the map, jsut the contour lines
	plots.cmd(noSurface);
	//pm3d is required for contour lines
	plots.cmd(pm3d);
	//Plot the lines (this takes ~15s per contour level)
	ss << "splot \"" << inputDatFile << "\"";
	plot = ss.str();

	cout << setOutput << "\n";
	cout << setContour << "\n";
	cout << cntrparam << "\n";
	cout << noSurface << "\n";
	cout << pm3d << "\n";
	cout << plot << "\n";
	plots.cmd(plot);

}


/**
 * Prints a graph of a given file with contour data.
 * Requires that a contour file for the existing file exists.
 */
void  Graph::printContourGraph(string inputDataFilePath, string contourDataFilePath, int width, int height) {
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


	cout << "Output File: " << outfile << "\n";
	cout << "Data File: " << inputDataFilePath << "\n";


	if (!fexists(inputDataFilePath)) {
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

	ss << "plot \"" << inputDataFilePath << "\" matrix with image;";
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

		cout << setOuput << "\n";
		cout << "set terminal gif\n";
		cout << xrange << "\n";
		cout << yrange << "\n";
		cout << size << "\n";
		cout << pallete << "\n";
		cout << plotData << "\n";

		plots.cmd(plotData);
	}
	catch (GnuplotException ge) {
	        cout << ge.what() << endl;
	}
	cout <<"Finished writing" << "\n";
}
