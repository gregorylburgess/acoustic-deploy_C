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
#include <Dense>
#include <Core>
#include "Grid.h"
#include "Utility.h"
#include "GlobalVars.h"
#define _USE_MATH_DEFINES
using namespace std;


double zero(double x) {
	if (x > 0) {
		return 0;
	}
	return x;
}


void simulatetopographyGrid(Grid* topographyGrid, int numRows, int numCols) {
	Eigen::VectorXd refx = refx.LinSpaced(numCols,-2*M_PI, 2*M_PI);
	Eigen::VectorXd refy = refx.LinSpaced(numRows,-2*M_PI, 2*M_PI);
	Eigen::MatrixXd X = refx.replicate(1,numRows);
	X.transposeInPlace();
	Eigen::MatrixXd Y = refy.replicate(1,numCols);

	//Eigen can only deal with two matrices at a time, so split the computation:
	//topographyGrid = sin(X) * sin(Y) * abs(X) * abs(Y) -pi
	Eigen::MatrixXd absXY = X.cwiseAbs().cwiseProduct(Y.cwiseAbs());
	Eigen::MatrixXd sins = X.array().sin().cwiseProduct(Y.array().sin());
	Eigen::MatrixXd temp = absXY.cwiseProduct(sins);

	//All this work to create a matrix of pi...
	Eigen::MatrixXd pi;
	pi.resize(numRows,numCols);
	pi.setConstant(M_PI);
	temp = temp - pi;
	//And the final result.
	topographyGrid->data.block(border,border,numRows,numCols) = temp.block(0,0,numRows,numCols);
	//Ignore positive values.
	topographyGrid->data = topographyGrid->data.unaryExpr(ptr_fun(zero));
	topographyGrid->clearNA();
}


void getBathy(Grid* topographyGrid, string inputFile, string inputFileType, size_t startRow, size_t startCol, size_t numRows, size_t numCols, string seriesName, string timestamp) {
	// This will be the netCDF ID for the file and data variable.
	Eigen::MatrixXd temp;
	temp.resize(numRows,numCols);
	cout<<"B1";
	int ncid, varid, retval=-100;
	if(strcmp(inputFileType.c_str(),"netcdf") == 0){

	   // Open the file. NC_NOWRITE tells netCDF we want read-only access to the file.
	   if ((retval = nc_open(inputFile.c_str(), NC_NOWRITE, &ncid))) {
		   printError("ERROR: Can't open NetCDF File. Invalid inputFile path.", retval, timestamp);
	   }

	   // Get the varid of the data variable, based on its name.
	   if ((retval = nc_inq_varid(ncid, seriesName.c_str(), &varid))) {
		   printError("ERROR: Can't access variable id.  Invalid seriesName.",retval, timestamp);
	   }
	   // Read the data.
	   try {
		   //for whatever reason, this is in column, row order.
		   static size_t start[] = {startCol, startRow};
		   static size_t range[] = {numCols, numRows};

		   retval = nc_get_vara_double(ncid, varid,start, range, temp.data());
		   cout<<"B1.5";
		   topographyGrid->data.block(border,border,numRows,numCols) = temp;
		   cout<<"B1.75";
	   }
	   catch (int i) {
		   printError("ERROR: Error reading data.  Invalid file format.", retval, timestamp);
	   }
	   // Close the file, freeing all resources.
	   if ((retval = nc_close(ncid))) {
		   printError("ERROR: Error closing the file.", retval, timestamp);
	   }
	}
	else {
		cout << "Bathymetry file type not supported.  Simulating Bathymetry.\n";
		simulatetopographyGrid(topographyGrid, (int)numRows, (int)numCols);
	}
	cout<<"B2";
	topographyGrid->clearNA();
	//topographyGrid->data.transposeInPlace();
	//topographyGrid->data = topographyGrid->data.colwise().reverse();
	if (acousticParams["debug"] == "1") {
		//topographyGrid->printData();
		cout<<"startx "<< startCol <<"\nXDist: "<<numCols<< "\nstartY: "<<startRow<<"\nYDist: "<<numRows<<"\n";
		cout<<"inputFileType: "<< inputFileType <<"\ninputFile: "<<inputFile<< "\nseriesName: "<<seriesName<<"\n";
		cout<<"retval: "<<retval<<"\n"<<"ncid: "<<ncid<<"\n\n";
	}
}



