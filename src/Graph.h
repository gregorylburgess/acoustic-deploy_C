#pragma once
#include <string>
using namespace std;
class Graph {
	public:
		bool contour;
		//Matricies are grids of z values.
		//An element's position in the matrix gives it's x/y value.
		string inputMatFile;
		//An input Data file is a list of x,y,z data points where each element is separated by a space.
		//One x,y,z tuple per line.
		//non-contiguous lines are separated by an empty line.
		string inputDatFile;
		//A contour data file is an input data file that only contains contour information.
		string contourDataFile;
		Grid* grid;
		Graph(Grid* g);
		void printContour(int contourLevels[]);
		void  printContourGraph(int width, int height);
		void writeDat();
		void writeMat();
};
