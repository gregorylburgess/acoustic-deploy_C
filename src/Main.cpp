
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
#include <sys/time.h>
using namespace std;


int main() {
	bool test = false;
	bool simulateBathy = false;
	if (test) {
		runTests();
		cout <<"Done";
	}
	else {
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
		int startCol = 100,
			startRow = 100,
			ColDist = 500,
			RowDist = 300,
			width = 1000,
			height = 1000,
			i=0,
			bias = 3,
			sensorRange = 2;
		string token;
		border = sensorRange;
		struct timeval start, end;

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


		Grid* bGrid = new Grid(RowDist + 2 * border, ColDist + 2 * border, "Behavior");
		Grid* gGrid = new Grid(RowDist + 2 * border, ColDist + 2 * border, "Goodness");
		//Grid* cGrid = new Grid(RowDist + 2* border, ColDist + 2 * border, "Coverage");
		Grid* tGrid= new Grid(RowDist + 2 * border, ColDist + 2 * border, "Topography");
		cout<<"M0";
		//Fetch or simulate topography
		if(simulateBathy) {
			simulatetopographyGrid(tGrid, RowDist,ColDist);
		}
		else {
			getBathy(tGrid, acousticParams["inputFile"], acousticParams["inputFileType"],  size_t(startRow),
					 size_t(startCol), size_t(RowDist),  size_t(ColDist), acousticParams["seriesName"], acousticParams["timestamp"]);
		}
		cout<<"M1";
		//Fill in Behavior Grid
		populateBehaviorGrid(tGrid, bGrid);
		cout<<"M2";
		gettimeofday(&start,NULL);
		cout<<"CPV";
		//Calculate good sensor locations
		calculateGoodnessGrid(tGrid, bGrid, gGrid, bias, sensorRange);
		cout<<"EndCPV";
		gettimeofday(&end,NULL);
		printf("\n%f\n",
			 (end.tv_sec*1000000.0 + end.tv_usec -
			  start.tv_sec*1000000.0 - start.tv_usec) / 1000000.0);
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


/*
		Grid* temp = new Grid(calcPercentViz(tGrid, 30,30,2),"temp");
		cout<<tGrid->data.block(30, 30, 5, 5);
		cout << "\n";
		temp->printData();
*/	}
	return 0;
}

