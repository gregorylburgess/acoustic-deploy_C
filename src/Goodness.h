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
                        Grid* goodnessGrid,
                        Eigen::MatrixXd* suppressionReference,
                        Eigen::MatrixXd* detectionGradient,
                        Eigen::MatrixXd* distanceGradient,
                        int bias, int sensorRange,
                        int startingRow, int startingCol,
                        int rowDist, int colDist);
std::vector<std::pair<int, int>> getCells(const std::pair <int, int> *origin,
                        const std::pair <int, int> *target);
void calcVizGrid(Grid* topographyGrid, Eigen::MatrixXd* distGradient,
                        Eigen::MatrixXd* solutionGrid,
                        Eigen::MatrixXd* localTopo, Eigen::MatrixXd* tempGrid,
                        int row, int col, int sensorDetectionRange);
double cdist(double mean, double sd, double x);
double cdistPartition(double mean, double sd, double start, double end);
void getStats(Grid* unsuppressedGoodnessGrid, Grid* suppressedGoodnessGrid,
                        Eigen::MatrixXd* bestSensors, int sensorRange,
                        double* sparsity, double* absRecoveryRate,
                        double* uniqueRecoveryRate, Grid* coverageGrid);
void goodness_Fish(Grid* topographyGrid, Grid* behaviorGrid,
                        Grid* goodnessGrid, Eigen::MatrixXd* detectionGradient,
                        Eigen::MatrixXd* distanceGradient,
                        Eigen::MatrixXd* suppressionReference,
                        int sensorRange, int startingRow, int startingCol,
                        int rowDist, int colDist);
void goodness_Viz(Grid* topographyGrid, Grid* behaviorGrid, Grid* goodnessGrid,
                        Eigen::MatrixXd* detectionGradient,
                        Eigen::MatrixXd* distanceGradient,
                        Eigen::MatrixXd* suppressionReference,
                        int sensorRange, int startingRow, int startingCol,
                        int rowDist, int colDist);
void goodness_VizOfFish(Grid* topographyGrid, Grid* behaviorGrid,
                        Grid* goodnessGrid, Eigen::MatrixXd* detectionGradient,
                        Eigen::MatrixXd* distanceGradient,
                        Eigen::MatrixXd* suppressionReference,
                        int sensorRange, int startingRow, int startingCol,
                        int rowDist, int colDist);
void makeDetectionGradient(Eigen::MatrixXd* detectionGradient,
                        Eigen::MatrixXd* distGradient,
                        double sensorPeakDetectionProbability,
                        double SDofSensorDetectionRange);
void makeDistGradient(Eigen::MatrixXd* distGradient, int rng);
void makeDistGradient(Eigen::MatrixXd* distGradient, int rng, double scalingFactor);
double normalProb(double peak, double sd, double x);
std::pair<int, int> offset(const std::pair<int, int> *point);
void selectTopSensorLocations(Grid* topographyGrid, Grid* behaviorGrid, Grid* goodnessGrid,
                        Grid* goodnessGridPerfect, Eigen::MatrixXd* bestSensors,
                        Eigen::MatrixXd* userSensors,
                        Eigen::MatrixXd* suppressionReference,
                        Eigen::MatrixXd* detectionGradient,
                        Eigen::MatrixXd* distanceGradient,
                        int numSensorsToPlace,
                        int sensorRange, int bias, double suppressionRangeFactor,
                        double suppressionDiameter,
                        double sensorPeakDetectionProbability,
                        double SDofSensorDetectionRange,
                        std::string timeStamp);
void suppress(Grid* topographyGrid, Grid* behaviorGrid, Grid* goodnessGrid,
                        Eigen::MatrixXd* suppressionReference,
                        Eigen::MatrixXd* detectionGradient,
                        Eigen::MatrixXd* distanceGradient,
                        Eigen::MatrixXd* suppressionDetectionGradient,
                        Eigen::MatrixXd* suppressionDistanceGradient,
                        int row, int col, int sensorRange, int bias,
                        double suppressionRangeFactor,
                        double sensorPeakDetectionProbability,
                        double SDofSensorDetectionRange,
                        double suppressionDiameter,
                        double detectionDiameter, std::string timeStamp);
void suppressionExact(Grid* behaviorGrid, Grid* topographyGrid,
                        Grid* goodnessGrid,
                        Eigen::MatrixXd* suppressionReference,
                        Eigen::MatrixXd* detectionGradient,
                        Eigen::MatrixXd* distanceGradient,
                        Eigen::MatrixXd* suppressionGradient,
                        int bias, int row, int col, int sensorRange,
                        int suppressionDiameter, std::string timeStamp);
void suppressionQuick(Grid* gridToSuppress,
                        Eigen::MatrixXd* suppressionReference,
                        Eigen::MatrixXd* suppressionGradient,
                        int row, int col, int sensorRange,
                        int suppressionDiameter);
#endif  // SRC_GOODNESS_H_
