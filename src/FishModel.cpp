#include <stdlib.h>
#include <iostream>
#include <fstream>
#include "Grid.h"
#include <unordered_map>
#include <Dense>
using namespace std;

Grid* fish(unordered_map <string, string> params, Grid topographyGrid) {
	int rows = topographyGrid.rows;
	int cols = topographyGrid.cols;
	int cellSize = atoi(params["cellSize"].c_str());
	double ousdx = atof(params["ousdx"].c_str()),
		   ousdy = atof(params["ousdy"].c_str()),
		   oucor = atof(params["oucor"].c_str()),
		   mux 	 = atof(params["mux"].c_str()),
		   muy   = atof(params["muy"].c_str());
	//land = topographyGrid$topographyGrid>=0
	string fishmodel = params["fishmodel"];
	if (fishmodel == "rw"){
		cout << "Using RW model";
		Grid* behaviorGrid = new Grid(rows,cols, "behaviorGrid");
		behaviorGrid->setAll(1);
	}
	else if (fishmodel == "ou") {
		cout << "Using OU model";
		//topographyGrid$x is a range of length numcols from cellsize to numcols*cellsize.
		mux = cellSize + (rows - 1) * cellSize * mux;
		muy = cellSize + (cols -1) * cellSize * muy;
		double varx = pow(ousdx,2);
		double vary = pow(ousdy,2);
		double covxy = oucor * ousdx * ousdy;

		Eigen::Matrix2d hrCov;
		hrCov << vary,covxy,covxy,varx; //2d comma initalizer shortcut from Eigen

		Eigen::VectorXd tx = tx.LinSpaced(rows,1, rows) * cellSize;
		Eigen::RowVectorXd ty = ty.LinSpaced(cols,1, cols) * cellSize;

		Eigen::MatrixXd Y = ty.replicate(rows,1);
		Eigen::MatrixXd X = tx.replicate(1,cols);
		//Eigen::MatrixXd XY = cbind(as.vector(X),as.vector(Y))
		//Eigen::MatrixXd hrVals = dmvnorm(XY,c(mux,muy),hrCov)
		//Grid behaviorGrid = matrix(hrVals,rows,cols,byrow=FALSE)
	}/*


	if((params.count("mindepth") > 0) && (params.count("maxdepth")>0)){
		behaviorGrid[!verticalHabitat(atof(params["mindepth"]),atof(params["maxdepth"]),topographyGrid$topographyGrid)] = 0
	}
	// Set land areas to zero
	behaviorGrid[land] = 0
	// Make sure behaviorGrid sums to one
	behaviorGrid = behaviorGrid/behaviorGrid.sum();
	return (behaviorGrid)
*/Grid* a =new Grid();
	return a;
}

