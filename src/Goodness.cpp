#include <algorithm>
#include <cmath>
#include <stdlib.h>
#include <iostream>
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

}


std::pair<double,double> offset (std::pair<double,double> *point) {
	cout<<0;
	std::pair<double,double> newPoint = std::make_pair(point->first+.5,point->second+.5);
	return newPoint;
}
/**
 * Gets a set of cell locations that intersect a beam between the origin cell and a target cell. Pairs are configured as (x,y), (column,row).
 */
std::set<std::pair<double,double>> getCells(std::pair <double,double> *origin, std::pair <double,double> *target) {
	std::set<std::pair<double,double> > pairs;

	double ox=origin->first, oy=origin->second, tx=target->first,ty=target->second;
	if(origin!=target) {
		double m = (ty - oy) / (tx - ox);

		cout<< m<<"\n";
		double absm = abs(m);
		if(absm == NAN) {
			if(m>0) {
				m = 999999;
			}
			else {
				m = -999999;
			}
		}
		// assume the sensor is in the middle of the cell
		std::pair<double,double> offsetO = offset(origin);
		double b = offsetO.second - m * offsetO.first;
		double lowerX = min(ox, tx);
		double upperX = max(ox, tx);
		double lowerY = min(oy, ty);
		double upperY = max(oy, ty);
		double startX = 0, endX = 0;
		double startY = 0, endY = 0;
		double temp = 0;
		double y = 0, y1 = 0;
		double x = 0, x1 = 0;
		//STEEP SLOPES
		if(absm > 1) {
			startY = lowerY;
			endY = upperY;
			if(m < 0) {
				temp = startY;
				startY = endY;
				endY = temp;
			}
			for(y = startY+1; y <= endY; y++) {
				x = (y - b) / m;
				x1 = floor(x);
				pairs.insert(std::make_pair(x1, y-1));
				pairs.insert(std::make_pair(x1, y1));
			}
		}
		//Slow Slopes
		else if(absm < 1) {
			startX = lowerX;
			endX = upperX;
			if(m < 0) {
				temp = lowerX;
				startX = upperX;
				endX = temp;
			}
			for(x = startX+1; x <= endX; x++) {
				y = m * x + b;
				y1 = floor(y);
				pairs.insert(std::make_pair(x-1, y1));
				pairs.insert(std::make_pair(x, y1));
			}
		}
		else {
			for(x = startX+1; x <= endX; x++) {
				cout<<y<<"\n";
				y = m * x + b;
				pairs.insert(std::make_pair(x, y));
			}
		}
	}
	if(pairs.size()>0) {
		pairs.erase(pairs.begin());
	}
	return pairs;

}
