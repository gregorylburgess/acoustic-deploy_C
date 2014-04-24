#pragma once
#include "Grid.h"
#include <unordered_map>

double bivariateNorm(double x, double y, double mux, double muy, double sdx, double sdy, double cor);
Grid* fish(Grid* topographyGrid);
double zero(double x);
