//============================================================================
// Name        : Main.cpp
// Author      : Greg Burgess
// Version     : 1.0
// Copyright   : Copyright 2014
// Description : Runs the program.
//============================================================================
#include <stdlib.h>
#include <time.h>
#include <Dense>
#include <algorithm>
#include <iostream>
#include <string>
#include <vector>
#include "Bathy.h"
#include "FishModel.h"
#include "GlobalVars.h"
#include "Goodness.h"
#include "Graph.h"
#include "Utility.h"
#include "Test/Test.h"
#include "Test/TestGoodness.h"

int main() {
    bool test = false, simulateBathy = false;

    if (test) {
        runTests();
        return 0;
    }

    acousticParams.insert({ "debug", "0" });

    acousticParams.insert({ "cellSize", "5" });
    acousticParams.insert({ "fishmodel", "0" });
    acousticParams.insert({ "sensorRange", "50" });
    acousticParams.insert({ "suppressionRangeFactor","1"}),
    acousticParams.insert({ "userSensors", "100,100,0,0,100,0,0,300" });
    acousticParams.insert({ "numOptimalSensors", "20" });
    acousticParams.insert({ "numProjectedSensors", "10" });
    acousticParams.insert({ "bias", "2" });

    acousticParams.insert({ "ousdx", ".1" });
    acousticParams.insert({ "ousdy", ".1" });
    acousticParams.insert({ "oucor", "0" });
    acousticParams.insert({ "mux", ".5" });
    acousticParams.insert({ "muy", ".5" });
    acousticParams.insert({ "fishmodel", "1" });
    acousticParams.insert({ "minDepth", "15" });
    acousticParams.insert({ "maxDepth", "30" });
    acousticParams.insert({ "meanRelativePosition", ".9" });
    acousticParams.insert({ "RelativePositionSD", ".05" });
    // acousticParams.insert({"inputFile","himbsyn.bathy.v19.grd"});
    // acousticParams.insert({"inputFile","himbsyn.bathytopo.1km.v19.grd"});
    acousticParams.insert({ "inputFile", "pal_5m.asc" });
    acousticParams.insert({ "inputFileType", "asc" });
    acousticParams.insert({ "seriesName", "z" });
    acousticParams.insert({ "timestamp", "-1" });
    acousticParams.insert({ "logScaleGraphColoring", "1" });
    acousticParams.insert({ "contourDepths", "0,-20,-40,-80" });
    int    startRow = 300,
           startCol = 200,  // 450,340,200,200 (1km)
           rowDist = 501,   // 100 0 800 1500 (5m)
           colDist = 301,   // 300 200 501 501 (palmyra)
           height = 1000,
           width = 1000,
           bias = 3,
           sensorRange = 4,
           peak = 1,
           sd = 1,
           i = 0,
           row = 0,
           col = 0,
           cellSize = std::stoi(acousticParams["cellSize"]),
           numOptimalSensors =
                   std::stoi(acousticParams["numOptimalSensors"]),
           numProjectedSensors =
                   std::stoi(acousticParams["numProjectedSensors"]);
           border = sensorRange;
    double suppressionRangeFactor = std::stod(acousticParams["suppressionRangeFactor"]),
           ousdx     = std::stod(acousticParams["ousdx"]),
           ousdy     = std::stod(acousticParams["ousdy"]),
           oucor     = std::stod(acousticParams["oucor"]),
           mux       = std::stod(acousticParams["mux"]),
           muy       = std::stod(acousticParams["muy"]),
           fishmodel = std::stod(acousticParams["fishmodel"]);


    // TODO(Greg) Data validation
    // Parse User Sensor Placements
    std::cout << "\nReading userSensor List...\n";
    std::vector<std::string> userSensors;
    parseCDString(&userSensors, acousticParams["userSensors"], ',');
    Eigen::MatrixXd userSensorList;
    userSensorList.resize(userSensors.size()/2, 3);
    for (i = 0; i < userSensorList.rows(); i ++) {
        row = std::stoi(userSensors[2 * i]);
        col = std::stoi(userSensors[2 * i + 1]);
        if (row < 0 || col < 0 || row >= rowDist || col >= colDist) {
            printError("A user-defined sensor is out of bounds.", 1,
                       acousticParams["timestamp"]);
        }
        // Translate user points to our internal grid
        userSensorList(i, 0) = row;
        userSensorList(i, 1) = col;
    }


    clock_t begin, end, vizBegin, vizEnd;
    double vizDelta, timeSpent;
    begin = clock();

    // Compute contour depth meta data (used for graphical output)
    std::vector<std::string> contourLevels;
    parseCDString(&contourLevels, acousticParams["contourDepths"], ',');
    // Note the number of contours we need to graph
    acousticParams.insert({ "numContourDepths",
                            std::to_string(contourLevels.size()) });

    // TODO(Greg) Sort is broken, throws segfaults.  Possibly b/c file doesn't
    //   exist (tried with pal5m.asc).  fix plx.
    // sort(&contourLevels, &contourLevels + numContourDepths);

    // File path variables
    std::string outputDataFilePath = "data/", outputDataFileType = ".dat",
           bathymetryTitle = "Topography", habitatTitle = "Habitat",
           goodnessTitle = "Goodness",
           coverageTitle = "Acoustic Coverage",
           bathymetryFilePath = outputDataFilePath + bathymetryTitle +
                                outputDataFileType,
           habitatFilePath = outputDataFilePath + habitatTitle +
                             outputDataFileType,
           goodnessFilePath = outputDataFilePath + goodnessTitle +
                             outputDataFileType,
           coverageFilePath = outputDataFilePath + coverageTitle +
                             outputDataFileType;

    Grid bGrid(rowDist + 2 * border, colDist + 2 * border, "Behavior");
    Grid gGrid(rowDist + 2 * border, colDist + 2 * border, "Goodness");
    Grid tGrid(rowDist + 2 * border, colDist + 2 * border, "Topography");
    tGrid.data.setConstant(0);

    // Fetch or simulate topography
    std::cout << "Getting topography...";
    if (simulateBathy) {
        // Simulate topography
        simulatetopographyGrid(&tGrid, rowDist, colDist);
    } else {
        // Fetch topography
        getBathy(&tGrid, acousticParams["inputFile"],
                 acousticParams["inputFileType"], size_t(startRow),
                 size_t(startCol), size_t(rowDist), size_t(colDist),
                 acousticParams["seriesName"], acousticParams["timestamp"]);
    }

    // Fill in Behavior Grid
    std::cout << "\nGetting Behavior...";
    populateBehaviorGrid(&tGrid, &bGrid, cellSize, ousdx, ousdy, oucor, mux,
                         muy, fishmodel);
    vizBegin = clock();
    std::cout << "\nGetting Goodness...\n";

    // Calculate good sensor locations
    calculateGoodnessGrid(&tGrid, &bGrid, &gGrid, bias, sensorRange, peak, sd);

    // Check if we should proceed...
    if (gGrid.data.sum() <= 0) {
        printError("No positive coefficients found in the goodness Grid. Aborting",
                0, acousticParams["timestamp"]);
    }
    vizEnd = clock();
    vizDelta = static_cast<double>(end - begin) / CLOCKS_PER_SEC;


    // Find optimal placements
    std::cout << "\nPlacing Sensors...\n";
    Eigen::MatrixXd bestSensors;
    bestSensors.resize(numOptimalSensors + numProjectedSensors, 3);

    // Grab the top n sensor r,c locations and values.
    selectTopSpots(&gGrid, &bestSensors, &userSensorList,
                   numOptimalSensors + numProjectedSensors,
                   sensorRange, suppressionRangeFactor,
                   peak, sd, acousticParams["timestamp"]);

    std::cout << bestSensors << "\n";

    // Generate graphs
    std::cout<< "\nWriting Graphs...";
    Graph gGraph = Graph(&gGrid);
    Graph tGraph = Graph(&tGrid);
    Graph bGraph = Graph(&bGrid);
    try {
        // Print the matrix & data files for Topography Grid
        tGraph.writeMat();
        tGraph.writeDat();
        // Print the contour file used by all graphs.
        //     (Do this just once as it takes a loooong time).
        // tGraph.printContour(contourPtr);
        // Graph the Topography Grid with contour lines.
        bool plotSensors = true;
        tGraph.printContourGraph(width, height, &contourLevels, plotSensors,
                                 &userSensorList, &bestSensors,
                                 numProjectedSensors, false);

        // Print the matrix & data files for Bathymetry Grid
        bGraph.writeMat();
        bGraph.writeDat();
        // Graph Behavior Grid with contour lines.
        bGraph.printContourGraph(width, height, &contourLevels, plotSensors,
                                 &userSensorList, &bestSensors,
                                 numProjectedSensors, false);

        // Print the matrix & data files for Goodness Grid
        gGraph.writeMat();
        gGraph.writeDat();
        // Graph Goodness Grid with contour lines.
        gGraph.printContourGraph(width, height, &contourLevels, plotSensors,
                                 &userSensorList, &bestSensors,
                                 numProjectedSensors, false);
    } catch (int e) {
        std::cout << "Error:" << e << "\n";
        return 0;
    }

    end = clock();
    vizDelta = static_cast<double>(vizEnd - vizBegin) / CLOCKS_PER_SEC;
    std::cout << "\nVisibility calculation took " << vizDelta << " s";
    timeSpent = static_cast<double>(end - begin) / CLOCKS_PER_SEC;
    std::cout << "\nEntire Run took " << timeSpent << " s";

    return 0;
}
