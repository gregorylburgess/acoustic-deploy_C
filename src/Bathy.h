//============================================================================
// Name        : acousticDeploy_C.cpp
// Author      : Greg Burgess
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : Hello World in C++, Ansi-style
//============================================================================

#include "Grid.h"
#include "Utility.h"


Grid simulatetopographyGrid(int XDist, int YDist);
Grid getBathy(std::string inputFile, std::string inputFileType, int startX, int startY,
			  int XDist, int YDist, std::string seriesName, long timestamp, bool debug);



