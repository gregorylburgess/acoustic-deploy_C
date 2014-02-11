
#include "Bathy.h"
#include "Utility.h"
#include <stdlib.h>
#include <iostream>
#include <string>
using namespace std;

int main() {
	string inputFile = "himbsyn.bathy.v19.grd",
		   inputFileType="netcdf",
		   seriesName="z";
	int startX = 7000,
	    startY = 7000,
	    XDist = 3000,
	    YDist = 3000;
	long timestamp = -1;
	bool debug = false;

	Grid tGrid = getBathy(inputFile, inputFileType, startX, startY,XDist, YDist, seriesName, timestamp, debug);
	//Grid tGrid = simulatetopographyGrid(XDist,YDist);

	string  title = "test",
	dataFilePath = "data/test.dat";
	int width = 500,
		height = 500;
	//tGrid.printData();
	tGrid.GNUwrite(dataFilePath);
	cout << "Printing" << "\n";
	try {
		printGraph(title, XDist,YDist, dataFilePath, width, height);
	}
	catch(int e) {
		return 0;
	}
	cout << "Done" << "\n";
	return 0;
}

