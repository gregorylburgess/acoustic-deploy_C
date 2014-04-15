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
		void clearNA();
		void printData();
		void replace(double find, double replace);
		void setAll(double val);
		void writeDat();
		void writeMat();
		double min();
		double max();
		double nanCheck(double x);
		double sum();
};
