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
#include <utility>
#include <vector>
#include "Grid.h"
#include "Utility.h"
void calculateGoodnessGrid(Grid* topographyGrid, Grid* behaviorGrid,
                           Grid* goodnessGrid, int bias, int range,
                           double peak, double sd);
std::vector<std::pair<int, int>> getCells(const std::pair <int, int> *origin,
                                         const std::pair <int, int> *target);
void calcVizGrid(Grid* topographyGrid, Eigen::MatrixXd* distGradient,
                 Eigen::MatrixXd* solutionGrid,
                 Eigen::MatrixXd* localTopo, Eigen::MatrixXd* temp, int rStart,
                 int cStart, int rng);
double cdist(double mean, double sd, double x);
double cdistPartition(double mean, double sd, double start, double end);
void goodFish(Grid* topographyGrid, Grid* behaviorGrid, Grid* goodnessGrid,
              Eigen::MatrixXd* distanceGradient,
              Eigen::MatrixXd* detectionGradient, double sensorRange,
              double sensorPeakDetectionProbability,
              double SDofSensorDetectionRange);
void goodViz(Grid* topographyGrid, Grid* behaviorGrid, Grid* goodnessGrid,
             Eigen::MatrixXd* distanceGradient,
             Eigen::MatrixXd* detectionGradient, double sensorRange,
             double sensorPeakDetectionProbability,
             double SDofSensorDetectionRange);
void goodVizOfFish(Grid* topographyGrid, Grid* behaviorGrid,
                   Grid* goodnessGrid, Eigen::MatrixXd* distanceGradient,
                   Eigen::MatrixXd* detectionGradient, double sensorRange,
                   double sensorPeakDetectionProbability,
                   double SDofSensorDetectionRange);
void makeDetectionGradient(Eigen::MatrixXd* detectionGradient,
                           Eigen::MatrixXd* distGradient,
                           double sensorPeakDetectionProbability,
                           double SDofSensorDetectionRange);
void makeDistGradient(Eigen::MatrixXd* distGradient, int rng);
double normalProb(double peak, double sd, double x);
std::pair<int, int> offset(const std::pair<int, int> *point);
void selectTopSpots(Grid* goodnessGrid, Eigen::MatrixXd* bestSensors,
                    Eigen::MatrixXd* userSensors,
                    int numSensorsToPlace, int sensorRange,
                    double sensorPeakDetectionProbability,
                    double SDofSensorDetectionRange);
#endif  // SRC_GOODNESS_H_
