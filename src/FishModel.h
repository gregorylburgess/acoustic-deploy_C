//============================================================================
// Name        : FishModel.cpp
// Author      : Greg Burgess
// Version     : 1.0
// Copyright   : Copyright 2014
// Description : Functions for modeling fish behavior.
//============================================================================
#ifndef SRC_FISHMODEL_H_
#define SRC_FISHMODEL_H_
#include "Grid.h"
#include <unordered_map>
double bivariateNorm(double x, double y, double mux, double muy, double sdx,
                     double sdy, double cor);
Grid* populateBehaviorGrid(Grid* topographyGrid, Grid* behaviorGrid,
                     int cellSize, double ousdx, double ousdy, double oucor,
                     double mux, double muy, int fishmodel);
double isNonNeg(double x);
double isNonPos(double x);
#endif  // SRC_FISHMODEL_H_
