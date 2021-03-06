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
#include "Test/Test.h"
#include "Test/TestGoodness.h"

#include "Utility.h"
int main() {
    bool test = false, simulateBathy = false;

    if (test) {
        runTests();
        return 0;
    }

    acousticParams.insert({ "debug", "0" });

    acousticParams.insert({ "cellSize", "5" });
    acousticParams.insert({ "fishmodel", "0" });
    acousticParams.insert({ "suppressionRangeFactor", "1"}),
    acousticParams.insert({ "suppressionMethod", "suppression.quick"}),
    acousticParams.insert({ "userSensors", "100,100,0,0,100,0,0,200" });
    acousticParams.insert({ "numOptimalSensors", "10" });
    acousticParams.insert({ "numProjectedSensors", "2" });
    acousticParams.insert({ "bias", "2" });

    acousticParams.insert({ "ousdx", ".1" });
    acousticParams.insert({ "ousdy", ".1" });
    acousticParams.insert({ "oucor", "0" });
    acousticParams.insert({ "mux", ".5" });
    acousticParams.insert({ "muy", ".5" });
    acousticParams.insert({ "fishmodel", "1" });
    acousticParams.insert({ "minDepth", "15" });
    acousticParams.insert({ "maxDepth", "30" });
    acousticParams.insert({ "meanRelativePosition", "1" });
    acousticParams.insert({ "relativePositionSD", "1" });
    // acousticParams.insert({"inputFile", "himbsyn.bathy.v19.grd"});
    acousticParams.insert({"inputFile", "himbsyn.bathytopo.1km.v19.grd"});
    // acousticParams.insert({ "inputFile", "pal_5m.asc" });
    acousticParams.insert({ "inputFileType", "netcdf" });
    acousticParams.insert({ "seriesName", "z" });
    acousticParams.insert({ "timestamp", "-1" });
    acousticParams.insert({ "logScaleGraphColoring", "1" });
    acousticParams.insert({ "contourDepths", "0,-20,-40,-80" });
    double suppressionRangeFactor =
                       std::stod(acousticParams["suppressionRangeFactor"]),
           ousdx     = std::stod(acousticParams["ousdx"]),
           ousdy     = std::stod(acousticParams["ousdy"]),
           oucor     = std::stod(acousticParams["oucor"]),
           mux       = std::stod(acousticParams["mux"]),
           muy       = std::stod(acousticParams["muy"]),
           fishmodel = std::stod(acousticParams["fishmodel"]),
           networkSparsity = 0, absRecoveryRate = 0, uniqueRecoveryRate = 0,
           goodnessGridComputationTime = 0, totalComputationTime = 0;

    int    startRow = 450,
           startCol = 340,  // 450,340,201,201 (1km)netcdf
           rowDist = 1001,   // 100 0 800 1500 (5m)netcdf
           colDist = 1001,   // 300 200 501 501 (palmyra) asc
           height = 1000,
           width = 1000,
           bias = 2,
           sensorDetectionRange = 4,
           sensorDetectionDiameter = 2 * sensorDetectionRange + 1,
           sensorPeakDetectionProbability = 1,
           SDofSensorDetectionRange = 1,
           i = 0,
           row = 0,
           col = 0,
           cellSize = std::stoi(acousticParams["cellSize"]),
           numOptimalSensors =
                   std::stoi(acousticParams["numOptimalSensors"]),
           numProjectedSensors =
                   std::stoi(acousticParams["numProjectedSensors"]),
           numSensorsToPlace = numOptimalSensors + numProjectedSensors,
           suppressionDiameter = (2 * ceil(sensorDetectionRange * suppressionRangeFactor)) + 1;
    // Set the global border size
    border = sensorDetectionRange;
    omp_set_num_threads(numThreads);
    Eigen::setNbThreads(numThreads);
    clock_t begin, end, vizBegin, vizEnd;




    // TODO(Greg) Data validation
    // Parse User Sensor Placements
    std::cout << "\nReading userSensor List...\n";
    std::vector<std::string> userSensors;
    parseCDString(&userSensors, acousticParams["userSensors"], ',');
    Eigen::MatrixXd userSensorList;
    userSensorList.resize(userSensors.size()/2, 4);
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
           habitatFilePath    = outputDataFilePath + habitatTitle +
                                outputDataFileType,
           goodnessFilePath   = outputDataFilePath + goodnessTitle +
                                outputDataFileType,
           coverageFilePath   = outputDataFilePath + coverageTitle +
                                outputDataFileType;

    Grid bGrid(rowDist + 2 * border, colDist + 2 * border, "Behavior");
    Grid gGrid(rowDist + 2 * border, colDist + 2 * border, "Goodness");
    Grid tGrid(rowDist + 2 * border, colDist + 2 * border, "Topography");
    Grid cGrid(rowDist + 2 * border, colDist + 2 * border, "Coverage");

    Eigen::MatrixXd suppressionReference;
    Eigen::MatrixXd distanceGradient;
    Eigen::MatrixXd detectionGradient;
    distanceGradient.resize(sensorDetectionDiameter, sensorDetectionDiameter);
    distanceGradient.setConstant(0);
    detectionGradient.resize(sensorDetectionDiameter, sensorDetectionDiameter);
    detectionGradient.setConstant(0);
    // Create a gradient of distances to avoid redundant computation
    makeDistGradient(&distanceGradient, sensorDetectionRange);
    // Create a gradient of probability of detection (due to sensorRange) to
    // avoid redundant computation
    makeDetectionGradient(&detectionGradient, & distanceGradient,
                   sensorPeakDetectionProbability, SDofSensorDetectionRange);

    // Fetch or simulate topography
    std::cout << "Getting topography...";
    if (simulateBathy) {
        // Simulate topography
        simulatetopographyGrid(&tGrid, rowDist, colDist);
    } else {
        // Fetch actual topography
        getBathy(&tGrid, acousticParams["inputFile"],
                 acousticParams["inputFileType"], size_t(startRow),
                 size_t(startCol), size_t(rowDist), size_t(colDist),
                 acousticParams["seriesName"], acousticParams["timestamp"]);
    }
    std::cout << bGrid.data;
    // Fill in Behavior Grid
    std::cout << "\nGetting Behavior...";
    populateBehaviorGrid(&tGrid, &bGrid, bias, cellSize, ousdx, ousdy, oucor, mux,
                         muy, fishmodel);

    // Initalize the Coverage Grid
    cGrid.data.block(border, border, rowDist, colDist).setConstant(1);

    // Mr. Gaeta, START THE CLOCK!
    vizBegin = clock();
    std::cout << "\nGetting Goodness...\nBias: " << bias << "\n";
    // Calculate good sensor locations
    calculateGoodnessGrid(&tGrid, &bGrid, &gGrid, &suppressionReference,
                        &detectionGradient, &distanceGradient, bias,
                        sensorDetectionRange, border, border,
                        rowDist, colDist);
    vizEnd = clock();
    goodnessGridComputationTime =
            static_cast<double>(end - begin) / CLOCKS_PER_SEC;
    std::cout << "Copying goodness grid...\n";
    Grid UGGrid(&gGrid, "Unsuppressed Goodness");

    // Find optimal placements
    std::cout << "\nPlacing Sensors...\n";
    Eigen::MatrixXd bestSensors;
    bestSensors.resize(numSensorsToPlace, 4);


    //============================
    gGrid.name = "x1";
    Graph x1Graph = Graph(&gGrid);
    x1Graph.writeMat();

    // Grab the top n sensor r,c locations and recovery rates.
    selectTopSensorLocations(&tGrid, &bGrid, &gGrid, &UGGrid,
                   &bestSensors, &userSensorList, &suppressionReference,
                   &detectionGradient, &distanceGradient,
                   numSensorsToPlace,
                   sensorDetectionRange, bias, suppressionRangeFactor,
                   suppressionDiameter, sensorPeakDetectionProbability,
                   SDofSensorDetectionRange, acousticParams["timestamp"]);

    gGrid.name = "x2";
    Graph x2Graph = Graph(&gGrid);
    x2Graph.writeMat();
    std::cout << bestSensors << "\n";

    std::cout << "Computing Statistics...\n";
    getStats(&UGGrid, &gGrid, &bestSensors, sensorDetectionRange, &networkSparsity,
              &absRecoveryRate, &uniqueRecoveryRate, &cGrid);

    gGrid.name = "x3";
    Graph x3Graph = Graph(&gGrid);
    x3Graph.writeMat();
    gGrid.name = "Goodness";
    // Generate graphs
    std::cout<< "\nWriting Graphs...";
    Graph gGraph = Graph(&UGGrid);
    Graph tGraph = Graph(&tGrid);
    Graph bGraph = Graph(&bGrid);
    Graph cGraph = Graph(&cGrid);
    try {
        // Print the matrix & data files for Topography Grid
        tGraph.writeMat();
        tGraph.writeDat();
        // Print the contour file used by all graphs.
        //     (Do this just once as it takes a loooong time).
        tGraph.printContourFile(&contourLevels);
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

        // Print the matrix & data files for Goodness Grid
        cGraph.writeMat();
        cGraph.writeDat();
        // Graph Goodness Grid with contour lines.
        cGraph.printContourGraph(width, height, &contourLevels, plotSensors,
                                 &userSensorList, &bestSensors,
                                 numProjectedSensors, false);
    } catch (int e) {
        std::cout << "Error:" << e << "\n";
        return 0;
    }
    end = clock();
    goodnessGridComputationTime =
            static_cast<double>(vizEnd - vizBegin) / CLOCKS_PER_SEC;
    std::cout << "\nVisibility calculation took " <<
            goodnessGridComputationTime << " s";
    totalComputationTime = static_cast<double>(end - begin) / CLOCKS_PER_SEC;
    std::cout << "\nEntire Run took " << totalComputationTime << " s";

    return 0;
}
