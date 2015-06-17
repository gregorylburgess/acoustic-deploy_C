//============================================================================
// Name        : Goodness.h
// Author      : Greg Burgess
// Version     : 1.0
// Copyright   : Copyright 2014
// Description : Functions for various goodness computations.
//============================================================================
#ifndef SRC_GOODNESS_H_
#define SRC_GOODNESS_H_
#include <Dense>
#include <set>
#include <string>
#include <utility>
#include <vector>
#include "Grid.h"
#include "Utility.h"
void calculateGoodnessGrid(Grid* topographyGrid, Grid* behaviorGrid,
                        Grid* goodnessGrid, Grid* coverageGrid, int bias,
                        int range, double peak, double sd);
std::vector<std::pair<int, int>> getCells(const std::pair <int, int> *origin,
                        const std::pair <int, int> *target);
void calcVizGrid(Grid* topographyGrid, Eigen::MatrixXd* distGradient,
                        Eigen::MatrixXd* solutionGrid,
                        Eigen::MatrixXd* localTopo, Eigen::MatrixXd* temp,
                        int rStart, int cStart, int rng);
double cdist(double mean, double sd, double x);
double cdistPartition(double mean, double sd, double start, double end);
void getStats(Grid* unsuppressedGoodnessGrid, Grid* suppressedGoodnessGrid,
                        Eigen::MatrixXd* bestSensors, int sensorRange,
                        double* sparsity, double* absRecoveryRate,
                        double* uniqueRecoveryRate, Grid* coverageGrid);
void goodness_Fish(Grid* topographyGrid, Grid* behaviorGrid,
                        Grid* goodnessGrid, Grid* coverageGrid,
                        Eigen::MatrixXd* distanceGradient,
                        Eigen::MatrixXd* detectionGradient, int sensorRange,
                        double sensorPeakDetectionProbability,
                        double SDofSensorDetectionRange);
void goodness_Viz(Grid* topographyGrid, Grid* behaviorGrid, Grid* goodnessGrid,
                        Grid* coverageGrid,
                        Eigen::MatrixXd* distanceGradient,
                        Eigen::MatrixXd* detectionGradient, int sensorRange,
                        double sensorPeakDetectionProbability,
                        double SDofSensorDetectionRange);
void goodness_VizOfFish(Grid* topographyGrid, Grid* behaviorGrid,
                        Grid* goodnessGrid, Grid* coverageGrid,
                        Eigen::MatrixXd* distanceGradient,
                        Eigen::MatrixXd* detectionGradient, int sensorRange,
                        double sensorPeakDetectionProbability,
                        double SDofSensorDetectionRange);
void makeDetectionGradient(Eigen::MatrixXd* detectionGradient,
                        Eigen::MatrixXd* distGradient,
                        double sensorPeakDetectionProbability,
                        double SDofSensorDetectionRange);
void makeDistGradient(Eigen::MatrixXd* distGradient, int rng);
double normalProb(double peak, double sd, double x);
std::pair<int, int> offset(const std::pair<int, int> *point);
void selectTopSensorLocations(Grid* topographyGrid, Grid* behaviorGrid, Grid* goodnessGrid,
                        Grid* goodnessGridPerfect,
                        Grid* coverageGrid, Eigen::MatrixXd* bestSensors,
                        Eigen::MatrixXd* userSensors,
                        int numSensorsToPlace,
                        int sensorRange, int bias, double suppressionRangeFactor,
                        double sensorPeakDetectionProbability,
                        double SDofSensorDetectionRange,
                        std::string timeStamp);
void suppress(Grid* topographyGrid, Grid* behaviorGrid, Grid* goodnessGrid,
                        Grid* coverageGrid, int row, int col,
                        int sensorRange, int bias, double suppressionRangeFactor,
                        double sensorPeakDetectionProbability,
                        double SDofSensorDetectionRange, std::string timeStamp);
void suppressionExact(Grid* behaviorGrid, Grid* topographyGrid,
                        Grid* goodnessGrid, Grid* coverageGrid, int bias,
                        int row, int col, int sensorRange,
                        int suppressionDiameter, std::string timeStamp);
void suppressionQuick(Grid* gridToSuppress, Grid* coverageGrid,
                        Eigen::MatrixXd* suppressionGradient, int row, int col,
                        int sensorRange, int suppressionDiameter);
#endif  // SRC_GOODNESS_H_
