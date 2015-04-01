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
#include  <utility>
#include  <vector>
#include "GlobalVars.h"
#include "Goodness.h"
#include "Grid.h"
#include "Utility.h"

namespace std {
/**
 * Helper class used for sorting pairs.
 */
class sortByDist {
 public:
    int r, c;
    /**
     * Constructor.
     * @param int r The row value of a point.
     * @param int c The col value of a point.
     */
    sortByDist(int r, int c) : r(r), c(c) {}

    /**
     * Returns the distance between the given point and this point.
     * @param pair<int, int> point The point to compare to the defiend point.
     * @return The distance between the stored point and the given point.
     */
    double distDelta(pair<int, int> point) {
        return sqrt(pow(r-(point.first), 2)+pow(c-(point.second), 2));
    }

    /**
     * Overloading the () operator to sort points by their distance from
     * the stored point.
     * @param pair<int, int> lhs The first point in the comparison.
     * @param pair<int, int> rhs The second point in the comparison.
     * @return True if lhs is closer than rhs, false otherwise.
     */
    bool operator() (pair<int, int> lhs, pair<int, int> rhs) {
        return distDelta(lhs)  <  distDelta(rhs);
    }
};

/**
 * Returns 0 if a given double is nan or inf.
 * A helper function to Eigen's unaryExpr() function.
 * @param double x The value to inspect.
 * @return 0 if x is a nan or inf value, x otherwise.
 */
double validate(double x) {
    if (std::isnan(x) || std::isinf(x)) {
        return 0;
    }
    return x;
}

/**
 * Determines the value at point x on a normal distribution with a given sd.
 * @param double sd The sd value of the normal distribution.  Determines the
 *                    width of the curve.
 * @param double x The point on the curve to evaluate.
 * @return The value at a point x on a normal distribution with a given sd.
 */
double normalDist(double sd, double x) {
    double exponent = -1*pow(x, 2.0),
           base = 1/(sd*sqrt(2*M_PI));
    exponent = exponent/(2*pow(sd, 2.0));
    exponent = exp(exponent);
    return(base*exponent);
}

/**
 * Normalizes f(x) to a value between 0 and peak, where f() is a normal
 * distribution defined by sd.
 * @param double peak A scaling factor representing the maximum value in a
 *                    range.
 * @param double sd The sd value of the normal distribution.  Determines the
 *                     width of the curve.
 * @param double x The point on the curve to evaluate.
 * @return The value of f(x) scaled into the range [0, peak], where f(x)
 *         is a normal distribution defined by sd.
 */
double normalProb(double peak, double sd, double x) {
    return peak*(normalDist(sd, x)/normalDist(sd, 0));
}

/**
 * Computes the integral of a normal distribution with the given mean and sd.
 * @param double mean The mean of the normal distribution.
 * @param double sd The sd of the normal distribution.
 * @param double x The point to integrate up to.
 * @return The integral of a normal distribution witht he given mean and sd,
 *         up to x.
 */
double cdist(double mean, double sd, double x) {
    return (1 + erf(  (x-mean)/(sd*sqrt(2))  ))/2.0;
}

/**
 * Computes a section of an integral from start to end on the normal curve
 * defined by mean and sd.
 * @param double mean The mean of the normal distribution.
 * @param double sd The sd of the normal distribution.
 * @param double start The point to start integrating at.
 * @param double end The point to stop integrating at.
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
 * @param double x The point to evaluate.
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
 *      information for the area of study.
 * @param behaviorGrid A pointer to a Grid object containing the probability
 *      distribution for the animal(s) being studied.
 * @param goodnessGrid A pointer to an empty Grid object.  Resultant data
 *      will be dumped here.
 * @param bias An integer representing the bias for goodness to use.
 *       Bias Meaning:
 *          1: Choose areas of high animal density, ignoring bathymetry.
 *          2: Choose areas with good visibility, ignoring animal density.
 *          3: Choose areas with good visibility of animals (both visibility
 *             and animal density are considered).
 * @param sensorRange The maximum detection range of a sensor given in units
 *      of grid cells.
 * @param sensorPeakDetectionProbability The probability of detecting a fish
 *      right next to the sensor.
 * @param SDofSensorDetectionRange The standard deviation of sensor range,
 *      determined by range testing.  Serves as a sigma value for the detection
 * probability curve (a normal distribution).
 */
void calculateGoodnessGrid(Grid* topographyGrid, Grid* behaviorGrid,
                           Grid* goodnessGrid, int bias, int sensorRange,
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
        goodFish(topographyGrid, behaviorGrid, goodnessGrid, &distanceGradient,
                 &detectionGradient, sensorRange,
                 sensorPeakDetectionProbability, SDofSensorDetectionRange);
    } else if (bias ==  2) {
        goodViz(topographyGrid, behaviorGrid, goodnessGrid, &distanceGradient,
                &detectionGradient, sensorRange,
                sensorPeakDetectionProbability, SDofSensorDetectionRange);
    } else if (bias ==  3) {
        goodVizOfFish(topographyGrid, behaviorGrid, goodnessGrid,
                      &distanceGradient, &detectionGradient, sensorRange,
                      sensorPeakDetectionProbability, SDofSensorDetectionRange);
    } else {
        printError("Invalid bias value.", -2, acousticParams["timestamp"]);
    }
}

/**
 * Simply sums the cells within sensorRange of each cell on the BehaviorGrid.
 * Results are written to goodnessGrid.
 * @param topographyGrid A pointer to a Grid object containing topographic
 *      information for the area of study.
 * @param behaviorGrid A pointer to a Grid object containing the probability
 *      distribution for the animal(s) being studied.
 * @param goodnessGrid A pointer to an empty Grid object.  Resultant data
 *      will be dumped here.
 * @param distanceGradient A pointer to a matrix containing the distance of
 *      each cell from the center.
 * @param detectionGradient A pointer to a matrix containing the probability
 *      (based on distance from the center cell) of detecting a tag in the
 *      given cell.
 * @param sensorRange The maximum detection range of a sensor given in units
 *      of grid cells.
 * @param sensorPeakDetectionProbability The probability of detecting a fish
 *      right next to the sensor.
 * @param SDofSensorDetectionRange The standard deviation of sensor range,
 *      determined by range testing.  Serves as a sigma value for the detection
 * probability curve (a normal distribution).
 */
void goodFish(Grid* topographyGrid, Grid* behaviorGrid, Grid* goodnessGrid,
              Eigen::MatrixXd* distanceGradient,
              Eigen::MatrixXd* detectionGradient, double sensorRange,
              double sensorPeakDetectionProbability,
              double SDofSensorDetectionRange) {
    int range = static_cast<int>(sensorRange),
        cols = topographyGrid->cols,
        rows = topographyGrid->rows,
        rstart = 0, cstart = 0,
        size = 2 * range + 1;
    double endRow = rows-border;

    for (int r = border; r < rows-border; r++) {
        cout  <<  (r + 1) / endRow  <<  "\n";
        rstart = r-range;
        for (int c = border; c < cols-border; c++) {
            cstart = c-range;
            goodnessGrid->data(r, c) =  (behaviorGrid->data.block(rstart,
                                         cstart, size, size).
                                         cwiseProduct(*detectionGradient)).
                                         sum();
        }
    }
}

/**
 * Determines the goodness of each cell in the study area based on percentage
 * of the water column (in the surrounding cells) that can be seen. determines
 * the average percentage of the water column visible from the current cell in
 * each cell. Results are written to goodnessGrid.
 * @param topographyGrid A pointer to a Grid object containing topographic
 *      information for the area of study.
 * @param behaviorGrid A pointer to a Grid object containing the probability
 *      distribution for the animal(s) being studied.
 * @param goodnessGrid A pointer to an empty Grid object.  Resultant data
 *      will be dumped here.
 * @param distanceGradient A pointer to a matrix containing the distance of
 *      each cell from the center.
 * @param detectionGradient A pointer to a matrix containing the probability
 *      (based on distance from the center cell) of detecting a tag in the
 *      given cell.
 * @param sensorRange The maximum detection range of a sensor given in units
 *      of grid cells.
 * @param sensorPeakDetectionProbability The probability of detecting a fish
 *      right next to the sensor.
 * @param SDofSensorDetectionRange The standard deviation of sensor range,
 *      determined by range testing.  Serves as a sigma value for the detection
 * probability curve (a normal distribution).
 */
void goodViz(Grid* topographyGrid, Grid* behaviorGrid, Grid* goodnessGrid,
             Eigen::MatrixXd* distanceGradient,
             Eigen::MatrixXd* detectionGradient, double sensorRange,
             double sensorPeakDetectionProbability,
             double SDofSensorDetectionRange) {
    int range = static_cast<int>(sensorRange),
        cols = topographyGrid->cols,
        rows = topographyGrid->rows,
        size = 2 * range + 1,
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
        // out << "\n" << r;
        cout  <<  (r + 1) / endRow  <<  "\n";
        for (int c = border; c < cols-border; c++) {
            // compute the visibility grid for each cell
            calcVizGrid(topographyGrid, distanceGradient, &visibilityGrid,
                        &localTopography, &temp, r, c, range);
            // invalidate cells above the surface
            visibilityGrid = visibilityGrid.
                    unaryExpr(ptr_fun(restrictMaxVizDepth));
            // compute {probability of detection due to range}*
            //   {visible depth}/{actual depth}
            temp = (visibilityGrid.cwiseQuotient(localTopography)).
                    cwiseProduct(*detectionGradient);
            // if we're near the border, there will be null values
            //  (because we divided visible depth by the depth of a border cell
            //  (which is 0)).
            if (c < lowerBorderRange || r < lowerBorderRange ||
                     r > upperRowRng || c > upperColRng) {
                temp = temp.unaryExpr(ptr_fun(validate));
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
 *      information for the area of study.
 * @param behaviorGrid A pointer to a Grid object containing the probability
 *      distribution for the animal(s) being studied.
 * @param goodnessGrid A pointer to an empty Grid object.  Resultant data
 *      will be dumped here.
 * @param distanceGradient A pointer to a matrix containing the distance of
 *      each cell from the center.
 * @param detectionGradient A pointer to a matrix containing the probability
 *      (based on distance from the center cell) of detecting a tag in the
 *      given cell.
 * @param sensorRange The maximum detection range of a sensor given in units
 *      of grid cells.
 * @param sensorPeakDetectionProbability The probability of detecting a fish
 *      right next to the sensor.
 * @param SDofSensorDetectionRange The standard deviation of sensor range,
 *      determined by range testing.  Serves as a sigma value for the detection
 *      probability curve (a normal distribution).
 */
void goodVizOfFish(Grid* topographyGrid, Grid* behaviorGrid,
                    Grid* goodnessGrid, Eigen::MatrixXd* distanceGradient,
                   Eigen::MatrixXd* detectionGradient, double sensorRange,
                   double sensorPeakDetectionProbability,
                   double SDofSensorDetectionRange) {
    int range = static_cast<int>(sensorRange),
        cols = topographyGrid->cols,
        rows = topographyGrid->rows,
        size = 2 * range + 1,
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
    cout  <<  "mean:"  <<  mean  <<  "\nsd:"  <<  sd  <<  "\n";

    cout  <<  "useRelativeBehaviorModel = "  <<  useRelativeBehaviorModel  <<
              "\n";
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
        cout  <<  (r + 1) / endRow  <<  "\n";
        // out  <<  "\n"  <<  r;
        for (int c = border; c < cols-border; c++) {
            // compute the visibility grid for each cell
            calcVizGrid(topographyGrid, distanceGradient, &visibilityMatrix,
                        &localTopography, &temp, r, c, range);
            // invalidate cells above the surface
            visibilityMatrix = visibilityMatrix.
                            unaryExpr(ptr_fun(restrictMaxVizDepth));
            localBehavior = behaviorGrid->data.
                            block(r-sensorRange, c-sensorRange, size, size);
            // Compute the visible percentage of fish in the water column of
            //   each cell

            // Linear distribution model
            fishVisibility = visibilityMatrix.cwiseQuotient(localTopography);
            fishVisibility.unaryExpr(ptr_fun(validate));


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

                            // cout  <<  "cdistPartition(" << mean <<
                            //   ", " << sd << ", "  <<
                            //   localBehavior(i, j) << ", " << 0 <<
                            //   ") = " << totalFish;
                            // compute total visible fish
                            visibleFish = cdistPartition(mean, sd,
                                          fishVisibility(i, j), 0);
                            // compute visible/total
                            fishVisibility(i, j) = visibleFish / totalFish;
                            // cout  <<  visibleFish  <<  "/"  <<
                            //   totalFish  <<  " = "   <<
                            //   visibleFish/totalFish  <<  "\n";
                        }
                    }
                }
                // revalidate in case we divided by zero again...
                fishVisibility.unaryExpr(ptr_fun(validate));
            }
            // compute {probability of detection due to range} * {visible fish}
            temp = (*detectionGradient).cwiseProduct(fishVisibility).
                    cwiseProduct(localBehavior);
            // if we're near the border, there will be null values
            //   (because we divided visible depth
            // by the depth of a border cell (which is 0)).
            if (c < lowerBorderRange  ||  r < lowerBorderRange  ||
                    r > upperRowRng || c > upperColRng) {
                temp = temp.unaryExpr(ptr_fun(validate));
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
 * coordinates to offset.
 * @return A new, offset point.
 */
pair<int, int> offset(const pair<int, int> *point) {
    pair<int, int> newPoint = make_pair(point->first + .5, point->second + .5);
    return newPoint;
}

/**
 * Gets a set of cell locations that intersect a beam between the origin cell
 * and a target cell. Pairs are configured as (x, y), (column, row).
 * @param origin A pointer to a Pair object that indicates the origin cell.
 * @param target A pointer to a Pair object that indicates the terminal cell.
 * @return A vector containing Pairs that intersect a line drawn between the
 *      two points.
 */
vector < pair<int, int>> getCells(const pair  < int, int> *origin,
                               const pair  < int, int> *target) {
    set < pair<int, int> > pairs;
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
        pair<int, int> offsetO = offset(origin);
        double b = offsetO.second - m * offsetO.first;
        int lowerX = min(ox, tx);
        int upperX = max(ox, tx);
        int lowerY = min(oy, ty);
        int upperY = max(oy, ty);
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
                // cout  << "Y: " << y  << ", X: " << x << "; added(" << x1 <<
                //   ", " << y-1 << ")
                //  and (" << x1 << ", " << y << ")\n";
                pairs.insert(make_pair(x1, y-1));
                pairs.insert(make_pair(x1, y));
            }
        } else if (absm  <  1) {  // Slow Slopes
            for (x = startX + 1; x  <=  endX; x ++) {
                y = m * x + b;
                y1 = floor(y);
                pairs.insert(make_pair(x - 1, y1));
                pairs.insert(make_pair(x, y1));
            }
        } else {  // Slope ==  1
            startX = lowerX;
            endX = upperX;
            for (x = startX; x  <=  endX; x++) {
                y = m * x + b;
                pairs.insert(make_pair(x, y));
            }
        }
        if (pairs.size() > 0) {
            pairs.erase(pairs.begin());
            pairs.erase(*origin);
        }
        pairs.insert(*target);
        pairs.erase(*origin);

        // copy to vector
        vector <pair< int, int>> vpairs(pairs.begin(), pairs.end());
        // Sort pairs by distance from origin
        sortByDist sorter(origin->first, origin->second);
        sort(vpairs.begin(), vpairs.end(), sorter);
        // cout  <<   "Returning vector of size "  <<  vpairs.size()  <<  "\n";

        return vpairs;
    } else {
        vector < pair<int, int>> vpairs;
        return vpairs;
    }
}

/**
 * Computes the distanceGradient for a given square matrix. Results are
 * dumped to distGradient.
 * The resulting matrix contains each cell's distance from the center cell.
 * @param distGradient A pointer to an empty matrix.
 * @param rng The maximum detection range of a sensor.  This determines the
 *      size of the grid.
 */
void makeDistGradient(Eigen::MatrixXd* distGradient, int rng) {
    int size = 2 * rng + 1;
    Eigen::MatrixXd X;
    Eigen::MatrixXd Y;
    // Vectors from -rng to rng, of length size
    Eigen::VectorXd refx = refx.LinSpaced(size, -rng, rng);
    Eigen::VectorXd refy = refy.LinSpaced(size, -rng, rng);
    vector < pair<int, int>> cells;

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
 *      distanceGradient generated by makeDistGradient().
 * @param sensorPeakDetectionProbability The probability of detecting a tag
 *      right next to a sensor.
 * @param SDofSensorDetectionRange The standard deviation of sensor range,
 *      determined by range testing.  Serves as a sigma value for the detection
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
                                                (*distGradient)(r, c)
                                                );
        }
    }
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
 *      of grid cells.
 */
void calcVizGrid(Grid* topographyGrid, Eigen::MatrixXd* distanceGradient,
        Eigen::MatrixXd* solutionGrid, Eigen::MatrixXd* localTopo,
        Eigen::MatrixXd* tempGrid, int row, int col, int sensorRange) {
    // cout  <<  "\n[CalcVizGrid()]\n";
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
    set  < pair<int, int>> unprocessedCells;
    vector < pair<int, int>> interveningCells;
    pair<int, int> origin = make_pair(sensorRange, sensorRange);
    double maxSlope = 0;
    // Add cells to process to a list.
    for (i = 0; i  <  sensorDiameter; i ++) {
        for (j = 0; j  <  sensorDiameter; j ++)
            unprocessedCells.insert((const pair<int, int>) make_pair(i, j));
    }
    unprocessedCells.erase(origin);
    maxSlope = 0;
    while (unprocessedCells.size() > 0) {
        interveningCells = getCells(&origin, &*unprocessedCells.cbegin());
        if (interveningCells.size() > 0) {
            auto iterator = interveningCells.cbegin();
            pair<int, int> it = *iterator;
            i = it.first;
            j = it.second;
            maxSlope = slopeGrid(i, j);
            // Process each cell for LoS
            for (auto iterator = interveningCells.begin();
                      iterator != interveningCells.cend(); ++iterator) {
                maxSlope = max(maxSlope, slopeGrid(iterator->first,
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
}  // namespace std
