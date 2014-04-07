#include <string>
using namespace std;

/**
 * Represents a matrix of data.
 */
class Grid {
	public:
		int rows, cols;
		double* data;
		string name;

		Grid();
		Grid(int Cols, int Rows, string Name);
		Grid(Grid* mat);
		void clearNA(double val);
		void printData();
		void writeMat();
		void writeDat();
		void replace(double find, double replace);
		void setAll(double val);
		double min();
		double max();
		double sum();
};
