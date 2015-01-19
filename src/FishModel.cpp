#include <stdlib.h>
#include <math.h>
#include <iostream>
#include <fstream>
#include "Grid.h"
#include <unordered_map>
#include <Dense>
#include "GlobalVars.h"
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


/**
 * Checks if x is non-negative, returning 1 if it is, and zero otherise.
 * @param x The value to check.
 */
double isNonNeg(double x) {
	if (x >= 0) {
		return 1;
	}
	return 0;
}

/**
 * Checks if x is non-positive, returning 1 if it is, and zero otherise.
 * @param x The value to check.
 */
double isNonPos(double x) {
	if (x <= 0) {
		return 1;
	}
	return 0;
}

void fish(Grid* topographyGrid, Grid* behaviorGrid) {
	int rows = topographyGrid->rows;
	int cols = topographyGrid->cols;
	int cellSize = stoi(acousticParams["cellSize"]);
	double ousdx = stod(acousticParams["ousdx"]),
		   ousdy = stod(acousticParams["ousdy"]),
		   oucor = stod(acousticParams["oucor"]),
		   mux 	 = stod(acousticParams["mux"]),
		   muy   = stod(acousticParams["muy"]);
	double fishmodel = stod(acousticParams["fishmodel"]);

	//RW
	if (fishmodel == 0){
		cout << "\nUsing RW model\n";
		behaviorGrid->setAll(1);
	}
	//OU
	else if (fishmodel == 1) {
		cout << "\nUsing OU model\n";
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
		double temp;
		for(double i=0; i<cols; i++) {
			for(double j=0; j<rows; j++) {
				temp = bivariateNorm(i/(cols-1), j/(rows-1), mux, muy, ousdx, ousdy, oucor);
				behaviorGrid->data(i,j) = temp;
			}
		}
	}
	//Vertical Habitat Restrictions
	if(acousticParams.count("minDepth") > 0 && acousticParams.count("maxDepth") > 0) {
		cout << "Using Vertical Habitat Restrictions: \nminDepth: " << acousticParams["minDepth"] << "m\nmaxDepth: " << acousticParams["maxDepth"] << "m\n";
		Grid* minGrid = new Grid(behaviorGrid->cols, behaviorGrid->rows, "min");
		Grid* maxGrid = new Grid(behaviorGrid->cols, behaviorGrid->rows, "max");
		Grid* temp = new Grid(behaviorGrid->cols, behaviorGrid->rows, "temp");
		//Add the minDepth to the topographyGrid (a copy, the original values are unmodified)
		minGrid->data = topographyGrid->data.array() + stod(acousticParams["minDepth"]);
		//Any non-positive values are valid, store a 1 there
		minGrid->data = minGrid->data.unaryExpr(ptr_fun(isNonPos));
		//Add teh maxDepth to the topographyGrid (again, a copy)
		maxGrid->data = topographyGrid->data.array() + stod(acousticParams["maxDepth"]);
		//Any non-negative values are valid, store a 1
		maxGrid->data = maxGrid->data.unaryExpr(ptr_fun(isNonNeg));
		//Multiply the two zero-one matricies to get cells with values between the min and max depths
		temp->data = minGrid->data.cwiseProduct(maxGrid->data);
		//Mask the behaviorGrid with our validitiy matrix
		behaviorGrid->data = behaviorGrid->data.cwiseProduct(temp->data);
	}
	//convert the grid to a probaility matrix
	double sum = behaviorGrid->data.sum();
    behaviorGrid->data = behaviorGrid->data / sum;
}





