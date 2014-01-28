//============================================================================
// Name        : acousticDeploy_C.cpp
// Author      : Greg Burgess
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include "netcdf.h"
#include "Grid.h"
#include "Utility.h"
#include <stdlib.h>
#include <cmath>
#define _USE_MATH_DEFINES
#include <math.h>
Grid getBathy(string inputFile, string inputFileType, int startX, int startY, int XDist, int YDist, string seriesName, long timestamp, bool debug) {

	if(startX < 1 || startY < 1) {
		printError("topographyGrid x and y coordinates must be integers greater than 0.", timestamp);
	}
	if(inputFileType == "netcdf"){
		NcFile dataFile("sfc_pres_temp.nc", NcFile::read);
		// grab a slice (in grid form)
	}
	else if(inputFileType == "arcgis"){
		// Read an ArcGIS file
	}
	else {
		cout << "Bathymetry file not found."
		topographyGrid = simulatetopographyGrid(XDist,YDist)
	}

	// Quick fix to get rid of NA in topographyGrid, should probably be interpolated (or something)
	if(any(is.na(topographyGrid))){
		print("Warning: NAs found in topographyGrid! setting to zero. This may be inappropriate so you may want to manually remove them.")
		topographyGrid[is.na(topographyGrid)] <- 0
	}

	//print(topographyGrid)
    return(topographyGrid)
}


Grid simulatetopographyGrid(int XDist, int YDist) {
	long size = XDist * sizeof(double);
	Grid topographyGrid = new Grid(XDist, YDist);
	double* data = topographyGrid.data;
	void* ref = malloc(size);
	int i = 0;
	for(int x = 0; x< XDist ; x++) {
		for(int y = 0; y< XDist ; y++) {
			i++;
			data[i] = ref[x] * ref[y] * abs(ref[x]) * abs(ref[y]) - M_PI;
		}
	}
	return(topographyGrid)
}
