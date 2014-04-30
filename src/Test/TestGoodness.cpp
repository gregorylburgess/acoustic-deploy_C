#include <stdlib.h>
#include <iostream>
#include <set>
#include <string>
#include "../Goodness.h"
#include "TestGoodness.h"
using namespace std;


/**
 * Executes all tests in TestGoodness.cpp.  Returns true if all tests pass, flase otherwise.
 */
bool runGoodnessTests() {
	bool success = (
			positiveSteep() &&
			negativeSteep() &&
			positiveSlow() &&
			negativeSlow() &&
			positiveHorizontal() &&
			negativeHorizontal() &&
			positiveVertical() &&
			negativeVertical() &&
			duplicatePoint()
			);
	if (success) {
		cout<<"All Goodness Tests Passed!";
	}
	return success;
}

void printSet(std::set<std::pair<double,double> > *pairs) {
	for (std::set<std::pair<double,double> >::iterator it = pairs->begin(); it != pairs->end(); ++it) {
		std::pair<double,double>  i = *it;
	    cout <<"("<< i.first <<","<< i.second<<")\n";
	}
}

/**
 * Calls getCells() with the given origin and target parameters, and compares the result to the solution parameter.  If they match, return true.
 */
bool check(std::pair<double,double> *origin, std::pair<double,double> *target, std::set<std::pair<double,double>> *solution, string errorMsg) {
	std::set<std::pair<double,double>> pairs = getCells(origin, target);
		for (std::set<std::pair<double,double> >::iterator it = pairs.begin(); it != pairs.end(); ++it) {
			std::pair<double,double>  i = *it;
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
		return true;
}

/**
 * Calls getcells() with a positive steep slope.
 */
bool positiveSteep() {
	std::pair<double,double> origin = make_pair(1.0,1.0),
							 target = make_pair(2.0,5.0);
	std::set<std::pair<double,double>>  solution = {
											make_pair(1,2),
											make_pair(1,3),
											make_pair(2,3),
											make_pair(2,4),
											make_pair(2,5)};
	string errorMsg = "positiveSteep";
	return check(&origin, &target, &solution, errorMsg);
}

/**
 * Calls getcells() with a negative steep slope.
 */
bool negativeSteep() {
	std::pair<double,double> origin = make_pair(2.0,5.0),
							 target = make_pair(1.0,1.0);
	std::set<std::pair<double,double>>  solution = {
											make_pair(1,1),
											make_pair(1,2),
											make_pair(1,3),
											make_pair(2,3),
											make_pair(2,4)};
	string errorMsg = "positiveSteep";
	return check(&origin, &target, &solution, errorMsg);
}

/**
 * Calls getcells() with a positive slow slope.
 */
bool positiveSlow() {
	std::pair<double,double> origin = make_pair(1.0,1.0),
							 target = make_pair(5.0,2.0);
	std::set<std::pair<double,double>>  solution = {
											make_pair(2,1),
											make_pair(3,1),
											make_pair(3,2),
											make_pair(4,2),
											make_pair(5,2)};
	string errorMsg = "positiveSlow";
	return check(&origin, &target, &solution, errorMsg);
}

/**
 * Calls getcells() with a negative slow slope.
 */
bool negativeSlow() {
	std::pair<double,double> origin = make_pair(5.0,2.0),
							 target = make_pair(1.0,1.0);
	std::set<std::pair<double,double>>  solution = {
											make_pair(1,1),
											make_pair(2,1),
											make_pair(3,1),
											make_pair(3,2),
											make_pair(4,2)};
	string errorMsg = "negativeSlow";
	return check(&origin, &target, &solution, errorMsg);
}

/**
 * Calls getcells() with a positive horizontal slope.
 */
bool positiveHorizontal() {
	std::pair<double,double> origin = make_pair(1.0,1.0),
							 target = make_pair(5.0,1.0);
	std::set<std::pair<double,double>>  solution = {
											make_pair(2,1),
											make_pair(3,1),
											make_pair(4,1),
											make_pair(5,1)};
	string errorMsg = "positiveHorizontal";
	return check(&origin, &target, &solution, errorMsg);
}

/**
 * Calls getcells() with a negative horizontal slope.
 */
bool negativeHorizontal() {
	std::pair<double,double> origin = make_pair(5.0,1.0),
							 target = make_pair(1.0,1.0);
	std::set<std::pair<double,double>>  solution = {
											make_pair(1,1),
											make_pair(2,1),
											make_pair(3,1),
											make_pair(4,1)};
	string errorMsg = "negativeHorizontal";
	return check(&origin, &target, &solution, errorMsg);
}

/**
 * Calls getcells() with a positive vertical slope.
 */
bool positiveVertical() {
	std::pair<double,double> origin = make_pair(1.0,1.0),
							 target = make_pair(1.0,5.0);
	std::set<std::pair<double,double>>  solution = {
											make_pair(1,2),
											make_pair(1,3),
											make_pair(1,4),
											make_pair(1,5)};
	string errorMsg = "positiveVertical";
	return check(&origin, &target, &solution, errorMsg);
}

/**
 * Calls getcells() with a negative vertical slope.
 */
bool negativeVertical() {
	std::pair<double,double> origin = make_pair(1.0,5.0),
							 target = make_pair(1.0,1.0);
	std::set<std::pair<double,double>>  solution = {
											make_pair(1,1),
											make_pair(1,2),
											make_pair(1,3),
											make_pair(1,4)};
	string errorMsg = "negativeVertical";
	return check(&origin, &target, &solution, errorMsg);
}

/**
 * Calls getcells() with duplicate points.
 */
bool duplicatePoint() {
	std::pair<double,double> origin = make_pair(1.0,1.0),
							 target = make_pair(1.0,1.0);
	std::set<std::pair<double,double>> pairs = getCells(&origin, &target);
	if (pairs.size()==0) {
		return true;
	}
	cout<< "ERROR in duplicatePoint.\n";
	cout<<"Expected: size 0.\n";
	cout<<"Recieved: size "<<pairs.size()<<".\n";
	printSet(&pairs);
}
