//============================================================================
// Name        : FishModel.cpp
// Author      : Greg Burgess
// Version     : 1.0
// Copyright   : Copyright 2014
// Description : Functions for modeling fish behavior.
//============================================================================
#include <stdlib.h>
#include <math.h>
#include <Dense>
#include <iostream>
#include <fstream>
#include <unordered_map>
#include "GlobalVars.h"
#include "Grid.h"
#include "Utility.h"

/**
 * Computes the bivariate normal distribution as a probability density for a
 * given x,y pair.
 * @param x The x coordinate of the point to calculate the probability for.
 * @param y The y coordinate of the point to calculate the probability for.
 * @param mux The location of the x-mean as a percentage of the x axis.  The
 *           center of the home range on the x axis.
 * @param muy The location of the y-mean as a percentage of the y axis.  The
 *           center of the home range on the y axis.
 * @param sdx The standard deviation of x as a percentage of the x axis.  The
 *           size of the home range on the x axis.
 * @param sdy The standard deviation of y as a percentage of the y axis.  The
 *           size of the home range on the y axis
 * @param cor The correlation between sdx and sdy.  Controls the tilt of the
 *           homerange.  Valid values are between 1 and -1
 * @return The value of the bivariate normal distribution at cell (x,y).
 */
double bivariateNorm(double x, double y, double mux, double muy, double sdx,
                     double sdy, double cor) {
    // double z =
    // (x-mux)^2/sdx^2-(2cor*(x-mux)*(y-muy))/(sdx*sdy)+ y-muy)^2/sdy^2;
    // return 1/(2*PI*sdx*sdy*sqrt(1-cor^2)) ^ -z/2(1-cor^2);
    double nx = x - mux;
    double ny = y - muy;
    double ncor = 1 - pow(cor, 2);
    double nsdxy = sdx * sdy;
    double z = pow(nx / sdx, 2) - (2 * cor * nx * ny) / nsdxy +
               pow(ny / sdy, 2);
    double base = (1 / (2 * M_PI * nsdxy * sqrt(ncor)));
    double power = (-1 * z / (2 * ncor));
    double val = pow(base, power);
    return val;
}


/**
 * Checks if x is non-negative, returning 1 if it is, and zero otherwise.
 * @param x The value to check.
 * @return 1 if x is non-negative, 0 otherwise.
 */
double isNonNeg(double x) {
    if (x >= 0) {
        return 1;
    }
    return 0;
}

/**
 * Checks if x is non-positive, returning 1 if it is, and zero otherwise.
 * @param x The value to check.
 * @return 1 if x is non-positive, 0 otherwise.
 */
double isNonPos(double x) {
    if (x <= 0) {
        return 1;
    }
    return 0;
}

/**
 * Populates a behaviorGrid with the probability of some tag releasing a ping
 * from each cell over the course of entire the observation period.
 * @param topographyGrid A pointer to a Grid object containing topographic
 *      information for the area of study.
 * @param behaviorGrid A pointer to a zero-initalized Grid object of the same
 *      size as the topographyGrid.
 */
void populateBehaviorGrid(Grid* topographyGrid, Grid* behaviorGrid,
                          int bias, int cellSize, double ousdx, double ousdy,
                          double oucor, double mux, double muy,
                          int fishmodel) {
    int rows = topographyGrid -> rows - 2 * border;
    int cols = topographyGrid -> cols - 2 * border;
    // Random Walk model
    if (fishmodel == 0 || bias == 2) {
        if (debug) {
            std::cout << "\nUsing RW model\n";
        }
        Eigen::MatrixXd temp;
        temp.resize(rows, cols);
        temp.setOnes();
        behaviorGrid -> data.block(border, border, rows, cols) = temp;
    // Ornstein–Uhlenbeck model
    } else if (fishmodel == 1) {
        if (debug) {
            std::cout << "\nUsing OU model\n";
        }
        double varx = pow(ousdx, 2);
        double vary = pow(ousdy, 2);
        double covxy = oucor * ousdx * ousdy;

        Eigen::VectorXd tx = tx.LinSpaced(rows, 1, rows) * cellSize;
        Eigen::RowVectorXd ty = ty.LinSpaced(cols, 1, cols) * cellSize;

        // A matrix where each row goes from 1 to the # of cols
        Eigen::MatrixXd Y = ty.replicate(rows, 1);
        // A matrix where each column goes from 1 to the # of rows
        Eigen::MatrixXd X = tx.replicate(1, cols);
        Eigen::MatrixXd XY;
        // A matrix containing all possible permutations x where {x|(a,b)},
        //  where 1 <= a <= # of cols , 1 <= b <= # of rows
        XY.resize(rows * cols, 2);
        X.resize(rows * cols, 1);
        Y.resize(rows * cols, 1);
        // create a table of x,y pairs
        XY.col(0) = X;
        XY.col(1) = Y;

        Eigen::Matrix2d hrCov;
        // 2d comma initializer shortcut fromEigen
        hrCov << vary, covxy, covxy, varx;
        double temp;

        for (double i = border; i < rows; i ++) {
            for (double j = border; j < cols; j ++) {
                temp = bivariateNorm(j / (cols - 1), i / (rows - 1), mux, muy,
                        ousdx, ousdy, oucor);
                behaviorGrid -> data(i, j) = temp;
            }
        }
    }
    // Vertical Habitat Restrictions
    if (acousticParams.count("minDepth") > 0 &&
        acousticParams.count("maxDepth") > 0 && bias != 2) {
        if (debug) {
            std::cout << "Using Vertical Habitat Restrictions: \n\tminDepth: " <<
                acousticParams["minDepth"] << "m\n\tmaxDepth: " <<
                acousticParams["maxDepth"] << "m\n";
        }
        Grid* minGrid = new Grid(rows, cols, "min");
        Grid* maxGrid = new Grid(rows, cols, "max");
        Grid* temp = new Grid(rows, cols, "temp");
        // Add the minDepth to the topographyGrid (a copy, the original values
        //   are unmodified)
        minGrid -> data = topographyGrid -> data.block(border,
                border, rows, cols).array() + stod(acousticParams["minDepth"]);
        // Any non-positive values are valid, store a 1 there
        minGrid -> data = minGrid -> data.unaryExpr(std::ptr_fun(isNonPos));
        // Add the maxDepth to the topographyGrid (again, a copy)
        maxGrid -> data = topographyGrid -> data.block(border, border, rows,
                cols).array() + stod(acousticParams["maxDepth"]);
        // Any non-negative values are valid, store a 1
        maxGrid -> data = maxGrid -> data.unaryExpr(std::ptr_fun(isNonNeg));
        // Multiply the two zero-one matrices to get cells with values between
        //  the min and max depths
        temp->data = minGrid -> data.cwiseProduct(maxGrid -> data);
        // Mask the behaviorGrid with our validity matrix
        behaviorGrid -> data.block(border, border, rows, cols) =
                 behaviorGrid -> data.block(border, border, rows,
                 cols).cwiseProduct(temp->data);
    }
    // convert the grid to a probability matrix
    double sum = behaviorGrid -> data.sum();
    if (sum <= 0) {
        std::string errorMsg = "Error: The fish model is empty.  This may "
                   "occur when no cells in your bathymetry fall between the "
                   "given \'minDepth\' and \'maxDepth\' parameters.\n";
        printError(errorMsg, 0, acousticParams["timestamp"]);
    }

    behaviorGrid -> data = behaviorGrid -> data / sum;
}
