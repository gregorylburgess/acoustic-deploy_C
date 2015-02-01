#pragma once

#include <Dense>
#include <set>
#include "Grid.h"
#include "Utility.h"
using namespace std;

	void calculateGoodnessGrid(Grid* topographyGrid, Grid* behaviorGrid, Grid* goodnessGrid, int bias, int range);
	std::set<std::pair<int,int>> getCells(const pair <int,int> *origin, const pair <int,int> *target);
	Eigen::MatrixXd calcPercentViz(Grid* topographyGrid, int rStart, int cStart, int rng);
	void goodFish(Grid* topographyGrid, Grid* behaviorGrid, Grid* goodnessGrid, double range);
	void goodViz(Grid* topographyGrid, Grid* behaviorGrid, Grid* goodnessGrid, double range);
	void goodVizOfFish(Grid* topographyGrid, Grid* behaviorGrid, Grid* goodnessGrid, double range);
	std::pair<int,int> offset (const pair<int,int> *point);
