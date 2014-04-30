#include <stdlib.h>
#include <iostream>
#include "Bathy.h"
#include "FishModel.h"
#include "Utility.h"
#include <string>
#include "Goodness.h"
#include "Graph.h"
#include "GlobalVars.h"
#include "Test/Test.h"
using namespace std;

int main() {
	/*acousticParams.insert({"debug",0});

	acousticParams.insert({"cellSize",5});
	acousticParams.insert({"fishmodel",1});
	acousticParams.insert({"sensorRange",5});
	acousticParams.insert({"bias",1});

	acousticParams.insert({"ousdx",.1});
	acousticParams.insert({"ousdy",.1});
	acousticParams.insert({"oucor",0});
	acousticParams.insert({"mux",.5});
	acousticParams.insert({"muy",.5});
	acousticParams.insert({"fishmodel",1});

	string inputFile = "himbsyn.bathytopo.1km.v19.grd",
		   inputFileType="netcdf",
		   seriesName="z";
	int startX = 200,
	    startY = 200,
	    XDist = 100,
	    YDist = 100;
	long timestamp = -1;
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

	Grid* bGrid = new Grid(XDist, YDist, "Behavior");
	Grid* gGrid = new Grid(XDist, YDist, "Goodness");
	Grid* cGrid = new Grid(XDist, YDist, "Coverage");

	//Grid* tGrid = getBathy(inputFile, inputFileType, startX, startY,XDist, YDist, seriesName, timestamp);
	Grid* tGrid = simulatetopographyGrid(XDist,YDist);
	//Fill in Behavior Grid
	fish(tGrid, bGrid);


	Graph tGraph = Graph(tGrid);
	Graph bGraph = Graph(bGrid);
	int contours[] = {0,-500,-1000};
	try {
		tGraph.writeMat();
		tGraph.writeDat();
		//print contour files and graph
		tGraph.printContour(contours);
		tGraph.printContourGraph(width,height);
		//print matrix for bGrid graph
		bGraph.writeMat();
		bGraph.writeDat();
	}
	catch(int e) {
		cout << "Error:" << e <<"\n";
		return 0;
	}
	cout << "Done Graphing" << "\n";
	calculateGoodnessGrid(tGrid, bGrid, gGrid, 1, 1);
	Graph gGraph = Graph(gGrid);
	gGraph.writeMat();*/
	//cout<<gGrid->data;

	runTests();
	return 0;
}

