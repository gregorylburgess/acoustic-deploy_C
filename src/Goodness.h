#pragma once
#include <set>
#include "Grid.h"
#include "Utility.h"
using namespace std;

	void calculateGoodnessGrid(Grid* topographyGrid, Grid* behaviorGrid, Grid* goodnessGrid, double bias, double range);
	std::set<std::pair<double, double>> getCells(std::pair <double, double> *origin, std::pair <double, double> *target);
	void goodFish(Grid* topographyGrid, Grid* behaviorGrid, Grid* goodnessGrid, double range);
	void goodViz(Grid* topographyGrid, Grid* behaviorGrid, Grid* goodnessGrid, double range);
	void goodVizOfFish(Grid* topographyGrid, Grid* behaviorGrid, Grid* goodnessGrid, double range);
	std::pair<double,double> offset (std::pair<double,double> *point);
