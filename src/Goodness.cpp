#include <algorithm>
#include <cmath>
#include <Dense>
#include <stdlib.h>
#include <iostream>
#include <limits>
#include <set>
#include <utility>
#include "GlobalVars.h"
#include "Goodness.h"
#include "Grid.h"
#include "Utility.h"

void calculateGoodnessGrid(Grid* topographyGrid, Grid* behaviorGrid, Grid* goodnessGrid, double bias, double range) {
	if (bias == 1) {
		goodFish(topographyGrid, behaviorGrid, goodnessGrid, range);
	}
	else if (bias == 2) {
		goodViz(topographyGrid, behaviorGrid, goodnessGrid, range);
	}
	else if (bias == 3) {
		goodVizOfFish(topographyGrid, behaviorGrid, goodnessGrid, range);
	}
	else {
		printError("Invalid bias value.", -2, acousticParams["timestamp"]);
	}
}

/**
 * Simply sums the cells within sensorRange of each cell on the BehaviorGrid.  Results are written to goodnessGrid.
 */
void goodFish(Grid* topographyGrid, Grid* behaviorGrid, Grid* goodnessGrid, double rng) {
	int range = (int) rng;
	int cols = topographyGrid->cols;
	int rows = topographyGrid->rows;
	int rstart=0, cstart=0, rdist=0,cdist=0,rend=0,cend=0;

	for (int r = 0; r<rows; r++) {
		rstart = max(r-range, 0);
		rend = min(rows-1,r+range);
		rdist = rend-rstart + 1;
		for (int c = 0; c<cols; c++) {
			cstart = max(c-range,0);
			cend = min(cols-1, c+range);
			cdist = cend-cstart + 1;
			//cout<<"\nr:"<<r<<"\nc"<<c<<"\nrstart:"<<rstart<<"\nrend:"<<rend<<"\nrdist:"<<rdist<<"\ncstart:"<<cstart<<"\ncend:"<<cend<<"\ncdist:"<<cdist<<"\n\n\n";
			goodnessGrid->data(r,c)= behaviorGrid->data.block(rstart, cstart, rdist, cdist).sum();
		}
	}
}


void goodViz(Grid* topographyGrid, Grid* behaviorGrid, Grid* goodnessGrid, double rng) {
	int range = (int) rng;
	int cols = topographyGrid->cols;
	int rows = topographyGrid->rows;
	int rstart=0, cstart=0, rdist=0,cdist=0,rend=0,cend=0;

	for (int r = 0; r<rows; r++) {
		rstart = max(r-range, 0);
		rend = min(rows-1,r+range);
		rdist = rend-rstart + 1;
		for (int c = 0; c<cols; c++) {
			cstart = max(c-range,0);
			cend = min(cols-1, c+range);
			cdist = cend-cstart + 1;
			//cout<<"\nr:"<<r<<"\nc"<<c<<"\nrstart:"<<rstart<<"\nrend:"<<rend<<"\nrdist:"<<rdist<<"\ncstart:"<<cstart<<"\ncend:"<<cend<<"\ncdist:"<<cdist<<"\n\n\n";
			goodnessGrid->data(r,c)= behaviorGrid->data.block(rstart, cstart, rdist, cdist).sum();
		}
	}
}


void goodVizOfFish(Grid* topographyGrid, Grid* behaviorGrid, Grid* goodnessGrid, double rng) {
	int range = (int) rng;
		int cols = topographyGrid->cols;
		int rows = topographyGrid->rows;
		int rstart=0, cstart=0, rdist=0,cdist=0,rend=0,cend=0;

		for (int r = 0; r<rows; r++) {
			rstart = max(r-range, 0);
			rend = min(rows-1,r+range);
			rdist = rend-rstart + 1;
			for (int c = 0; c<cols; c++) {
				cstart = max(c-range,0);
				cend = min(cols-1, c+range);
				cdist = cend-cstart + 1;
				//cout<<"\nr:"<<r<<"\nc"<<c<<"\nrstart:"<<rstart<<"\nrend:"<<rend<<"\nrdist:"<<rdist<<"\ncstart:"<<cstart<<"\ncend:"<<cend<<"\ncdist:"<<cdist<<"\n\n\n";
				goodnessGrid->data(r,c)= behaviorGrid->data.block(rstart, cstart, rdist, cdist).sum();
			}
		}
	}


std::pair<double,double> offset (std::pair<double,double> *point) {
	std::pair<double,double> newPoint = std::make_pair(point->first+.5,point->second+.5);
	return newPoint;
}
/**
 * Gets a set of cell locations that intersect a beam between the origin cell and a target cell. Pairs are configured as (x,y), (column,row).
 */
std::set<std::pair<double,double>> getCells(std::pair <double,double> *origin, std::pair <double,double> *target) {
	std::set<std::pair<double,double> > pairs;

	//Calculate the slope
	double ox=origin->first, oy=origin->second, tx=target->first,ty=target->second;
	if(origin!=target) {
		double m = (ty - oy) / (tx - ox);
		double absm = abs(m);
		//Handle INF/NAN values.
		if(absm == std::numeric_limits<double>::infinity()) {
			if(m>0) {
				m = 999999;
			}
			else {
				m = -999999;
			}
			absm = abs(m);
		}
		// assume the sensor is in the middle of the cell
		std::pair<double,double> offsetO = offset(origin);
		double b = offsetO.second - m * offsetO.first;
		double lowerX = min(ox, tx);
		double upperX = max(ox, tx);
		double lowerY = min(oy, ty);
		double upperY = max(oy, ty);
		double startX = lowerX, endX = upperX;
		double startY = lowerY, endY = upperY;
		double temp = 0;
		double y = 0, y1 = 0;
		double x = 0, x1 = 0;

		//====================================Calculate Cells==============================================
		//Steep Slopes
		if(absm > 1) {

			for(y = startY+1; y <= endY; y++) {
				x = (y - b) / m;
				x1 = floor(x);
				//cout<<"Y: "<<y <<", X: "<<x<<";  added("<<x1<<","<<y-1<<") and ("<<x1<<","<<y<<")\n";
				pairs.insert(std::make_pair(x1, y-1));
				pairs.insert(std::make_pair(x1, y));
			}
		}
		//Slow Slopes
		else if(absm < 1) {
			for(x = startX+1; x <= endX; x++) {
				y = m * x + b;
				y1 = floor(y);
				pairs.insert(std::make_pair(x-1, y1));
				pairs.insert(std::make_pair(x, y1));
			}
		}
		//Slope == 1
		else {
			startX = lowerX;
			endX = upperX;
			for(x = startX; x <= endX; x++) {
				y = m * x + b;
				pairs.insert(std::make_pair(x, y));
			}
		}
		if(pairs.size()>0) {
			pairs.erase(pairs.begin());
			pairs.erase(*origin);
		}
		pairs.insert(*target);
		pairs.erase(*origin);
	}
	return pairs;

}

/**
 * Calculates the visibility Grid for a cell at r,c on the topographyGrid.  Should be a grid containing the max visible depth.
 */
Eigen::MatrixXd calcPercentViz(Grid* topographyGrid, int rStart, int cStart, int rng) {
	int size = 2 * rng + 1;
	Eigen::MatrixXd slopeGrid;
	Eigen::MatrixXd vizGrid;
	Eigen::MatrixXd localTopo;
	//Vectors from -rng to rng, of length size
	Eigen::VectorXd refx = refx.LinSpaced(size, -rng, rng);
	Eigen::VectorXd refy = refx.LinSpaced(size, -rng, rng);
	Eigen::MatrixXd X = refx.replicate(1,size);
	Eigen::MatrixXd Y = refy.replicate(1,size);
	X.transposeInPlace();
	//set slopeGrid to a gird containing each cell's linear distance from the center cell.
	slopeGrid = Y.cwiseAbs2() + X.cwiseAbs2();
	slopeGrid = slopeGrid.cwiseSqrt();

	//copy out the block of topography we're interested in.
	localTopo = topographyGrid->data.block(rStart, cStart, size, size);
	vizGrid << localTopo(rng,rng);
	vizGrid.replicate(size,size);
	//vizGrid now contains the depth deltas from the origin cell
	vizGrid = localTopo - vizGrid;
	//slopeGrid now contains depth deltas divided by distance deltas, aka the slope from the center cell to each cell.
	slopeGrid = vizGrid.cwiseQuotient(slopeGrid);




	return slopeGrid;
}

double add (int n) {

}
