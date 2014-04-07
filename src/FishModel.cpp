#include <stdlib.h>
#include <iostream>
#include <fstream>
#include "Grid.h"
#include <unordered_map>
using namespace std;

Grid fish(unordered_map <string, string> params, Grid topographyGrid) {
	int rows = topographyGrid.rows;
	int cols = topographyGrid.cols;
	//land = topographyGrid$topographyGrid>=0
	string fishmodel = params["fishmodel"];
	if (fishmodel == "rw"){
		cout << "Using RW model";
		Grid behaviorGrid = new Grid(rows,cols);
		behaviorGrid.setAll(1);
	}
	else if (fishmodel == "ou") {
		cout << "Using OU model";
		double mux = min(topographyGrid$x) + diff(range(topographyGrid$x))*params$mux
		double muy = min(topographyGrid$y) + diff(range(topographyGrid$y))*params$muy
		double varx = std::pow(params["ousdx"],2);
		double vary = std::pow(params["ousdy"],2);
		double covxy = params["oucor"] * params["ousdx"] * params["ousdy"];
		double hrCov = matrix(c(vary,covxy,covxy,varx),2,2)
		double Y = matrix(rep(topographyGrid$y,rows),rows,cols,byrow=TRUE)
		double X = matrix(rep(topographyGrid$x,cols),rows,cols,byrow=FALSE)
		double XY = cbind(as.vector(X),as.vector(Y))
		double hrVals = dmvnorm(XY,c(mux,muy),hrCov)
		Grid behaviorGrid = matrix(hrVals,rows,cols,byrow=FALSE)
	}


	if((params.count("mindepth") > 0) && (params.count(maxdepth)>0)){
		behaviorGrid[!verticalHabitat(params$mindepth,params$maxdepth,topographyGrid$topographyGrid)] = 0
	}
	## Set land areas to zero
	behaviorGrid[land] = 0
	## Make sure behaviorGrid sums to one
	behaviorGrid = behaviorGrid/sum(behaviorGrid)
	return (behaviorGrid)

	return new Grid();
}

