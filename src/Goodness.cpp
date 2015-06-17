//============================================================================
// Name        : Goodness.cpp
// Author      : Greg Burgess
// Version     : 1.0
// Copyright   : Copyright 2014
// Description : Functions for various goodness computations.
//============================================================================
#include  <stdlib.h>
#include  <Dense>
#include  <algorithm>
#include  <cmath>
#include  <iostream>
#include  <limits>
#include  <set>
#include <string>
#include  <utility>
#include  <vector>
#include "GlobalVars.h"
#include "Goodness.h"
#include "Grid.h"
#include "Utility.h"

/**
 * Helper class used for sorting pairs.
 */
class sortByDist {
 public:
    int r, c;
    /**
     * Constructor.
     * @param r The row value of a point.
     * @param c The col value of a point.
     */
    sortByDist(int r, int c) : r(r), c(c) {}

    /**
     * Returns the distance between the given point and this point.
     * @param point The point to compare to the defined point.
     * @return The distance between the stored point and the given point.
     */
    double distDelta(std::pair<int, int> point) {
        return sqrt(pow(r-(point.first), 2)+pow(c-(point.second), 2));
    }

    /**
     * Overloading the () operator to sort points by their distance from
     * the stored point.
     * @param lhs The first point in the comparison.
     * @param rhs The second point in the comparison.
     * @return True if lhs is closer than rhs, false otherwise.
     */
    bool operator() (std::pair<int, int> lhs, std::pair<int, int> rhs) {
        return distDelta(lhs)  <  distDelta(rhs);
    }
};

/**
 * Returns 0 if a given double is nan or inf.
 * A helper function to Eigen's unaryExpr() function.
 * @param x The value to inspect.
 * @return 0 if x is a nan or inf value, x otherwise.
 */
double validateZero(double x) {
    if (std::isnan(x) || std::isinf(x)) {
        return 0;
    }
    return x;
}


/**
 * Determines the value at point x on a normal distribution with a given sd.
 * @param sd The sd value of the normal distribution.  Determines the
 *        width of the curve.
 * @param x The point on the curve to evaluate.
 * @return The value at a point x on a normal distribution with a given sd.
 */
double normalDist(double sd, double x) {
    double exponent = -1*std::pow(x, 2.0),
           base = 1/(sd*std::sqrt(2*M_PI));
    exponent = exponent/(2*std::pow(sd, 2.0));
    exponent = std::exp(exponent);
    return(base*exponent);
}

/**
 * Normalizes f(x) to a value between 0 and peak, where f() is a normal
 * distribution defined by sd.
 * @param peak A scaling factor representing the maximum value in the
 *        range.
 * @param sd The sd value of the normal distribution.  Determines the
 *        width of the curve.
 * @param x The point on the curve to evaluate.
 * @return The value of f(x) scaled into the range [0, peak], where f(x)
 *         is a normal distribution defined by sd.
 */
double normalProb(double peak, double sd, double x) {
    return peak*(normalDist(sd, std::abs(x))/normalDist(sd, 0));
}

/**
 * Computes the integral of a normal distribution with the given mean and sd.
 * @param mean The mean of the normal distribution.
 * @param sd The sd of the normal distribution.
 * @param x The point to integrate up to.
 * @return The integral of a normal distribution witht he given mean and sd,
 *         up to x.
 */
double cdist(double mean, double sd, double x) {
    return (1 + erf(  (x-mean)/(sd*sqrt(2))  ))/2.0;
}

/**
 * Computes a section of an integral from start to end on the normal curve
 * defined by mean and sd.
 * @param mean The mean of the normal distribution.
 * @param sd The sd of the normal distribution.
 * @param start The point to start integrating at.
 * @param end The point to stop integrating at.
 * @return The integral of the normal curve defined by mean and sd, from
 *           start to end.
 */
double cdistPartition(double mean, double sd, double start, double end) {
    double alpha = cdist(mean, sd, start);
    double beta = cdist(mean, sd, end);
    return beta - alpha;
}

/**
 * Restricts the maximum value of x to zero.
 * @param x The point to evaluate.
 * @return 0 if x is positive, x otheriwse.
 */
double restrictMaxVizDepth(double x) {
    if (x > 0) {
        return 0;
    }
    return x;
}

/**
 * Precalculates a distanceGradient representing the probability of detection
 * for a given cell based on its distance from the center, then calls the
 * appropriate goodness function based on the given bias.
 * @param topographyGrid A pointer to a Grid object containing topographic
 *        information for the area of study.
 * @param behaviorGrid A pointer to a Grid object containing the probability
 *        distribution for the animal(s) being studied.
 * @param goodnessGrid A pointer to an empty Grid object.  Resultant data
 *        will be dumped here.
 * @param bias An integer representing the bias for goodness to use.
 *        Bias Meaning:
 *          1: Choose areas of high animal density, ignoring bathymetry.
 *          2: Choose areas with good visibility, ignoring animal density.
 *          3: Choose areas with good visibility of animals (both visibility
 *             and animal density are considered).
 * @param sensorRange The maximum detection range of a sensor given in units
 *        of grid cells.
 * @param sensorPeakDetectionProbability The probability of detecting a fish
 *        right next to the sensor.
 * @param SDofSensorDetectionRange The standard deviation of sensor range,
 *        determined by range testing.  Serves as a sigma value for the detection
 *        probability curve (a normal distribution).
 */
void calculateGoodnessGrid(Grid* topographyGrid, Grid* behaviorGrid,
                           Grid* goodnessGrid, Grid* coverageGrid, int bias,
                           int sensorRange,
                           double sensorPeakDetectionProbability,
                           double SDofSensorDetectionRange) {
    int size = 2 * sensorRange + 1;
    Eigen::MatrixXd distanceGradient;
    Eigen::MatrixXd detectionGradient;
    distanceGradient.resize(size, size);
    distanceGradient.setConstant(0);
    detectionGradient.resize(size, size);
    detectionGradient.setConstant(0);
    // Create a gradient of distances to avoid redundant computation
    makeDistGradient(&distanceGradient, sensorRange);
    // Create a gardient of probability of detection (due to sensorRange) to
    //   avoid redundant computation
    makeDetectionGradient(&detectionGradient, & distanceGradient,
                    sensorPeakDetectionProbability, SDofSensorDetectionRange);

    if (bias ==  1) {
        goodness_Fish(topographyGrid, behaviorGrid, goodnessGrid, coverageGrid,
                &distanceGradient, &detectionGradient, sensorRange,
                sensorPeakDetectionProbability, SDofSensorDetectionRange);
    } else if (bias ==  2) {
        goodness_Viz(topographyGrid, behaviorGrid, goodnessGrid,  coverageGrid,
                &distanceGradient, &detectionGradient, sensorRange,
                sensorPeakDetectionProbability, SDofSensorDetectionRange);
    } else if (bias ==  3) {
        goodness_VizOfFish(topographyGrid, behaviorGrid, goodnessGrid,
                coverageGrid, &distanceGradient, &detectionGradient, sensorRange,
                sensorPeakDetectionProbability, SDofSensorDetectionRange);
    } else {
        printError("ERROR! Invalid bias value.", -2,
                    acousticParams["timestamp"]);
    }
    goodnessGrid->data =
            goodnessGrid->data.unaryExpr(std::ptr_fun(validateZero));
}

/**
 * Simply sums the cells within sensorRange of each cell on the BehaviorGrid.
 * Results are written to goodnessGrid.
 * @param topographyGrid A pointer to a Grid object containing topographic
 *        information for the area of study.
 * @param behaviorGrid A pointer to a Grid object containing the probability
 *        distribution for the animal(s) being studied.
 * @param goodnessGrid A pointer to an empty Grid object.  Resultant data
 *        will be dumped here.
 * @param distanceGradient A pointer to a matrix containing the distance of
 *        each cell from the center.
 * @param detectionGradient A pointer to a matrix containing the probability
 *        (based on distance from the center cell) of detecting a tag in the
 *        given cell.
 * @param sensorRange The maximum detection range of a sensor given in units
 *        of grid cells.
 * @param sensorPeakDetectionProbability The probability of detecting a fish
 *        right next to the sensor.
 * @param SDofSensorDetectionRange The standard deviation of sensor range,
 *        determined by range testing.  Serves as a sigma value for the
 *        detection probability curve (a normal distribution).
 */
void goodness_Fish(Grid* topographyGrid, Grid* behaviorGrid, Grid* goodnessGrid,
              Grid* coverageGrid, Eigen::MatrixXd* distanceGradient,
              Eigen::MatrixXd* detectionGradient, int sensorRange,
              double sensorPeakDetectionProbability,
              double SDofSensorDetectionRange) {
    int cols = topographyGrid->cols,
        rows = topographyGrid->rows,
        rstart = 0, cstart = 0,
        size = 2 * sensorRange + 1;
    double endRow = rows-border;

    for (int r = border; r < rows-border; r++) {
        if (debug) {
            std::cout  <<  (r + 1) / endRow  <<  "\n";
        }
        rstart = r - sensorRange;
        for (int c = border; c < cols - border; c++) {
            cstart = c - sensorRange;
            goodnessGrid->data(r, c) =  (behaviorGrid->data.block(rstart,
                                         cstart, size, size).
                                         cwiseProduct(*detectionGradient)).
                                         cwiseProduct(coverageGrid->data.block(
                                         rstart, cstart, size, size)).sum();
        }
    }
}

/**
 * Determines the goodness of each cell in the study area based on percentage
 * of the water column (in the surrounding cells) that can be seen. determines
 * the average percentage of the water column visible from the current cell in
 * each cell. Results are written to goodnessGrid.
 * @param topographyGrid A pointer to a Grid object containing topographic
 *        information for the area of study.
 * @param behaviorGrid A pointer to a Grid object containing the probability
 *        distribution for the animal(s) being studied.
 * @param goodnessGrid A pointer to an empty Grid object.  Resultant data
 *        will be dumped here.
 * @param distanceGradient A pointer to a matrix containing the distance of
 *        each cell from the center.
 * @param detectionGradient A pointer to a matrix containing the probability
 *        (based on distance from the center cell) of detecting a tag in the
 *        given cell.
 * @param sensorRange The maximum detection range of a sensor given in units
 *        of grid cells.
 * @param sensorPeakDetectionProbability The probability of detecting a fish
 *        right next to the sensor.
 * @param SDofSensorDetectionRange The standard deviation of sensor range,
 *        determined by range testing.  Serves as a sigma value for the detection
 *        probability curve (a normal distribution).
 */
void goodness_Viz(Grid* topographyGrid, Grid* behaviorGrid, Grid* goodnessGrid,
             Grid* coverageGrid, Eigen::MatrixXd* distanceGradient,
             Eigen::MatrixXd* detectionGradient, int sensorRange,
             double sensorPeakDetectionProbability,
             double SDofSensorDetectionRange) {
    int cols = topographyGrid->cols,
        rows = topographyGrid->rows,
        size = 2 * sensorRange + 1,
        lowerBorderRange = 2*border,
        upperRowRng = rows-2*border-1,
        upperColRng = cols-2*border-1;
    double endRow = rows-border;

    // declare and initialize matrices
    Eigen::MatrixXd visibilityGrid;
    Eigen::MatrixXd localTopography;
    Eigen::MatrixXd temp;
    visibilityGrid.resize(size, size);
    visibilityGrid.setConstant(0);

    for (int r = border; r < rows-border; r++) {
        if (debug) {
            std::cout  <<  (r + 1) / endRow  <<  "\n";
        }
        for (int c = border; c < cols-border; c++) {
            // compute the visibility grid for each cell
            calcVizGrid(topographyGrid, distanceGradient, &visibilityGrid,
                        &localTopography, &temp, r, c, sensorRange);
            // invalidate cells above the surface
            visibilityGrid = visibilityGrid.
                    unaryExpr(std::ptr_fun(restrictMaxVizDepth));
            // compute {probability of detection due to range}*
            //   {visible depth}/{actual depth}
            temp = ((visibilityGrid.cwiseQuotient(localTopography)).
                    cwiseProduct(*detectionGradient)).
                    cwiseProduct(coverageGrid->data.block(
                    r - sensorRange, c - sensorRange, size, size));
            // if we're near the border, there will be null values
            //  (because we divided visible depth by the depth of a border cell
            //  (which is 0)).
            if (c < lowerBorderRange || r < lowerBorderRange ||
                     r > upperRowRng || c > upperColRng) {
                temp = temp.unaryExpr(std::ptr_fun(validateZero));
            }
            // sum the resulting goodness
            goodnessGrid->data(r, c) = temp.sum();
        }
    }
    // normalize the goodness of all cells to the range [0, 1]
    goodnessGrid->data = goodnessGrid->data/(size*size);
}


/**
 * Determines the goodness of each cell in the study area based on the number
 * of fish that can be seen from each cell.
 * the average percentage of the water column visible from the current cell in
 * each cell. Results are written to goodnessGrid.
 * @param topographyGrid A pointer to a Grid object containing topographic
 *        information for the area of study.
 * @param behaviorGrid A pointer to a Grid object containing the probability
 *        distribution for the animal(s) being studied.
 * @param goodnessGrid A pointer to an empty Grid object.  Resultant data
 *        will be dumped here.
 * @param distanceGradient A pointer to a matrix containing the distance of
 *        each cell from the center.
 * @param detectionGradient A pointer to a matrix containing the probability
 *        (based on distance from the center cell) of detecting a tag in the
 *        given cell.
 * @param sensorRange The maximum detection range of a sensor given in units
 *        of grid cells.
 * @param sensorPeakDetectionProbability The probability of detecting a fish
 *        right next to the sensor.
 * @param SDofSensorDetectionRange The standard deviation of sensor range,
 *        determined by range testing.  Serves as a sigma value for the detection
 *        probability curve (a normal distribution).
 */
// TODO(GREG) Abstract this to calculate goodness for a start point/ dist,
//              then pass in  a pointer to catch the contents of the last calculation.
//              This'll let us calculate visibility from single points (suppressionExact)
//              and preserve the visibility grid.
void goodness_VizOfFish(Grid* topographyGrid, Grid* behaviorGrid,
                    Grid* goodnessGrid, Grid* coverageGrid,
                    Eigen::MatrixXd* distanceGradient,
                    Eigen::MatrixXd* detectionGradient, int sensorRange,
                    double sensorPeakDetectionProbability,
                    double SDofSensorDetectionRange) {
    int cols = topographyGrid->cols,
        rows = topographyGrid->rows,
        size = 2 * sensorRange + 1,
        lowerBorderRange = 2*border,
        upperRowRng = rows-2*border-1,
        upperColRng = cols-2*border-1,
        i = 0,
        j = 0;
    double totalFish = 0,
           visibleFish = 0,
           mean = 0,
           sd = 0,
           endRow = rows-border;
    bool useRelativeBehaviorModel  =
            (acousticParams.count("meanRelativePosition") +
             acousticParams.count("RelativePositionSD") ==  2);

    if (useRelativeBehaviorModel) {
        mean = stod(acousticParams["meanRelativePosition"]);
        sd = stod(acousticParams["RelativePositionSD"]);
    }
    if (debug) {
        std::cout  <<  "useRelativeBehaviorModel = "  <<
                        useRelativeBehaviorModel  <<  "\n";
    }
    // declare and initialize matrices
    Eigen::MatrixXd visibilityMatrix;
    Eigen::MatrixXd localTopography;
    Eigen::MatrixXd localBehavior;
    Eigen::MatrixXd fishVisibility;
    Eigen::MatrixXd temp;

    visibilityMatrix.resize(size, size);
    visibilityMatrix.setConstant(0);
    fishVisibility.resize(size, size);
    fishVisibility.setConstant(0);

    for (int r = border; r < rows-border; r++) {
        if (debug) {
            std::cout  <<  (r + 1) / endRow  <<  "\n";
        }
        // out  <<  "\n"  <<  r;
        for (int c = border; c < cols-border; c++) {
            // compute the visibility grid for each cell
            calcVizGrid(topographyGrid, distanceGradient, &visibilityMatrix,
                        &localTopography, &temp, r, c, sensorRange);
            // invalidate cells above the surface
            visibilityMatrix = visibilityMatrix.
                            unaryExpr(std::ptr_fun(restrictMaxVizDepth));
            localBehavior = behaviorGrid->data.
                            block(r-sensorRange, c-sensorRange, size, size);
            // Compute the visible percentage of fish in the water column of
            //   each cell

            // Linear distribution model
            fishVisibility = visibilityMatrix.cwiseQuotient(localTopography);
            fishVisibility.unaryExpr(std::ptr_fun(validateZero));


            // Normal distribution model
            if (useRelativeBehaviorModel) {
                for (i = 0; i < size; i++) {
                    for (j = 0; j < size; j++) {
                        // if you can see the whole water column, you can see
                        //   all available fish,
                        // regardless of the distribution model
                        if (visibilityMatrix(i, j) ==  1) {
                            fishVisibility(i, j) = 1;
                        } else {  // if you can't, then things get interesting.
                            // compute total available fish
                            totalFish = cdistPartition(mean, sd, 1, 0);

                            // std::cout  <<  "cdistPartition(" << mean <<
                            //   ", " << sd << ", "  <<
                            //   localBehavior(i, j) << ", " << 0 <<
                            //   ") = " << totalFish;
                            // compute total visible fish
                            visibleFish = cdistPartition(mean, sd,
                                          fishVisibility(i, j), 0);
                            // compute visible/total
                            fishVisibility(i, j) = visibleFish / totalFish;
                            // std::cout  <<  visibleFish  <<  "/"  <<
                            //   totalFish  <<  " = "   <<
                            //   visibleFish/totalFish  <<  "\n";
                        }
                    }
                }
                // revalidate in case we divided by zero again...
                fishVisibility.unaryExpr(std::ptr_fun(validateZero));
            }
            // compute {probability of detection due to range} * {visible fish}
            temp = ((*detectionGradient).cwiseProduct(fishVisibility).
                    cwiseProduct(localBehavior)).
                    cwiseProduct(coverageGrid->data.block(r - sensorRange,
                                 c - sensorRange, size, size));
            // if we're near the border, there will be null values
            //   (because we divided visible depth
            // by the depth of a border cell (which is 0)).
            if (c < lowerBorderRange  ||  r < lowerBorderRange  ||
                    r > upperRowRng || c > upperColRng) {
                temp = temp.unaryExpr(std::ptr_fun(validateZero));
            }

            // sum the resulting goodness
            goodnessGrid->data(r, c) = temp.sum();
        }
    }
    // normalize the goodness of all cells to the range [0, 1]
    goodnessGrid->data = goodnessGrid->data / (size*size);
}


/**
 * Offsets a Cartesian coordinate to the center of a cell.'
 * @param point A pointer to a Pair object containing the x and y Cartesian
 *        coordinates to offset.
 * @return A new, offset point.
 */
std::pair<int, int> offset(const std::pair<int, int> *point) {
    std::pair<int, int> newPoint = std::make_pair(point->first + .5,
                                                  point->second + .5);
    return newPoint;
}

/**
 * Gets a set of cell locations that intersect a beam between the origin cell
 * and a target cell. Pairs are configured as (x, y), (column, row).
 * @param origin A pointer to a Pair object that indicates the origin cell.
 * @param target A pointer to a Pair object that indicates the terminal cell.
 * @return A vector containing Pairs that intersect a line drawn between the
 *         two points.
 */
std::vector<std::pair<int, int>> getCells(const std::pair<int, int> *origin,
                               const std::pair<int, int> *target) {
    std::set<std::pair<int, int>> pairs;
    // Calculate the slope
    int ox = origin->first,
        oy = origin->second,
        tx = target->first,
        ty = target->second;
    if (ox!= tx || oy!= ty) {
        double slopeNumerator = (ty - oy),
               slopeDenominator = (tx - ox),
               m = 0;
        // Handle INF/NAN values.
        if (slopeDenominator ==  0) {
            if (slopeNumerator > 0) {
                m = 999999;
            } else {
                m = -999999;
            }
        } else {
            m = slopeNumerator / slopeDenominator;
        }
        double absm = abs(m);
        // assume the sensor is in the middle of the cell
        std::pair<int, int> offsetO = offset(origin);
        double b = offsetO.second - m * offsetO.first;
        int lowerX = std::min(ox, tx);
        int upperX = std::max(ox, tx);
        int lowerY = std::min(oy, ty);
        int upperY = std::max(oy, ty);
        int startX = lowerX, endX = upperX;
        int startY = lowerY, endY = upperY;
        int y = 0, y1 = 0;
        int x = 0, x1 = 0;
        // =====================Calculate Cells================================
        // Steep Slopes
        if (absm > 1) {
            for (y = startY + 1; y  <=  endY; y ++) {
                x = (y - b) / m;
                x1 = floor(x);
                // std::cout  << "Y: " << y  << ", X: " << x << "; added(" <<
                //                x1 << ", " << y-1 << ")
                //  and (" << x1 << ", " << y << ")\n";
                pairs.insert(std::make_pair(x1, y-1));
                pairs.insert(std::make_pair(x1, y));
            }
        } else if (absm  <  1) {  // Slow Slopes
            for (x = startX + 1; x  <=  endX; x ++) {
                y = m * x + b;
                y1 = floor(y);
                pairs.insert(std::make_pair(x - 1, y1));
                pairs.insert(std::make_pair(x, y1));
            }
        } else {  // Slope ==  1
            startX = lowerX;
            endX = upperX;
            for (x = startX; x  <=  endX; x++) {
                y = m * x + b;
                pairs.insert(std::make_pair(x, y));
            }
        }
        if (pairs.size() > 0) {
            pairs.erase(pairs.begin());
            pairs.erase(*origin);
        }
        pairs.insert(*target);
        pairs.erase(*origin);

        // copy to vector
        std::vector<std::pair< int, int>> vpairs(pairs.begin(), pairs.end());
        // Sort pairs by distance from origin
        sortByDist sorter(origin->first, origin->second);
        sort(vpairs.begin(), vpairs.end(), sorter);
        // std::cout  <<   "Returning vector of size "  <<  vpairs.size()  <<
        // "\n";

        return vpairs;
    } else {
        std::vector< std::pair<int, int>> vpairs;
        return vpairs;
    }
}

/**
 * Computes the distanceGradient for a given square matrix. Results are
 * dumped to distGradient.
 * The resulting matrix contains each cell's distance from the center cell.
 * @param distGradient A pointer to an empty matrix.
 * @param rng The maximum detection range of a sensor.  This determines the
 *        size of the grid.
 */
void makeDistGradient(Eigen::MatrixXd* distGradient, int rng) {
    int size = 2 * rng + 1;
    Eigen::MatrixXd X;
    Eigen::MatrixXd Y;
    // Vectors from -rng to rng, of length size
    Eigen::VectorXd refx = refx.LinSpaced(size, -rng, rng);
    Eigen::VectorXd refy = refy.LinSpaced(size, -rng, rng);
    std::vector< std::pair<int, int>> cells;

    Y = refy.replicate(1, size);
    X = refx.replicate(1, size);
    X.transposeInPlace();

    // set slopeGrid to a gird containing each cell's linear distance from the
    //   center cell.
    *distGradient = Y.cwiseAbs2() + X.cwiseAbs2();
    *distGradient = distGradient->cwiseSqrt();
    (*distGradient)(rng, rng) = 1;
}

/**
 * Computes the detectionGradient for a given distanceGradient. Results are
 * dumped to detectionGradient.
 * @param detectionGradient A pointer to an empty matrix.
 * @param distanceGradient A pointer to a matrix containing the
 *        distanceGradient generated by makeDistGradient().
 * @param sensorPeakDetectionProbability The probability of detecting a tag
 *         right next to a sensor.
 * @param SDofSensorDetectionRange The standard deviation of sensor range,
 *        determined by range testing.  Serves as a sigma value for the
 *        detection curve.
 */
void makeDetectionGradient(Eigen::MatrixXd* detectionGradient,
                           Eigen::MatrixXd* distGradient,
                           double sensorPeakDetectionProbability,
                           double SDofSensorDetectionRange) {
    int rows = distGradient->rows(),
        cols = distGradient->cols(),
        r = 0,
        c = 0;
    for (r = 0; r  <  rows; r ++) {
        for (c = 0; c  <  cols; c ++) {
            (*detectionGradient)(r, c) = normalProb(
                                                sensorPeakDetectionProbability,
                                                SDofSensorDetectionRange,
                                                (*distGradient)(r, c));
        }
    }

    (*detectionGradient)(rows / 2, c / 2) = sensorPeakDetectionProbability;
}

/**
 * Calculates the visibility Grid for a cell at r, c on the topographyGrid.
 * The result  is a grid containing the max visible depth at each cell from
 * the center cell.  Results are dumped into solutionGrid.
 *  @param topographyGrid A pointer to a Grid object containing topographic
 *      information for the area of study.
 * @param distanceGradient A pointer to a matrix containing the distance of
 *      each cell from the center.
 * @param solutionGrid A pointer to an empty matrix.
 * @param localTopo A pointer to an empty matrix.
 * @param tempGrid A pointer to an empty matrix.
 * @param row The row index for the origin cell in the topographyGrid.
 * @param col The col index for the origin cell in the topographyGrid.
 * @param sensorRange The maximum detection range of a sensor given in units
 *        of grid cells.
 */
void calcVizGrid(Grid* topographyGrid, Eigen::MatrixXd* distanceGradient,
                Eigen::MatrixXd* solutionGrid, Eigen::MatrixXd* localTopo,
                Eigen::MatrixXd* tempGrid, int row, int col, int sensorRange) {
    // std::cout  <<  "\n[CalcVizGrid()]\n";
    int i = 0, j = 0,
        // Compute row metadata
        startRow = row - sensorRange,
        // Compute col metadata
        startCol = col - sensorRange,
        sensorDiameter = 2 * sensorRange + 1;
    Eigen::MatrixXd slopeGrid;
    // assign dimensions, and set all values in vizGrid to the center
    //    cell's depth
    // copy out the block of topography we're interested in.
    (*localTopo) = topographyGrid->data.block(startRow, startCol,
                     sensorDiameter, sensorDiameter);
    tempGrid->resize(sensorDiameter, sensorDiameter);
    tempGrid->setConstant((*localTopo)(sensorRange, sensorRange));
    // vizGrid now contains the depth deltas from the origin cell
    *tempGrid = (*localTopo) - (*tempGrid);
    // slopeGrid now contains depth deltas divided by distance deltas, aka the
    //   slope from the center cell to each cell.
    slopeGrid = tempGrid->cwiseQuotient(*distanceGradient);
    // slopeGrid now has the slope from all cells to the center
    // viz grid has depth deltas
    std::set<std::pair<int, int>> unprocessedCells;
    std::vector<std::pair<int, int>> interveningCells;
    std::pair<int, int> origin = std::make_pair(sensorRange, sensorRange);
    double maxSlope = 0;
    // Add cells to process to a list.
    for (i = 0; i  <  sensorDiameter; i ++) {
        for (j = 0; j  <  sensorDiameter; j ++)
            unprocessedCells.insert(
                    (const std::pair<int, int>) std::make_pair(i, j));
    }
    unprocessedCells.erase(origin);
    maxSlope = 0;
    while (unprocessedCells.size() > 0) {
        interveningCells = getCells(&origin, &*unprocessedCells.cbegin());
        if (interveningCells.size() > 0) {
            auto iterator = interveningCells.cbegin();
            std::pair<int, int> it = *iterator;
            i = it.first;
            j = it.second;
            maxSlope = slopeGrid(i, j);
            // Process each cell for LoS
            for (auto iterator = interveningCells.begin();
                      iterator != interveningCells.cend(); ++iterator) {
                maxSlope = std::max(maxSlope, slopeGrid(iterator->first,
                                                   iterator->second));
                (*tempGrid)(iterator->first, iterator->second) = maxSlope;
                unprocessedCells.erase(*iterator);
            }
            unprocessedCells.erase(*interveningCells.crend());
        }
    }
    // at this point, tempGrid has all the max slopes (LoS)
    //                 slopeGrid is free to use
    //                 localTopo still has topography
    *solutionGrid = tempGrid->cwiseProduct(*distanceGradient).array() +
                    (*localTopo)(sensorRange, sensorRange);
    // solutionGrid now has the max visible depths from the origin cell.
}


/**
 * Selects the n cells in the goodnesGrid with the highest unique
 * detection probability, and returns a matrix containing the locations.
 * @param goodnessGrid The pre-computed goodnessGrid.
 * @param bestSensors A pointer to an empty Matrix, results will be dumped
 *        here.
 * @param userSensors A Matrix with four columns that holds the r,c pairs
 *        for user specified sensors, with one sensor per row, the unique
 *        recovery rate for that sensor, and the absoloute recovery rate for
 *        that sensor.
 * @param numTotalSensors The number of spots to search for.  The number of
 *        rows in the resultant matrix.
 * @param sensorRange he maximum detection range of a sensor given in units
 *        of grid cells.
 * @param sensorPeakDetectionProbability The probability of detecting a tag
 *        right next to a sensor.
 * @param SDofSensorDetectionRange The standard deviation of sensor range,
 *        determined by range testing.  Serves as a sigma value for the
 *        detection curve.
 * @param timestamp A timestamp string for identifying related files.
 * @param useUniqueRR A bool set to true if only unique recovery rates should
 *        be calculated.  If true, suppression is applied.  If false, the
 *        absoloute recovery rate is calculated, and no suppression is applied.
 */
void selectTopSensorLocations(Grid* topographyGrid, Grid* behaviorGrid, Grid* goodnessGrid, Grid* goodnessGridPerfect,
                    Grid* coverageGrid, Eigen::MatrixXd* bestSensors,
                    Eigen::MatrixXd* userSensors, int numSensorsToPlace,
                    int sensorRange, int bias, double suppressionRangeFactor,
                    double sensorPeakDetectionProbability,
                    double SDofSensorDetectionRange,
                    std::string timeStamp) {
    int row = 0, col = 0, i = 0;

    // Process all the user-specified sensor locations
    for (i = 0; i < userSensors->rows(); i++) {
        // Translate user data to our internal grid
        row = (*userSensors)(i, 0) + border;
        col = (*userSensors)(i, 1) + border;

        // Record the Unique Recovery Rate
        (*userSensors)(i, 2) = goodnessGrid->data(row, col);
        // Record the Absolute Recovery Rate
        (*userSensors)(i, 3) = goodnessGridPerfect->data(row, col);
        // Suppress the chosen point
        suppress(topographyGrid, behaviorGrid, goodnessGrid, coverageGrid, row, col, sensorRange,
                bias, suppressionRangeFactor, sensorPeakDetectionProbability,
                SDofSensorDetectionRange, timeStamp);

    }

    // Select and process the top location in the goodness grid
    for (i = 0; i < numSensorsToPlace; i++) {
        // Find the max coefficient in the matrix
        goodnessGrid->data.maxCoeff(&row, &col);

        // Translate user data to our internal grid
        (*bestSensors)(i, 0) = row - border;
        (*bestSensors)(i, 1) = col - border;

        // Record the Unique Recovery Rate
        (*bestSensors)(i, 2) = goodnessGrid->data(row, col);
       // Record the Absolute Recovery Rate
        (*bestSensors)(i, 3) = goodnessGridPerfect->data(row, col);
        // Suppress the chosen point
        suppress(topographyGrid, behaviorGrid, goodnessGrid, coverageGrid, row, col,
                sensorRange, bias, suppressionRangeFactor,
                sensorPeakDetectionProbability, SDofSensorDetectionRange,
                timeStamp);
    }
}



void suppressionQuick(Grid* gridToSuppress, Grid* coverageGrid,
                      Eigen::MatrixXd* suppressionGradient,
                      int row, int col, int sensorRange,
                      int suppressionDiameter) {
    int GRID_ROW_COUNT = gridToSuppress->data.rows(),
        GRID_COL_COUNT = gridToSuppress->data.cols(),
        startRow = std::max(row - sensorRange, 0),
        startCol = std::max(col - sensorRange, 0),
        rowDist = std::min(suppressionDiameter, GRID_ROW_COUNT - startRow),
        colDist = std::min(suppressionDiameter, GRID_COL_COUNT - startCol);
    Eigen::MatrixXd temp;
    temp.resize(suppressionDiameter, suppressionDiameter);
    if (debug) {
        std::cout << "Blocking " << startRow << "," << startCol << " for " <<
                     rowDist << ", " << colDist << " cells.\n" <<
                     "suppress sensor @ (" << row << "," << col <<
                     ")\nsuppressionDiameter: " << suppressionDiameter <<
                     "\nstartRow,startCol: " << startRow << "," << startCol <<
                     "\nrowDist, colDist: " << rowDist << "," << colDist <<
                     "\n";
    }
    // Suppress the chosen point
    temp.block(0, 0, rowDist, colDist) =
            gridToSuppress->data.block(startRow, startCol, rowDist, colDist);
    gridToSuppress->data.block(startRow, startCol, rowDist, colDist) =
            (temp.cwiseProduct(*suppressionGradient)).block(0, 0, rowDist,
            colDist);
}

// TODO(GREG) implement bias options
void suppressionExact(Grid* behaviorGrid, Grid* topographyGrid,
                Grid* goodnessGrid, Grid* coverageGrid, int bias, int row,
                int col, int sensorRange, int suppressionDiameter,
                std::string timeStamp) {
    // Do the right thing for the given bias.
    if (bias == 1) {
        // Ca
    } else if (bias == 3) {
        // Calculate specific visibility grid for cell (row,col)
        // Subtract this from the behaviorGrid
        // Call goodness_VizOfFish() function to update goodness for cells within sensor range of (row,col)

        // Calculate affected indicies
        int GRID_ROW_COUNT = gridToSuppress->data.rows(),
                GRID_COL_COUNT = gridToSuppress->data.cols(),
                startRow = std::max(row - sensorRange, 0),
                startCol = std::max(col - sensorRange, 0),
                rowDist = std::min(suppressionDiameter, GRID_ROW_COUNT - startRow),
                colDist = std::min(suppressionDiameter, GRID_COL_COUNT - startCol);
            Eigen::MatrixXd temp;
            temp.resize(suppressionDiameter, suppressionDiameter);
            if (debug) {
                std::cout << "Blocking " << startRow << "," << startCol << " for " <<
                             rowDist << ", " << colDist << " cells.\n" <<
                             "suppress sensor @ (" << row << "," << col <<
                             ")\nsuppressionDiameter: " << suppressionDiameter <<
                             "\nstartRow,startCol: " << startRow << "," << startCol <<
                             "\nrowDist, colDist: " << rowDist << "," << colDist <<
                             "\n";
            }
            // Suppress the chosen point
            temp.block(0, 0, rowDist, colDist) =
                    gridToSuppress->data.block(startRow, startCol, rowDist, colDist);
            behaviorGrid->data.block(startRow, startCol, rowDist, colDist) =
                    (temp.cwiseProduct(*suppressionGradient)).block(0, 0, rowDist,
                    colDist);
        // Recalculate the goodness of cells within
        // suppressionDiameter + 2 * sensorRange
    } else if (bias == 2) {
        // Recalculate the goodness of cells within suppressDiameter +
        // 2 * sensorRange
    } else {
        // We should never normally hit this error
        std::string errorMsg = "Invalid bais given: " + std::to_string(bias) +
                               ". Valid values are between 1 and 3.";
        printError(errorMsg, 1, timeStamp);
    }
}


void suppress(Grid* topographyGrid, Grid* behaviorGrid, Grid* goodnessGrid, Grid* coverageGrid, int row, int col,
              int sensorRange, int bias, double suppressionRangeFactor,
              double sensorPeakDetectionProbability,
              double SDofSensorDetectionRange, std::string timeStamp) {
        // TODO(GREG) suppression range factor does nothing, bc at large dist,
        // the exponential dropoff of the detection function results in very
        // small penalties.  Need to somehow scale up that curve to increase
        // the penalty at distance.

        int suppressionDiameter = ceil((2 * sensorRange * suppressionRangeFactor + 1) );
        if (acousticParams["suppressionMethod"] == "suppression.quick") {
            // Compute suppression gradient
            Eigen::MatrixXd suppressionGradient;
            Eigen::MatrixXd distanceGradient;
            suppressionGradient.resize(suppressionDiameter, suppressionDiameter);
            distanceGradient.resize(suppressionDiameter, suppressionDiameter);

            makeDistGradient(&distanceGradient, sensorRange);
            // Since we're discounting the fish we can see, our detection
            // gradient is the same as our suppression gradient.
            makeDetectionGradient(&suppressionGradient, &distanceGradient,
                                  sensorPeakDetectionProbability,
                                  SDofSensorDetectionRange);
            suppressionGradient.array() *= -1;
            suppressionGradient.array() += sensorPeakDetectionProbability;
            suppressionQuick(goodnessGrid, coverageGrid, &suppressionGradient,
                            row, col, sensorRange, suppressionDiameter);
        } else if (acousticParams["suppressionMethod"] == "suppression.exact") {
            // TODO(GREG) fill in
            suppressionExact(behaviorGrid, topographyGrid, goodnessGrid,
                    coverageGrid, bias, row, col, sensorRange,
                    suppressionDiameter, timeStamp);

        } else {
            std::string errorMsg = "Invalid suppression method specified: "
                    + acousticParams["suppressionMethod"];
            printError(errorMsg, 1, timeStamp);
        }
}

void getStats(Grid* unsuppressedGoodnessGrid, Grid* suppressedGoodnessGrid,
              Eigen::MatrixXd* bestSensors, int sensorRange, double* sparsity,
              double* absRecoveryRate, double* uniqueRecoveryRate,
              Grid* coverageGrid) {
    int i = 0, j = 0,
        numSensors = bestSensors->rows(),
        myR = 0, myC = 0,
        newR = 0, newC = 0;
    // =======================================================================
    // ===================== Compute absolute Recovery Rate ==================
    // =======================================================================
    *absRecoveryRate = (*bestSensors).col(3).sum();
    std::cout << "AbsRecoveryRate=" << *absRecoveryRate;

    // =======================================================================
    // ===================== Compute unique Recovery Rate ====================
    // =======================================================================
    *uniqueRecoveryRate = (*bestSensors).col(2).sum();
    std::cout << "uniqueRecoveryRate=" << *uniqueRecoveryRate;

    // =======================================================================
    // ===================== Compute Coverage Grid ===========================
    // =======================================================================
    coverageGrid->data = unsuppressedGoodnessGrid->data -
                         suppressedGoodnessGrid->data;

    // ========================================================================
    // ================ Compute Network Sparsity (delta) ======================
    // ========================================================================
    /* "Spatially, we calculate an absolute measure of station closeness, a, as
     * the median of {a[1], . . . , a[n]}, where a[i] is the distance from
     * station i to its nearest neighboring station.
     * Network sparsity is then defined as a/(2*d_r)"
     * - (Pedersen & Weng, 2013)
     *
     *  So, delta = median(distVec)/(2*params$detectionRange).
     */

    int midpoint = numSensors / 2;
    double a = 0;
    std::vector<double> minSensorDists;
    Eigen::MatrixXd sensorDistMat;
    Eigen::MatrixXd temp;

    sensorDistMat.resize(numSensors, numSensors);
    temp.resize(numSensors, numSensors);

    // Compute the distance between sensor i and sensor j (given by the value
    // at sensorDistMat(i,j). Given that sensorDistMat(i,j)==sensorDistMat(i,j),
    // we need only perform compuations for the Matrix's upper triangle.
    for (i = 0; i < numSensors; i ++) {
        myR = (*bestSensors) (i, 0);
        myC = (*bestSensors) (i, 1);
        for (j = i + 1; j < numSensors; j ++) {
            newR = (*bestSensors) (j, 0);
            newC = (*bestSensors) (i, 1);
            sensorDistMat(i, j) = sqrt(pow((myR - newR), 2) +
                                  pow((myC - newC), 2));
        }
    }

    // Create a transposition of the upper triangle (the lower triangle).
    temp = sensorDistMat.transpose();
    // Add the upper and lower triangles.
    sensorDistMat += temp;
    // Take the min of the rows to get the distance to the nearest neighbor for
    // each sensor.
    for (i = 0; i < numSensors; i ++) {
        minSensorDists.push_back(sensorDistMat.row(i).minCoeff());
    }

    // Sort the min distances
    sort(minSensorDists.begin(), minSensorDists.end());

    // Take the median
    if (numSensors % 2) {
        a = minSensorDists[midpoint];
    } else {
        a = minSensorDists[midpoint - 1] + minSensorDists[midpoint];
    }

    // Finally!
    *sparsity = a / static_cast<double>(2 * sensorRange);
    std::cout << "sparsity=" << *sparsity;
}

