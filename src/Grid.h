#pragma once
#include <string>
#include <Dense> //Eigen/Dense
using namespace std;

/**
 * Represents a matrix of data.
 */
class Grid {
	public:
		int rows, cols;
		Eigen::MatrixXd data;
		string name;

		Grid();
		Grid(int Cols, int Rows, string name);
		Grid(Grid* mat, string name);
		Grid(Eigen::MatrixXd dat, string newName);
		void clearNA();
		void printData();
		void replace(double find, double replace);
		void setAll(double val);
		double nanCheck(double x);
};
