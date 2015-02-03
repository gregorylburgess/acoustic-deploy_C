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

class sortByDist {
	public:
		int r, c;
		sortByDist(int r, int c) : r(r), c(c){}

		double distDelta(pair<int,int> point) {
			return sqrt(pow(r-(point.first),2)+pow(c-(point.second),2));
		}

		bool operator() (pair<int,int> lhs, pair<int,int> rhs) {
			return distDelta(lhs) < distDelta(rhs);
		}
};

void calculateGoodnessGrid(Grid* topographyGrid, Grid* behaviorGrid, Grid* goodnessGrid, int bias, int range) {
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


pair<int,int> offset (const pair<int,int> *point) {
	pair<int,int> newPoint = make_pair(point->first+.5,point->second+.5);
	return newPoint;
}
/**
 * Gets a set of cell locations that intersect a beam between the origin cell and a target cell. Pairs are configured as (x,y), (column,row).
 */
vector<pair<int,int>> getCells(const pair <int,int> *origin, const pair <int,int> *target) {
	set<pair<int,int> > pairs;
	//Calculate the slope
	int ox=origin->first, oy=origin->second, tx=target->first,ty=target->second;
	if(ox!=tx || oy!=ty) {
		double slopeNumerator = (ty - oy),
			   slopeDenominator = (tx - ox),
			   m = 0;
		//Handle INF/NAN values.
		if(slopeDenominator == 0) {
			if(slopeNumerator > 0) {
				m = 999999;
			}
			else {
				m = -999999;
			}
		}
		else {
			m = slopeNumerator/slopeDenominator;
		}
		double absm = abs(m);
		// assume the sensor is in the middle of the cell
		pair<int,int> offsetO = offset(origin);
		double b = offsetO.second - m * offsetO.first;
		int lowerX = min(ox, tx);
		int upperX = max(ox, tx);
		int lowerY = min(oy, ty);
		int upperY = max(oy, ty);
		int startX = lowerX, endX = upperX;
		int startY = lowerY, endY = upperY;
		int y = 0, y1 = 0;
		int x = 0, x1 = 0;

		//====================================Calculate Cells==============================================
		//Steep Slopes
		if(absm > 1) {

			for(y = startY+1; y <= endY; y++) {
				x = (y - b) / m;
				x1 = floor(x);
				//cout<<"Y: "<<y <<", X: "<<x<<";  added("<<x1<<","<<y-1<<") and ("<<x1<<","<<y<<")\n";
				pairs.insert(make_pair(x1, y-1));
				pairs.insert(make_pair(x1, y));
			}
		}
		//Slow Slopes
		else if(absm < 1) {
			for(x = startX+1; x <= endX; x++) {
				y = m * x + b;
				y1 = floor(y);
				pairs.insert(make_pair(x-1, y1));
				pairs.insert(make_pair(x, y1));
			}
		}
		//Slope == 1
		else {
			startX = lowerX;
			endX = upperX;
			for(x = startX; x <= endX; x++) {
				y = m * x + b;
				pairs.insert(make_pair(x, y));
			}
		}
		if(pairs.size()>0) {
			pairs.erase(pairs.begin());
			pairs.erase(*origin);
		}
		pairs.insert(*target);
		pairs.erase(*origin);

		//copy to vector
		vector<pair<int,int>> vpairs(pairs.begin(), pairs.end());
		//Sort pairs by distance from origin
		sortByDist sorter(origin->first,origin->second);
		sort(vpairs.begin(),vpairs.end(),sorter);
		//cout<< "Returning vector of size "<<vpairs.size()<<"\n";

		return vpairs;
	}
	else {
		vector<pair<int,int>> vpairs;
		return vpairs;
	}
}

/**
 * Calculates the visibility Grid for a cell at r,c on the topographyGrid.  Should be a grid containing the max visible depth.
 */
Eigen::MatrixXd calcPercentViz(Grid* topographyGrid, int r, int c, int rng) {
	int size = 2 * rng + 1,
		i=0,
		j=0;
	Eigen::MatrixXd slopeGrid;
	Eigen::MatrixXd solutionGrid;
	Eigen::MatrixXd vizGrid;
	Eigen::MatrixXd localTopo;
	Eigen::MatrixXd X;
	Eigen::MatrixXd Y;
	//Vectors from -rng to rng, of length size
	Eigen::VectorXd refx = refx.LinSpaced(size, -rng, rng);
	Eigen::VectorXd refy = refy.LinSpaced(size, -rng, rng);
	vector<pair<int,int>> cells;

	Y = refy.replicate(1,size);
	X = refx.replicate(1,size);
	X.transposeInPlace();

	//set slopeGrid to a gird containing each cell's linear distance from the center cell.
	slopeGrid = Y.cwiseAbs2() + X.cwiseAbs2();
	slopeGrid = slopeGrid.cwiseSqrt();
	slopeGrid(rng,rng) = 1;

	//copy out the block of topography we're interested in.
	localTopo = topographyGrid->data.block(r-rng, c-rng, size, size);
	//assign dimensions, and set all values in vizGrid to the center cell's depth
	vizGrid.resize(size,size);
	solutionGrid.resize(size,size);
	vizGrid.setConstant(localTopo(rng,rng));
	//vizGrid now contains the depth deltas from the origin cell
	vizGrid = localTopo - vizGrid;
	//slopeGrid now contains depth deltas divided by distance deltas, aka the slope from the center cell to each cell.
	slopeGrid = vizGrid.cwiseQuotient(slopeGrid);
	//cout << slopeGrid <<"\n\n";
	/** slopeGrid now has the slope from all cells to the center
	 *  viz grid has depth deltas
	 */

	set <pair<int,int>> unprocessedCells;
	vector<pair<int,int>> interveningCells;
	pair<int,int> origin = make_pair(rng,rng);
	double maxSlope=0;
	for(i=0;i<size;i++) {
		for (j=0;j<size;j++)
			unprocessedCells.insert((const pair<int,int>) make_pair(i,j));

	}
	//cout<<"Added cells to process\n";
	unprocessedCells.erase(origin);
	maxSlope=0;
	while (unprocessedCells.size() > 0) {
		//cout<<"Getting cells\n";
		//cout<<unprocessedCells.cbegin()->first <<","<<unprocessedCells.cbegin()->second;
		interveningCells = getCells(&origin, &*unprocessedCells.cbegin());
		//cout<<"\nGot cells\n";
		if(interveningCells.size() > 0) {
			auto iterator = interveningCells.cbegin();
			pair<int,int> it = *iterator;
			//cout<<"Getting slope\n";
			i=it.first;
			j=it.second;
			//cout<<"Getting "<<i<<","<<j<<"\n";
			maxSlope = slopeGrid(i,j);
			//cout<<"Processing cells\n";
			for (auto iterator = interveningCells.begin(); iterator != interveningCells.cend(); ++iterator) {
				//cout << iterator->first << "," << iterator->second << "\n";
				maxSlope = max(maxSlope, slopeGrid(iterator->first,iterator->second));
				solutionGrid(iterator->first,iterator->second) = maxSlope;
				unprocessedCells.erase(*iterator);
			}
			//cout<<"Erasing extra cell \n";
			unprocessedCells.erase(*interveningCells.crend());
		}
	}
	//cout<<"Returning";
	return solutionGrid;
}

