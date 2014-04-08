
#include "Bathy.h"
#include "Utility.h"
#include <unordered_map>
#include <stdlib.h>
#include <iostream>
#include <string>
#include "Graph.h"
using namespace std;

int main() {
	unordered_map <string, double> params;
	string inputFile = "himbsyn.bathy.v19.grd",
		   inputFileType="netcdf",
		   seriesName="z";
	int startX = 7000,
	    startY = 7000,
	    XDist = 3000,
	    YDist = 3000;
	long timestamp = -1;
	bool debug = false;

	//Grid tGrid = getBathy(inputFile, inputFileType, startX, startY,XDist, YDist, seriesName, timestamp, debug);
	Grid tGrid = simulatetopographyGrid(XDist,YDist);
	Grid hGrid = new Grid(XDist, YDist, "Habitat");
	Grid gGrid = new Grid(XDist, YDist, "Goodness");
	Grid cGrid = new Grid(XDist, YDist, "Coverage");


	int width = 500,
		height = 500;
	string  bathymetryTitle = "Topography",
			bathymetryFilePath = "data/Topography.dat",
			habitatTitle = "Habitat",
			habitatFilePath = "data/Habitat.dat",
			goodnessTitle = "Goodness",
			goodnessFilePath = "data/Goodness.dat",
			coverageTitle = "Acoustic Coverage",
			coverageFilePath = "data/Coverage.dat";

	tGrid.writeMat();
	tGrid.writeDat();
	Graph tGraph = Graph(&tGrid);
	int contourSize = 3;
	int contours[] = {0,-500,-1000};
	try {
		tGraph.printContour(contours, contourSize);
	}
	catch(int e) {
		cout << "Error:" << e <<"\n";
		return 0;
	}
	cout << "Done" << "\n";
	return 0;
}

