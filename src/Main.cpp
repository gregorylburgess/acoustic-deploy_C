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
	bool test = true;
	if (test) {
		runTests();
		cout <<"Done";
	}
	else {
		bool simulateBathy = false;
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

		acousticParams.insert({"contourDepths","0,-20,-40,-80"});

		acousticParams.insert({"minDepth","20"});
		acousticParams.insert({"maxDepth","40"});

		acousticParams.insert({"inputFile","himbsyn.bathytopo.1km.v19.grd"});
		acousticParams.insert({"inputFileType","netcdf"});
		acousticParams.insert({"seriesName","z"});
		acousticParams.insert({"timestamp","-1"});

		//TODO: Data validation
		int startX = 350,
			startY = 400,
			XDist = 100,
			YDist = 100,
			width = 500,
			height = 500,
			i=0,
			bias = 3,
			sensorRange = 5;
		string token;
		//Compute contour depth meta data (used for graphical output)
		istringstream contourString(acousticParams["contourDepths"]);
		size_t numContourDepths = std::count(acousticParams["contourDepths"].begin(), acousticParams["contourDepths"].end(), ',') + 1;
		acousticParams.insert({"numContourDepths",to_string(numContourDepths)});
		double contourLevels [numContourDepths];
		i=0;
		while(getline(contourString, token, ',')) {
			contourLevels[i] = stod(token);
			i++;
		}
		sort(contourLevels, contourLevels + numContourDepths);


		//File path variables
		string
				outputDataFilePath = "data/",
				outputDataFileType = ".dat",
				bathymetryTitle = "Topography",
				habitatTitle = "Habitat",
				goodnessTitle = "Goodness",

				coverageTitle = "Acoustic Coverage",
				bathymetryFilePath = outputDataFilePath + bathymetryTitle + outputDataFileType,
				habitatFilePath = outputDataFilePath + habitatTitle + outputDataFileType,
				goodnessFilePath = outputDataFilePath + goodnessTitle + outputDataFileType,
				coverageFilePath = outputDataFilePath + coverageTitle + outputDataFileType;


		Grid* bGrid = new Grid(XDist, YDist, "Behavior");
		Grid* gGrid = new Grid(XDist, YDist, "Goodness");
		Grid* cGrid = new Grid(XDist, YDist, "Coverage");
		Grid* tGrid;

		//Fetch or simulate topography
		if(simulateBathy) {
			tGrid = simulatetopographyGrid(XDist,YDist);
		}
		else {
			tGrid = getBathy(acousticParams["inputFile"], acousticParams["inputFileType"],  size_t(startX),
					 size_t(startY), size_t(XDist),  size_t(YDist), acousticParams["seriesName"], acousticParams["timestamp"]);
		}

		//Fill in Behavior Grid
		populateBehaviorGrid(tGrid, bGrid);

		//Calculate good sensor locations
		calculateGoodnessGrid(tGrid, bGrid, gGrid, bias, sensorRange);

		Graph gGraph = Graph(gGrid);
		Graph tGraph = Graph(tGrid);
		Graph bGraph = Graph(bGrid);

		// A pointer to the array with the Contour depths
		double *contourPtr = contourLevels;

		//Generate graphs
		try {
			//Print the matrix & data files for Topography Grid
			tGraph.writeMat();
			tGraph.writeDat();
			//Print the contour file used by all graphs.  (Do this just once as it takes a loooong time).
			tGraph.printContour(contourPtr);
			//Graph the Topography Grid with contour lines.
			tGraph.printContourGraph(width,height, contourPtr);

			//Print the matrix & data files for Bathymetry Grid
			bGraph.writeMat();
			bGraph.writeDat();
			//Graph Behavior Grid with contour lines.
			bGraph.printContourGraph(width,height, contourPtr);

			//Print the matrix & data files for Goodness Grid
			gGraph.writeMat();
			gGraph.writeDat();
			//Graph Goodness Grid with contour lines.
			gGraph.printContourGraph(width,height, contourPtr);
		}
		catch(int e) {
			cout << "Error:" << e <<"\n";
			return 0;
		}



		Grid* temp = new Grid(calcPercentViz(tGrid, 30,30,2),"temp");
		cout<<tGrid->data.block(30, 30, 5, 5);
		cout << "\n";
		temp->printData();
	}
	return 0;
}

