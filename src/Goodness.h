#pragma once

#include <Dense>
#include <set>
#include "Grid.h"
#include "Utility.h"
using namespace std;

	void calculateGoodnessGrid(Grid* topographyGrid, Grid* behaviorGrid, Grid* goodnessGrid, double bias, double range);
	std::set<std::pair<int,int>> getCells(std::pair <int,int> *origin, std::pair <int,int> *target);
	Eigen::MatrixXd calcPercentViz(Grid* topographyGrid, int rStart, int cStart, int rng);
	void goodFish(Grid* topographyGrid, Grid* behaviorGrid, Grid* goodnessGrid, double range);
	void goodViz(Grid* topographyGrid, Grid* behaviorGrid, Grid* goodnessGrid, double range);
	void goodVizOfFish(Grid* topographyGrid, Grid* behaviorGrid, Grid* goodnessGrid, double range);
	std::pair<int,int> offset (std::pair<int,int> *point);
