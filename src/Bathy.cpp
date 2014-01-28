//============================================================================
// Name        : acousticDeploy_C.cpp
// Author      : Greg Burgess
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================
using namespace std;
#include <iostream>
#include <netcdf.h>
#include <netcdfcpp.h>
#include <stdlib.h>
#include <cmath>
#include <math.h>
#include "Grid.h"
#include "Utility.h"
#define _USE_MATH_DEFINES


Grid simulatetopographyGrid(int XDist, int YDist) {
	long size = XDist * sizeof(double);
	Grid topographyGrid = new Grid(XDist, YDist);
	double* data = topographyGrid.data;
	double* ref = (double*)malloc(size);
	int i = 0;
	for(int x = 0; x< XDist; x++) {
		for(int y = 0; y< XDist; y++) {
			i++;
			data[i] = ref[x] * ref[y] * abs(ref[x]) * abs(ref[y]) - M_PI;
		}
	}
	return(topographyGrid);
}

Grid getBathy(string inputFile, string inputFileType, int startX, int startY, int XDist, int YDist, string seriesName, long timestamp, bool debug) {
	Grid topographyGrid;
	if(startX < 1 || startY < 1) {
		printError("topographyGrid x and y coordinates must be integers greater than 0.", timestamp);
	}
	if(inputFileType == "netcdf"){
		NcFile file = NcFile("simple_xy.nc");
		// grab a slice (in grid form)
	}
	else if(inputFileType == "arcgis"){
		// Read an ArcGIS file
	}
	else {
		cout << "Bathymetry file not found.";
		topographyGrid = simulatetopographyGrid(XDist,YDist);
	}

	// Quick fix to get rid of NA in topographyGrid, should probably be interpolated (or something)
	//if(any(is.na(topographyGrid))){
		//print("Warning: NAs found in topographyGrid! setting to zero. This may be inappropriate so you may want to manually remove them.")
	//	topographyGrid[is.na(topographyGrid)] <- 0
	//}

	//print(topographyGrid)
    return(topographyGrid);
}



