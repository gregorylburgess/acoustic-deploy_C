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

double foo(double x) {
	return x+1;
}

int main() {
	bool test = true;
	acousticParams.insert({"debug","0"});

	acousticParams.insert({"cellSize","5"});
	acousticParams.insert({"fishmodel","0"});
	acousticParams.insert({"sensorRange","50"});
	acousticParams.insert({"bias","2"});

	acousticParams.insert({"ousdx",".1"});
	acousticParams.insert({"ousdy",".1"});
	acousticParams.insert({"oucor","0"});
	acousticParams.insert({"mux",".5"});
	acousticParams.insert({"muy",".5"});
	acousticParams.insert({"fishmodel","1"});
	acousticParams.insert({"contourDepths","0,-5,-15,-10"});
	acousticParams.insert({"minDepth","15"});
	acousticParams.insert({"maxDepth","20"});

	string  inputFile = "himbsyn.bathytopo.1km.v19.grd",
			inputFileType="netcdf",
			seriesName="z",
			timestamp = "-1",
			bathymetryTitle = "Topography",
			bathymetryFilePath = "data/Topography.dat",
			habitatTitle = "Habitat",
			habitatFilePath = "data/Habitat.dat",
			goodnessTitle = "Goodness",
			goodnessFilePath = "data/Goodness.dat",
			coverageTitle = "Acoustic Coverage",
			coverageFilePath = "data/Coverage.dat",
			token;

	int startX = 200,
	    startY = 200,
	    XDist = 1000,
	    YDist = 1000,
	    width = 500,
		height = 500,
		i=0;

	Grid* bGrid = new Grid(XDist, YDist, "Behavior");
	Grid* gGrid = new Grid(XDist, YDist, "Goodness");
	Grid* cGrid = new Grid(XDist, YDist, "Coverage");
	Grid* tGrid;

	if(test) {
		tGrid = simulatetopographyGrid(XDist,YDist);
	}
	else {
		tGrid = getBathy(inputFile, inputFileType, startX, startY,XDist, YDist, seriesName, timestamp);
	}

	//Fill in Behavior Grid
	fish(tGrid, bGrid);
	Graph tGraph = Graph(tGrid);
	Graph bGraph = Graph(bGrid);

	//Compute contour depth meta data (used for graphical output)
	istringstream contourString(acousticParams["contourDepths"]);
	size_t numContourDepths = std::count(acousticParams["contourDepths"].begin(), acousticParams["contourDepths"].end(), ',') + 1;
	acousticParams.insert({"numContourDepths",to_string(numContourDepths)});
	double contourLevels [numContourDepths];
	while(getline(contourString, token, ',')) {
		contourLevels[i] = stod(token);
		i++;
	}
	sort(contourLevels, contourLevels + numContourDepths);
	double *contourPtr = contourLevels;
	try {

		tGraph.writeMat();
		tGraph.writeDat();
		//print contour files (used by all graphs)
		tGraph.printContour(contourPtr);

		//Graph the Topography grid
		tGraph.printContourGraph(width,height, contourPtr);

		//print matrix for bGrid graph
		bGraph.writeMat();
		bGraph.writeDat();
		//print Behaviour Grid
		bGraph.printContourGraph(width,height, contourPtr);
	}
	catch(int e) {
		cout << "Error:" << e <<"\n";
		return 0;
	}

	calculateGoodnessGrid(tGrid, bGrid, gGrid, stoi(acousticParams["bias"]), stoi(acousticParams["sensorRange"]));
	Graph gGraph = Graph(gGrid);
	gGraph.writeMat();
	gGraph.writeDat();
	gGraph.printContourGraph(width,height, contourPtr);
	//cout<<gGrid->data;
	//Grid* temp = new Grid(calcPercentViz(tGrid, 30,30,2),"temp");
	//temp->printData();

	return 0;
}

