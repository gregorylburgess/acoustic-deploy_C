#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include "Grid.h"
#include <unordered_map>
#include <Dense>
using namespace std;


/**
 * Computes the bivariate normal distribution as a probability density for a given x,y pair.
 * param x The x coordinate of the point to calculate the probability for.
 * param y The y coordinate of the point to calculate the probability for.
 * param mux The location of the x-mean as a percentage of the x axis.  The center of the homerange on the x axis.
 * param muy The location of the y-mean as a percentage of the y axis.  The center of the homerange on the y axis.
 * param sdx The standard deviation of x as a percentage of the x axis.  The size of the homerange on the x axis.
 * param sdy The standard deviation of y as a percentage of the y axis.  The size of the homerange on the y axis
 * param cor The correlation between sdx and sdy.  Controls the tilt of the homerange.  Valid values are between 1 and -1
 */
double bivariateNorm(double x, double y, double mux, double muy, double sdx, double sdy, double cor) {
	//double z = (x-mux)^2/sdx^2  -  (2cor(x-mux)(y-muy)) / (sdx*sdy)   +  (y-muy)^2/sdy^2;
	//return 1/(2*PI*sdx*sdy*sqrt(1-cor^2)) ^ -z/2(1-cor^2);
	double nx = x - mux;
	double ny = y - muy;
	double ncor = 1 - pow(cor, 2);
	double nsdxy = sdx * sdy;
	double z = pow(nx / sdx, 2) - (2 * cor * nx * ny) / nsdxy + pow(ny / sdy, 2);
	double base = (1 / (2 * M_PI * nsdxy * sqrt(ncor)));
	double power = (-1 * z / (2 * ncor));
	double val = pow(base, power);
	//cout << "nx:"<<nx<< "  ny:"<<ny<<"   ncor:"<<ncor<< "    nsdxy:"<<nsdxy<< "   z:"<<z<<"   base:"<<base <<"    pow:"<<power<<"    val:"<<val<<"\n";
	return val;
}


Grid* fish(unordered_map <string, string> params, Grid* topographyGrid) {
	int rows = topographyGrid->rows;
	int cols = topographyGrid->cols;
	Grid* behaviorGrid = new Grid(rows, cols, "Behavioral Grid");
	int cellSize = atoi(params["cellSize"].c_str());
	double ousdx = atof(params["ousdx"].c_str()),
		   ousdy = atof(params["ousdy"].c_str()),
		   oucor = atof(params["oucor"].c_str()),
		   mux 	 = atof(params["mux"].c_str()),
		   muy   = atof(params["muy"].c_str());
	string fishmodel = params["fishmodel"];
	//land = topographyGrid$topographyGrid>=0
	if (fishmodel == "rw"){
		cout << "Using RW model";
		behaviorGrid->setAll(1);
	}
	else if (fishmodel == "ou") {
		cout << "Using OU model\n";
		//topographyGrid$x is a range of length numcols from cellsize to numcols*cellsize.
		//mux = cellSize + (rows - 1) * cellSize * mux;
		//muy = cellSize + (cols -1) * cellSize * muy;
		double varx = pow(ousdx,2);
		double vary = pow(ousdy,2);
		double covxy = oucor * ousdx * ousdy;

		Eigen::VectorXd tx = tx.LinSpaced(rows,1, rows) * cellSize;
		Eigen::RowVectorXd ty = ty.LinSpaced(cols,1, cols) * cellSize;

		//A matrix where each row goes from 1 to the # of cols
		Eigen::MatrixXd Y = ty.replicate(rows,1);
		//A matrix where each column goes from 1 to the # of rows
		Eigen::MatrixXd X = tx.replicate(1,cols);
		Eigen::MatrixXd XY;
		//A matrix containing all possible permutations x where {x|(a,b)}, where 1 <= a <= # of cols , 1 <= b <= # of rows
		XY.resize(rows*cols,2);
		X.resize(rows*cols,1);
		Y.resize(rows*cols,1);
		//create a table of x,y pairs
		XY.col(0) = X;
		XY.col(1) = Y;

		Eigen::Matrix2d hrCov;
		hrCov << vary,covxy,covxy,varx; //2d comma initalizer shortcut from Eigen
		double val;
		for(double i=0; i<cols; i++) {
			for(double j=0; j<rows; j++) {
				val= bivariateNorm(i/(cols-1), j/(rows-1), mux, muy, ousdx, ousdy, oucor);
				behaviorGrid->data(i,j)=val;
				//cout << "("<<i/(cols-1)<<","<<j/(rows-1)<<"): "<<val << +"\n";
			}

		}
	}
	return behaviorGrid;
}
			/*if((params.count("mindepth") > 0) && (params.count("maxdepth")>0)){
		behaviorGrid[!verticalHabitat(atof(params["mindepth"]),atof(params["maxdepth"]),topographyGrid$topographyGrid)] = 0
	}
	// Set land areas to zero
	behaviorGrid[land] = 0
	// Make sure behaviorGrid sums to one
	behaviorGrid = behaviorGrid/behaviorGrid.sum();
	return (behaviorGrid)
*/




