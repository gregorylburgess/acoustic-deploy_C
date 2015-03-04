#pragma once

#include <Dense>
#include <set>
#include "Grid.h"
#include "Utility.h"
using namespace std;
	void calculateGoodnessGrid(Grid* topographyGrid, Grid* behaviorGrid, Grid* goodnessGrid, int bias, int range,
					double peak, double sd);
	std::vector<std::pair<int,int>> getCells(const pair <int,int> *origin, const pair <int,int> *target);
	void calcVizGrid(Grid* topographyGrid, Eigen::MatrixXd* distGradient, Eigen::MatrixXd* solutionGrid,
					Eigen::MatrixXd* localTopo, Eigen::MatrixXd* temp, int rStart, int cStart, int rng);
	double cdist(double mean, double sd, double x);
	double cdistPartition(double mean, double sd, double start, double end);
	void goodFish(Grid* topographyGrid, Grid* behaviorGrid, Grid* goodnessGrid, Eigen::MatrixXd* distanceGradient,
					Eigen::MatrixXd* detectionGradient, double sensorRange, double sensorPeakDetectionProbability,
					double SDofSensorDetectionRange);
	void goodViz(Grid* topographyGrid, Grid* behaviorGrid, Grid* goodnessGrid, Eigen::MatrixXd* distanceGradient,
					Eigen::MatrixXd* detectionGradient, double sensorRange, double sensorPeakDetectionProbability,
					double SDofSensorDetectionRange);
	void goodVizOfFish(Grid* topographyGrid, Grid* behaviorGrid, Grid* goodnessGrid, Eigen::MatrixXd* distanceGradient,
					Eigen::MatrixXd* detectionGradient, double sensorRange, double sensorPeakDetectionProbability,
					double SDofSensorDetectionRange);
	void makeDetectionGradient(Eigen::MatrixXd* detectionGradient, Eigen::MatrixXd* distGradient, double peak, double sd);
	void makeDistGradient(Eigen::MatrixXd* distGradient,int rng);
	double normalProb(double peak, double sd, double x);
	std::pair<int,int> offset (const pair<int,int> *point);
