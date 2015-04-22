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
    acousticParams.insert({ "userSensors", "100,100,200,200,100,200" });
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
    // TODO(Greg) Data validation
    int    startRow = 100,
           startCol = 0,  // 450,340,200,200 (1km)
           RowDist = 300,   // 100 0 800 1500 (5m)
           ColDist = 300,
           height = 1000,
           width = 1000,
           bias = 3,
           sensorRange = 4,
           peak = 1, sd = 1,
           cellSize = std::stoi(acousticParams["cellSize"]),
           numOptimalSensors =
                   std::stoi(acousticParams["numOptimalSensors"]),
           numProjectedSensors =
                   std::stoi(acousticParams["numProjectedSensors"]),
           i = 0;

    double ousdx     = std::stod(acousticParams["ousdx"]),
           ousdy     = std::stod(acousticParams["ousdy"]),
           oucor     = std::stod(acousticParams["oucor"]),
           mux       = std::stod(acousticParams["mux"]),
           muy       = std::stod(acousticParams["muy"]),
           fishmodel = std::stod(acousticParams["fishmodel"]);

    border = sensorRange;
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

    Grid bGrid(RowDist + 2 * border, ColDist + 2 * border, "Behavior");
    Grid gGrid(RowDist + 2 * border, ColDist + 2 * border, "Goodness");
    Grid tGrid(RowDist + 2 * border, ColDist + 2 * border, "Topography");
    tGrid.data.setConstant(0);
    // Fetch or simulate topography
    std::cout << "Getting topography...";
    if (simulateBathy) {
        // Simulate topography
        simulatetopographyGrid(&tGrid, RowDist, ColDist);
    } else {
        // Fetch topography
        getBathy(&tGrid, acousticParams["inputFile"],
                 acousticParams["inputFileType"], size_t(startRow),
                 size_t(startCol), size_t(RowDist), size_t(ColDist),
                 acousticParams["seriesName"], acousticParams["timestamp"]);
    }

    std::cout << "\nGetting Behavior...";
    // Fill in Behavior Grid
    populateBehaviorGrid(&tGrid, &bGrid, cellSize, ousdx, ousdy, oucor, mux,
                         muy, fishmodel);
    vizBegin = clock();
    std::cout << "\nGetting Goodness...\n";
    // Calculate good sensor locations
    calculateGoodnessGrid(&tGrid, &bGrid, &gGrid, bias, sensorRange, peak, sd);
    vizEnd = clock();
    vizDelta = static_cast<double>(end - begin) / CLOCKS_PER_SEC;

    Graph gGraph = Graph(&gGrid);
    Graph tGraph = Graph(&tGrid);
    Graph bGraph = Graph(&bGrid);

    std::cout << "\nReading userSensor List...\n";
    // Parse User Sensor Placements
    std::vector<std::string> userSensors;
    parseCDString(&userSensors, acousticParams["userSensors"], ',');

    std::cout << "\nPlacing Sensors...";
    // Grab the top n sensor r,c locations and values.
    Eigen::MatrixXd bestSensors;
    Eigen::MatrixXd userSensorList;
    bestSensors.resize(numOptimalSensors + numProjectedSensors, 3);
    userSensorList.resize(userSensors.size()/2, 2);
    for (i = 0; i < userSensorList.rows(); i ++) {
        userSensorList(i, 0) = std::stoi(userSensors[2 * i]);
        userSensorList(i, 1) = std::stoi(userSensors[2 * i + 1]);
    }
    selectTopSpots(&gGrid, &bestSensors, &userSensorList,
                   numOptimalSensors + numProjectedSensors,
                   sensorRange, peak, sd);

    std::cout << bestSensors << "\n";
    std::cout<< "\nWriting Graphs...";
    // Generate graphs
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
