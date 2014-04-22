
#include "Bathy.h"
#include "FishModel.h"
#include "Utility.h"
#include <unordered_map>
#include <stdlib.h>
#include <iostream>
#include <string>
#include "Graph.h"
using namespace std;

int main() {
	unordered_map <string, string> params;
	string inputFile = "himbsyn.bathy.v19.grd",
		   inputFileType="netcdf",
		   seriesName="z";
	int startX = 1,
	    startY = 1,
	    XDist = 100,
	    YDist = 100;
	long timestamp = -1;
	bool debug = false;

	//Grid tGrid = getBathy(inputFile, inputFileType, startX, startY,XDist, YDist, seriesName, timestamp, debug);
	Grid* tGrid = simulatetopographyGrid(XDist,YDist);
	tGrid->printData();
	Grid* hGrid = new Grid(XDist, YDist, "Habitat");
	Grid* gGrid = new Grid(XDist, YDist, "Goodness");
	Grid* cGrid = new Grid(XDist, YDist, "Coverage");
	Grid* bGrid = new Grid(XDist, YDist, "Behavior");

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

	tGrid->writeMat();
	tGrid->writeDat();
	Graph tGraph = Graph(tGrid);
	int contourSize = 3;
	int contours[] = {0,-500,-1000};
	try {
		tGraph.printContour(contours, contourSize);
		tGraph.printContourGraph(width,height);
	}
	catch(int e) {
		cout << "Error:" << e <<"\n";
		return 0;
	}
	cout << "Done" << "\n";
	params.insert({"cellSize","5"});
	params.insert({"ousdx",".2"});
	params.insert({"ousdy",".2"});
	params.insert({"oucor",".7"});
	params.insert({"mux",".5"});
	params.insert({"muy",".5"});
	params.insert({"fishmodel","ou"});
	int cellSize = atoi(params["cellSize"].c_str());
	double ousdx = atof(params["ousdx"].c_str()),
		   ousdy = atof(params["ousdy"].c_str()),
		   oucor = atof(params["oucor"].c_str()),
		   mux 	 = atof(params["mux"].c_str()),
		   muy   = atof(params["muy"].c_str());
	string fishmodel = params["fishmodel"];

	bGrid = fish(params,tGrid);
	bGrid->writeMat();
	bGrid->writeDat();
	return 0;
}

