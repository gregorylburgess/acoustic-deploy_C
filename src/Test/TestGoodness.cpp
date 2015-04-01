#include <stdlib.h>
#include <iostream>
#include <set>
#include <string>
#include "../Goodness.h"
#include "../Utility.h"
#include "TestGoodness.h"
using namespace std;
//Don't set this too low, as we'll get errors when using compiler
    //optimization... (google "floating point comparison c++")
double tolerance = 0.0001;



/**
 * Executes all tests in TestGoodness.cpp.  Returns true if all tests pass, false otherwise.
 */
bool runGoodnessTests() {
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
			checkCalculateGoodness()
			);
	if (success) {
		cout<<"All Goodness Tests Passed!";
	}
	else {
		cout<<"Failures occurred!";
	}
	return success;
}


bool checkCalculateGoodness() {
    calculateGoodnessGrid(Grid* topographyGrid, Grid* behaviorGrid,
                              Grid* goodnessGrid, int bias, int range,
                              double peak, double sd);
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
        cout << "Passed: " << "checkCDistPartition\n";
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
    cout << "Passed: " << "checkNormalProb\n";
    return true;
}


/**Compares two numbers, checking if the difference is within an acceptable
 * tolerance.
 */
bool compare(double expectedValue, double receivedValue, string functionName) {
    double delta = abs(expectedValue - receivedValue);
    if (delta >= tolerance) {
        cout << "Error in " << functionName << "\nExpected value: "
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
                   Eigen::MatrixXd* receivedMatrix, string functionName) {
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
                cout << delta (i,j);
                cout << "Error in " << functionName << "\nExpected value: "
                     << (*expectedMatirx)(i,j) << "\nRecieved value: "
                     << (*receivedMatrix)(i,j) << " @ " << i << ", " << j
                     <<"\n";
                return false;
            }
        }
    }
    cout << "Passed: " << functionName << "\n";
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
                     0.135335,  0.606531,  0.606531,  0.606531,  0.135335,
                     0.082085,  0.367879,  0.606531,  0.367879,  0.082085,
                    0.0183156,  0.082085,  0.135335,  0.082085, 0.0183156;
        makeDistGradient(&distanceGradient, 2);
        detectionGradient.resize(size,size);
        makeDetectionGradient(&detectionGradient, &distanceGradient,
                sensorPeakDetectionProbability, SDofSensorDetectionRange);
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
bool testGetCells(pair<int,int> *origin, pair<int,int> *target, set<pair<int,int>> *solution, string errorMsg) {
	vector<pair<int,int>> vec = getCells(origin, target);
	set<pair<int,int>> pairs(vec.begin(),vec.end());
		for (set<pair<int,int> >::iterator it = pairs.begin(); it != pairs.end(); ++it) {
			pair<int,int>  i = *it;
			if(pairs.find(i)==pairs.end()) {
				cout<<"ERROR in :"<<errorMsg<<".\n";
				cout<<"Expected:\n";
				printSet(solution);
				cout<<"\n";
				cout<<"Recieved:\n";
				printSet(&pairs);
				return false;
			}
		}
		cout<<"Passed: "<<errorMsg <<"\n";
		return true;
}



/**
 * Calls getcells() with a positive steep slope.
 */
bool positiveSteep() {
	pair<int,int> origin = make_pair(1,1),
				  target = make_pair(2,5);
	set<pair<int,int>>  solution = {
											make_pair(1,2),
											make_pair(1,3),
											make_pair(2,3),
											make_pair(2,4),
											make_pair(2,5)};
	string errorMsg = "positiveSteep";
	return testGetCells(&origin, &target, &solution, errorMsg);
}

/**
 * Calls getcells() with a negative steep slope.
 */
bool negativeSteep() {
	pair<int,int> origin = make_pair(2,5),
							 target = make_pair(1,1);
	set<pair<int,int>>  solution = {
											make_pair(1,1),
											make_pair(1,2),
											make_pair(1,3),
											make_pair(2,3),
											make_pair(2,4)};
	string errorMsg = "negativeSteep";
	return testGetCells(&origin, &target, &solution, errorMsg);
}

/**
 * Calls getcells() with a positive slow slope.
 */
bool positiveSlow() {
	pair<int,int> origin = make_pair(1,1),
				  target = make_pair(5,2);
	set<pair<int,int>>  solution = {
											make_pair(2,1),
											make_pair(3,1),
											make_pair(3,2),
											make_pair(4,2),
											make_pair(5,2)};
	string errorMsg = "positiveSlow";
	return testGetCells(&origin, &target, &solution, errorMsg);
}

/**
 * Calls getcells() with a negative slow slope.
 */
bool negativeSlow() {
	pair<int,int> origin = make_pair(5,2),
				  target = make_pair(1,1);
	set<pair<int,int>>  solution = {
											make_pair(1,1),
											make_pair(2,1),
											make_pair(3,1),
											make_pair(3,2),
											make_pair(4,2)};
	string errorMsg = "negativeSlow";
	return testGetCells(&origin, &target, &solution, errorMsg);
}

/**
 * Calls getcells() with a positive horizontal slope.
 */
bool positiveHorizontal() {
	pair<int,int> origin = make_pair(1,1),
				  target = make_pair(5,1);
	set<pair<int,int>>  solution = {
											make_pair(2,1),
											make_pair(3,1),
											make_pair(4,1),
											make_pair(5,1)};
	string errorMsg = "positiveHorizontal";
	return testGetCells(&origin, &target, &solution, errorMsg);
}

/**
 * Calls getcells() with a negative horizontal slope.
 */
bool negativeHorizontal() {
	pair<int,int> origin = make_pair(5,1),
				  target = make_pair(1,1);
	set<pair<int,int>>  solution = {
											make_pair(1,1),
											make_pair(2,1),
											make_pair(3,1),
											make_pair(4,1)};
	string errorMsg = "negativeHorizontal";
	return testGetCells(&origin, &target, &solution, errorMsg);
}

/**
 * Calls getcells() with a positive vertical slope.
 */
bool positiveVertical() {
	pair<int,int> origin = make_pair(1,1),
				  target = make_pair(1,5);
	set<pair<int,int>>  solution = {
											make_pair(1,2),
											make_pair(1,3),
											make_pair(1,4),
											make_pair(1,5)};
	string errorMsg = "positiveVertical";
	return testGetCells(&origin, &target, &solution, errorMsg);
}

/**
 * Calls getcells() with a negative vertical slope.
 */
bool negativeVertical() {
	pair<int,int> origin = make_pair(1,5),
			      target = make_pair(1,1);
	set<pair<int,int>>  solution = {
											make_pair(1,1),
											make_pair(1,2),
											make_pair(1,3),
											make_pair(1,4)};
	string errorMsg = "negativeVertical";
	return testGetCells(&origin, &target, &solution, errorMsg);
}

/**
 * Calls getcells() with duplicate points.
 */
bool duplicatePoint() {
	pair<int,int> origin = make_pair(1,1),
				  target = make_pair(1,1);
	vector<pair<int,int>> pairs = getCells(&origin, &target);
	if (pairs.size()==0) {
		return true;
	}
	cout<< "ERROR in duplicatePoint.\n";
	cout<<"Expected: size 0.\n";
	cout<<"Recieved: size "<<pairs.size()<<".\n";
	return false;
}
