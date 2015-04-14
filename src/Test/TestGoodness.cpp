#include <stdlib.h>
#include <Dense>
#include <iostream>
#include <set>
#include <string>
#include "../Goodness.h"
#include "../Bathy.h"
#include "../FishModel.h"
#include "TestGoodness.h"

//Don't set this too low, as we'll get errors when using compiler
    //optimization... (google "floating point comparison c++")
double tolerance = 0.0001;
/**
 * Executes all tests in TestGoodness.cpp.  Returns true if all tests pass, false otherwise.
 */
bool runGoodnessTests() {
    debug = false;
	bool success = (
			duplicatePoint() &&
			negativeHorizontal() &&
			negativeSlow() &&
			negativeSteep() &&
			negativeVertical() &&
			positiveHorizontal() &&
			positiveSlow() &&
			positiveSteep() &&
			positiveVertical() &&
			checkCalcPercentViz() &&
			checkDistGradient()&&
			checkDetectionGradient() &&
			checkNormalProb() &&
			checkCDistPartition() &&
			checkCalculateGoodness() &&
			testSelectTopSpots()
			);
	if (success) {
		std::cout<<"All Goodness Tests Passed!";
	}
	else {
		std::cout<<"Failures occurred!";
	}
	return success;
}


bool checkCalculateGoodness() {
    int numRows = 5,
        numCols = 5,
        range = 2,
        cellSize = 5,
        fishmodel = 0,
        bias = 0,
        i = 0;
    double peak = 1,
           sd = 1,
           ousdx = 0.9,
           ousdy = 0.9,
           oucor = 0,
           mux = 1,
           muy = 1;
    bool result = true;
    std::string methodID;

    border = range;
    Grid bGrid(numRows + 2 * border, numCols + 2 * border, "Behavior");
    Grid gGrid(numRows + 2 * border, numCols + 2 * border, "Goodness");
    Grid tGrid(numRows + 2 * border, numCols + 2 * border, "Topography");

    simulatetopographyGrid(&tGrid, numRows, numCols);

    //allocate solution array
    Eigen::MatrixXd sols[6];
    for (i=0;i<6;i++) {
        sols[i].resize(numRows+2*border,numCols+2*border);
    }
    sols[0] << 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0.121364, 0.163624, 0.173053, 0.163624, 0.121364, 0, 0,
    0, 0, 0.163624, 0.220599, 0.233312, 0.220599, 0.163624, 0, 0,
    0, 0, 0.173053, 0.233312, 0.246757, 0.233312, 0.173053, 0, 0,
    0, 0, 0.163624, 0.220599, 0.233312, 0.220599, 0.163624, 0, 0,
    0, 0, 0.121364, 0.163624, 0.173053, 0.163624, 0.121364, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0;

    sols[1] << 0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0.359983,  0.427732,  0.321347,  0.140188, 0.0330089,  0,  0,
    0,  0,  0.427732,  0.508426,  0.382617,  0.168131, 0.0407405,  0,  0,
    0,  0,  0.321347,  0.382617,  0.289702,  0.130446, 0.0345502,  0,  0,
    0,  0,  0.140188,  0.168131,  0.130446, 0.0642306, 0.0220156,  0,  0,
    0,  0,  0.0330089, 0.0407405, 0.0345502, 0.0220156, 0.0117116,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0;

    sols[2] << 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0.121364, 0.163624, 0.173053, 0.163624, 0.121364, 0, 0,
    0, 0, 0.163624, 0.220599, 0.233312, 0.220599, 0.163624, 0, 0,
    0, 0, 0.173053, 0.233312, 0.246757, 0.233312, 0.173053, 0, 0,
    0, 0, 0.163624, 0.220599, 0.233312, 0.220599, 0.163624, 0, 0,
    0, 0, 0.121364, 0.163624, 0.173053, 0.163624, 0.121364, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0;

    sols[3] << 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0.121364, 0.163624, 0.173053, 0.163624, 0.121364, 0, 0,
    0, 0, 0.163624, 0.220599, 0.233312, 0.220599, 0.163624, 0, 0,
    0, 0, 0.173053, 0.233312, 0.246757, 0.233312, 0.173053, 0, 0,
    0, 0, 0.163624, 0.220599, 0.233312, 0.220599, 0.163624, 0, 0,
    0, 0, 0.121364, 0.163624, 0.173053, 0.163624, 0.121364, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0;

    sols[4] << 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0.00485456, 0.00654495, 0.00692212, 0.00654495, 0.00485456, 0, 0,
    0, 0, 0.00654495, 0.00882395, 0.00933246, 0.00882395, 0.00654495, 0, 0,
    0, 0, 0.00692212, 0.00933246, 0.00987028, 0.00933246, 0.00692212, 0, 0,
    0, 0, 0.00654495, 0.00882395, 0.00933246, 0.00882395, 0.00654495, 0, 0,
    0, 0, 0.00485456, 0.00654495, 0.00692212, 0.00654495, 0.00485456, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0;

    sols[5] << 0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0.0143993,  0.0171093,  0.0128539,  0.00560752,  0.00132035,  0,  0,
    0,  0,  0.0171093,  0.020337,  0.0153047,  0.00672524,  0.00162962,  0,  0,
    0,  0,   0.0128539,  0.0153047,  0.0115881,  0.00521785,  0.00138201,  0,  0,
    0,  0,  0.00560752,  0.00672524,  0.00521785,  0.00256922, 0.000880626,  0,  0,
    0,  0,  0.00132035,  0.00162962,  0.00138201, 0.000880626, 0.000468463,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0,
    0,  0,  0,  0,  0,  0,  0,  0,  0;

    for (bias=1; bias<4; bias++) {
        for (fishmodel=0; fishmodel < 2; fishmodel++) {
            populateBehaviorGrid(&tGrid, &bGrid, cellSize, ousdx, ousdy,
                                  oucor, mux, muy, fishmodel);
            calculateGoodnessGrid(&tGrid, &bGrid, &gGrid, bias,
                                  range, peak, sd);
            //gGrid.printData();
            methodID = "checkCalculateGoodness bias:" + std::to_string(bias) + " fishmodel:" + std::to_string(fishmodel);
            result = result & compareMatrix(&gGrid.data, &sols[(bias-1) * 2 + fishmodel], methodID);
        }
    }
    return result;
}
/**
 * Checks the cumulative distribution functions.
 */
bool checkCDistPartition() {
    int i = 0,
            size = 9;
        double start[9] = {-3, -2, -1,  0, 0, -3, -.5, -1, 1},
               end[9] =   {0,   0,  0,  1, 0,  3,  .5,  1, 0},
               solution[9] = {0.49865, 0.47725, 0.341345, 0.341345, 0, 0.9973, 0.382925, 0.682689, -0.341345},
               result[9],
               sd = 1,
               mean = 0;
        for(i = 0;i < size;i ++) {
            result[i] = cdistPartition(mean,  sd,  start[i],  end[i]);
            compare(result[i], solution[i], "checkNormalProb");
        }
        std::cout << "Passed: " << "checkCDistPartition\n";
        return true;
}
/**
 * Checks the normal distribution functions.
 */
bool checkNormalProb() {
    int i = 0,
        size = 9;
    double challenge[9] = {-3, -2, -1, -0.5, 0, .5, 1, 2, 3},
           solution[9] = {0.011109, 0.135335, 0.606531, 0.882497, 1, 0.882497, 0.606531, 0.135335, 0.011109},
           result[9],
           peak = 1,
           sd = 1;
    for(i = 0;i < size;i ++) {
        result[i] = normalProb(peak, sd,challenge[i]);
        compare(result[i], solution[i], "checkNormalProb");
    }
    std::cout << "Passed: " << "checkNormalProb\n";
    return true;
}


/**Compares two numbers, checking if the difference is within an acceptable
 * tolerance.
 */
bool compare(double expectedValue, double receivedValue, std::string functionName) {
    double delta = abs(expectedValue - receivedValue);
    if (delta >= tolerance) {
        std::cout << "Error in " << functionName << "\nExpected value: "
                             << expectedValue << "\nRecieved value: "
                             << receivedValue <<"\n";
        return false;
    }
    return true;
}

/**
 * Compares two Eigen Matricies, returning true if they're both within
 */
bool compareMatrix(Eigen::MatrixXd* expectedMatirx,
                   Eigen::MatrixXd* receivedMatrix, std::string functionName) {
    int i = 0,
        j = 0,
        size = expectedMatirx->rows();
    Eigen::MatrixXd delta;
    delta.setConstant(0);
    delta = (*expectedMatirx) - (*receivedMatrix);
    delta = delta.cwiseAbs();
    for(i = 0;i < size;i ++) {
        for(j = 0;j < size;j ++) {
            if (delta(i, j) > tolerance) {
                std::cout << delta (i,j);
                std::cout << "Error in " << functionName << "\nExpected value: "
                     << (*expectedMatirx)(i,j) << "\nRecieved value: "
                     << (*receivedMatrix)(i,j) << " @ " << i << ", " << j
                     <<"\n";
                return false;
            }
        }
    }
    std::cout << "Passed: " << functionName << "\n";
    return true;
}


bool checkDetectionGradient() {
    //a 5x5 matrix
        int rng = 2,
            size = 2 * rng + 1;
        double sensorPeakDetectionProbability = 1,
               SDofSensorDetectionRange = 1;
        Eigen::MatrixXd solution;
        Eigen::MatrixXd distanceGradient;
        Eigen::MatrixXd detectionGradient;
        solution.resize(size,size);
        solution << 0.0183156,  0.082085,  0.135335,  0.082085, 0.0183156,
                     0.082085,  0.367879,  0.606531,  0.367879,  0.082085,
                     0.135335,  0.606531,  1,         0.606531,  0.135335,
                     0.082085,  0.367879,  0.606531,  0.367879,  0.082085,
                    0.0183156,  0.082085,  0.135335,  0.082085, 0.0183156;
        makeDistGradient(&distanceGradient, 2);
        detectionGradient.resize(size,size);
        makeDetectionGradient(&detectionGradient, &distanceGradient,
                              sensorPeakDetectionProbability,
                              SDofSensorDetectionRange);
        return compareMatrix(&solution, &detectionGradient, "checkDetectionGradient");
}


bool checkDistGradient() {
    //a 5x5 matrix
    int rng = 2,
        size = 2 * rng + 1;
    Eigen::MatrixXd solution;
    Eigen::MatrixXd distanceGradient;
    solution.resize(size,size);
    solution << 2.82843,   2.23607,  2,  2.23607,  2.82843,
                2.23607,   1.41421,  1,  1.41421,  2.23607,
                      2,         1,  1,        1,        2,
                2.23607,   1.41421,  1,  1.41421,  2.23607,
                2.82843,   2.23607,  2,  2.23607,  2.82843;

    makeDistGradient(&distanceGradient, 2);
    return compareMatrix(&distanceGradient, &solution, "checkDistGradient");
}
/**
 * Calls calcPercentViz() with a square matrix to test LoS correctness.
 */
bool checkCalcPercentViz () {
	Grid* challenge = new Grid(5, 5, "Challenge");
	challenge->data <<   1,  1,   1,  8,  8,
						 1,  7,   7,  7,  8,
						-7, -7,   0,  7, 14,
						-1, -7,  -7, -7, -8,
						-1, -1, -14, -8, -8;
	Grid* solution = new Grid(5, 5, "Solution");
	solution->data <<   14, 15.6525,  14, 15.6525,      14,
                    11.068,       7,   7,       7,  11.068,
                        -7,      -7,    0,       7,      14,
                        -1,      -7,  -7,      -7, 15.6525,
                        -1,      -1, -14,      -8,      -8;

	Eigen::MatrixXd distGradient;
	makeDistGradient(&distGradient,2);
	Eigen::MatrixXd result;
	Eigen::MatrixXd localTopo;
	Eigen::MatrixXd temp;
	calcVizGrid(challenge, &distGradient, &result, &localTopo, &temp, 2, 2, 2);
	temp = solution->data;
	return compareMatrix(&result, &temp, "checkCalcPercentViz");
}


/**
 * Calls getCells() with the given origin and target parameters, and compares the result to the solution parameter.  If they match, return true.
 */
bool testGetCells(std::pair<int,int> *origin, std::pair<int,int> *target, std::set<std::pair<int,int>> *solution, std::string errorMsg) {
	std::vector<std::pair<int,int>> vec = getCells(origin, target);
	std::set<std::pair<int,int>> pairs(vec.begin(),vec.end());
		for (std::set<std::pair<int,int> >::iterator it = pairs.begin(); it != pairs.end(); ++it) {
			std::pair<int,int>  i = *it;
			if(pairs.find(i)==pairs.end()) {
				std::cout<<"ERROR in :"<<errorMsg<<".\n";
				std::cout<<"Expected:\n";
				printSet(solution);
				std::cout<<"\n";
				std::cout<<"Recieved:\n";
				printSet(&pairs);
				return false;
			}
		}
		std::cout<<"Passed: "<<errorMsg <<"\n";
		return true;
}



/**
 * Calls getcells() with a positive steep slope.
 */
bool positiveSteep() {
	std::pair<int,int> origin = std::make_pair(1,1),
				  target = std::make_pair(2,5);
	std::set<std::pair<int,int>>  solution = {
											std::make_pair(1,2),
											std::make_pair(1,3),
											std::make_pair(2,3),
											std::make_pair(2,4),
											std::make_pair(2,5)};
	std::string errorMsg = "positiveSteep";
	return testGetCells(&origin, &target, &solution, errorMsg);
}

/**
 * Calls getcells() with a negative steep slope.
 */
bool negativeSteep() {
	std::pair<int,int> origin = std::make_pair(2,5),
							 target = std::make_pair(1,1);
	std::set<std::pair<int,int>>  solution = {
											std::make_pair(1,1),
											std::make_pair(1,2),
											std::make_pair(1,3),
											std::make_pair(2,3),
											std::make_pair(2,4)};
	std::string errorMsg = "negativeSteep";
	return testGetCells(&origin, &target, &solution, errorMsg);
}

/**
 * Calls getcells() with a positive slow slope.
 */
bool positiveSlow() {
	std::pair<int,int> origin = std::make_pair(1,1),
				  target = std::make_pair(5,2);
	std::set<std::pair<int,int>>  solution = {
											std::make_pair(2,1),
											std::make_pair(3,1),
											std::make_pair(3,2),
											std::make_pair(4,2),
											std::make_pair(5,2)};
	std::string errorMsg = "positiveSlow";
	return testGetCells(&origin, &target, &solution, errorMsg);
}

/**
 * Calls getcells() with a negative slow slope.
 */
bool negativeSlow() {
	std::pair<int,int> origin = std::make_pair(5,2),
				  target = std::make_pair(1,1);
	std::set<std::pair<int,int>>  solution = {
											std::make_pair(1,1),
											std::make_pair(2,1),
											std::make_pair(3,1),
											std::make_pair(3,2),
											std::make_pair(4,2)};
	std::string errorMsg = "negativeSlow";
	return testGetCells(&origin, &target, &solution, errorMsg);
}

/**
 * Calls getcells() with a positive horizontal slope.
 */
bool positiveHorizontal() {
	std::pair<int,int> origin = std::make_pair(1,1),
				  target = std::make_pair(5,1);
	std::set<std::pair<int,int>>  solution = {
											std::make_pair(2,1),
											std::make_pair(3,1),
											std::make_pair(4,1),
											std::make_pair(5,1)};
	std::string errorMsg = "positiveHorizontal";
	return testGetCells(&origin, &target, &solution, errorMsg);
}

/**
 * Calls getcells() with a negative horizontal slope.
 */
bool negativeHorizontal() {
	std::pair<int,int> origin = std::make_pair(5,1),
				  target = std::make_pair(1,1);
	std::set<std::pair<int,int>>  solution = {
											std::make_pair(1,1),
											std::make_pair(2,1),
											std::make_pair(3,1),
											std::make_pair(4,1)};
	std::string errorMsg = "negativeHorizontal";
	return testGetCells(&origin, &target, &solution, errorMsg);
}

/**
 * Calls getcells() with a positive vertical slope.
 */
bool positiveVertical() {
	std::pair<int,int> origin = std::make_pair(1,1),
				  target = std::make_pair(1,5);
	std::set<std::pair<int,int>>  solution = {
											std::make_pair(1,2),
											std::make_pair(1,3),
											std::make_pair(1,4),
											std::make_pair(1,5)};
	std::string errorMsg = "positiveVertical";
	return testGetCells(&origin, &target, &solution, errorMsg);
}

/**
 * Calls getcells() with a negative vertical slope.
 */
bool negativeVertical() {
	std::pair<int,int> origin = std::make_pair(1,5),
			      target = std::make_pair(1,1);
	std::set<std::pair<int,int>>  solution = {
											std::make_pair(1,1),
											std::make_pair(1,2),
											std::make_pair(1,3),
											std::make_pair(1,4)};
	std::string errorMsg = "negativeVertical";
	return testGetCells(&origin, &target, &solution, errorMsg);
}

/**
 * Calls getcells() with duplicate points.
 */
bool duplicatePoint() {
	std::pair<int,int> origin = std::make_pair(1,1),
				  target = std::make_pair(1,1);
	std::vector<std::pair<int,int>> pairs = getCells(&origin, &target);
	if (pairs.size()==0) {
		return true;
	}
	std::cout<< "ERROR in duplicatePoint.\n";
	std::cout<<"Expected: size 0.\n";
	std::cout<<"Recieved: size "<<pairs.size()<<".\n";
	return false;
}

/**
 * Tests selectTopSpots().
 */
bool testSelectTopSpots() {
    int sensorRange = 2,
        numOptimalSensors = 3,
        numUserSensors = 0,
        numTotalSensors = numOptimalSensors + numUserSensors;
    double sensorPeakDetectionProbability = 1,
           SDofSensorDetectionRange = 1;
    border = sensorRange;

    int size = 2 * (sensorRange + border) + 1;
    Grid* goodnessGrid = new Grid(size, size,"goodness");
    goodnessGrid->data << 0, 0, 0, 0, 0, 0, 0, 0, 0,
                          0, 0, 0, 0, 0, 0, 0, 0, 0,
                          0, 0, 1, 1, 2, 1, 1, 0, 0,
                          0, 0, 1, 1, 1, 1, 1, 0, 0,
                          0, 0, 1, 1, 1, 1, 1, 0, 0,
                          0, 0, 1, 3, 1, 2, 1, 0, 0,
                          0, 0, 1, 1, 1, 1, 1, 0, 0,
                          0, 0, 0, 0, 0, 0, 0, 0, 0,
                          0, 0, 0, 0, 0, 0, 0, 0, 0;
    Eigen::MatrixXd bestSensors;
    Eigen::MatrixXd userSensors;
    bestSensors.resize(numOptimalSensors, 3);
    userSensors.resize(numUserSensors,2);
    bestSensors.setConstant(0);
    userSensors.setConstant(0);
    selectTopSpots(goodnessGrid, &bestSensors, &userSensors,
                   numTotalSensors, sensorRange,
                   sensorPeakDetectionProbability,
                   SDofSensorDetectionRange);

    // Solutions
    Eigen::MatrixXd solGoodnessGrid;
    solGoodnessGrid.resize(size,size);
    solGoodnessGrid <<  0, 0, 0,        0,        0,        0,        0,        0, 0,
                        0, 0, 0,        0,        0,        0,        0,        0, 0,
                        0, 0, 0.864665, 0.393469, 0,        0.393469, 0.864665, 0, 0,
                        0, 0, 0.842568, 0.536562, 0.331525, 0.536562, 0.842568, 0, 0,
                        0, 0, 0.620543, 0.331525, 0.3455,   0.331525, 0.620543, 0, 0,
                        0, 0, 0.393469, 0,        0.154818, 0,        0.393469, 0, 0,
                        0, 0, 0.632121, 0.361171, 0.399576, 0.361171, 0.632121, 0, 0,
                        0, 0, 0,        0,        0,        0,        0,        0, 0,
                        0, 0, 0,        0,        0,        0,        0,        0, 0;
    Eigen::MatrixXd solBestSensors;
    solBestSensors.resize(numTotalSensors,3);
    solBestSensors << 5, 3, 3,
                      2, 4, 2,
                      5, 5, 1.72933;

    bool result = compareMatrix(goodnessGrid->getDataPointer(), &solGoodnessGrid, "testSelectTopSpots-GoodnessGrid");
    result = result & compareMatrix(&bestSensors, &solBestSensors, "testSelectTopSpots-BestSensors");
    return result;
    return true;
}
