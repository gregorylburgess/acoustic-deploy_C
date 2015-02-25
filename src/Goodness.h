#pragma once

#include <Dense>
#include <set>
#include "Grid.h"
#include "Utility.h"
using namespace std;
	void calculateGoodnessGrid(Grid* topographyGrid, Grid* behaviorGrid, Grid* goodnessGrid, int bias, int range);
	std::vector<std::pair<int,int>> getCells(const pair <int,int> *origin, const pair <int,int> *target);
	void calcVizGrid(Grid* topographyGrid, Eigen::MatrixXd* distGradient, Eigen::MatrixXd* solutionGrid, Eigen::MatrixXd* localTopo, Eigen::MatrixXd* temp, int rStart, int cStart, int rng);
	void goodFish(Grid* topographyGrid, Grid* behaviorGrid, Grid* goodnessGrid, double range);
	void goodViz(Grid* topographyGrid, Grid* behaviorGrid, Grid* goodnessGrid, double range);
	void goodVizOfFish(Grid* topographyGrid, Grid* behaviorGrid, Grid* goodnessGrid, double range);
	void makeDistGradient(Eigen::MatrixXd* distGradient,int rng);
	std::pair<int,int> offset (const pair<int,int> *point);
