//============================================================================
// Name        : acousticDeploy_C.cpp
// Author      : Greg Burgess
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include <iostream>
#include <string.h>
#include <netcdf.h>
#include <stdlib.h>
#include <cmath>
#include <math.h>
#include <vector>
#include "Grid.h"
#include "Utility.h"
#define _USE_MATH_DEFINES
using namespace std;


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
	Grid topographyGrid = new Grid(XDist, YDist);
	if(strcmp(inputFileType.c_str(),"netcdf") == 0){
		// grab a slice (in grid form)

	   // This will be the netCDF ID for the file and data variable.
	   int ncid, varid;

	   // Loop indexes, and error handling.
	   int retval;
	   // Open the file. NC_NOWRITE tells netCDF we want read-only access to the file.
	   if ((retval = nc_open(inputFile.c_str(), NC_NOWRITE, &ncid))) {
		   printError("ERROR: Can't open NetCDF File; check your inputFile.", retval, timestamp);
	   }

	   // Get the varid of the data variable, based on its name.
	   if ((retval = nc_inq_varid(ncid, seriesName.c_str(), &varid))) {
		   printError("ERROR: Can't access variable id; check your seriesName.",retval, timestamp);
	   }
	   // Read the data.
	   try {
		   static size_t start[] = {startX,startY};
		   static size_t range[] = {XDist,YDist};
		   retval = nc_get_vara_double(ncid, varid,start, range, topographyGrid.data);
	   }
	   catch (int i) {
		   printError("ERROR: Error reading data.", retval, timestamp);
	   }
	   // Close the file, freeing all resources.
	   if ((retval = nc_close(ncid))) {
		   cout << "ERROR: Error closing the file." << retval;
	   }
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
	topographyGrid.printData();
    return(topographyGrid);
}



