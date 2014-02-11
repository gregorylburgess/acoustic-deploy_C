#include <string>
class Grid {
	public:
		int rows, cols;
		double* data;
		Grid();
		Grid(int Rows, int Cols);
		Grid(Grid* mat);
		void clearNA(double val);
		void printData();
		void GNUwrite(std::string filename);
};
