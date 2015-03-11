/** Copyright 2015 Greg Burgess. **/
#ifndef SRC_FISHMODEL_H_
#define SRC_FISHMODEL_H_
#pragma once
#include "Grid.h"
#include <unordered_map>
namespace std {
double bivariateNorm(double x, double y, double mux, double muy, double sdx,
                     double sdy, double cor);
Grid* populateBehaviorGrid(Grid* topographyGrid, Grid* behaviorGrid);
double isNonNeg(double x);
double isNonPos(double x);
}  // namespace std
#endif  // SRC_FISHMODEL_H_
