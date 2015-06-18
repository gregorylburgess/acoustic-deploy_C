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
    // acousticParams.insert({"inputFile", "himbsyn.bathy.v19.grd"});
    acousticParams.insert({"inputFile", "himbsyn.bathytopo.1km.v19.grd"});
    // acousticParams.insert({ "inputFile", "pal_5m.asc" });
    acousticParams.insert({ "inputFileType", "netcdf" });
    acousticParams.insert({ "seriesName", "z" });
    acousticParams.insert({ "timestamp", "-1" });
    acousticParams.insert({ "logScaleGraphColoring", "1" });
    acousticParams.insert({ "contourDepths", "0,-20,-40,-80" });
    int    startRow = 450,
           startCol = 340,  // 450,340,200,200 (1km)netcdf
           rowDist = 201,   // 100 0 800 1500 (5m)netcdf
           colDist = 201,   // 300 200 501 501 (palmyra) asc
           height = 1000,
           width = 1000,
           bias = 3,
           sensorDetectionRange = 4,
           sensorDetectionDiameter = 2 * sensorDetectionRange + 1,
           // sensorRangeSD = 1,
           sensorPeakDetectionProbability = 1,
           SDofSensorDetectionRange = 1,
           i = 0,
           row = 0,
           col = 0,
           cellSize = std::stoi(acousticParams["cellSize"]),
           numOptimalSensors =
                   std::stoi(acousticParams["numOptimalSensors"]),
           numProjectedSensors =
                   std::stoi(acousticParams["numProjectedSensors"]);
           border = sensorDetectionRange;
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
    tGrid.data.setConstant(0);
    Eigen::MatrixXd suppressionReference;

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

    // Initalize the Coverage Grid
    cGrid.data.block(border, border, rowDist, colDist).setConstant(1);

    // Mr. Gaeta, START THE CLOCK!
    vizBegin = clock();
    std::cout << "\nGetting Goodness...\n";

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

    // Calculate good sensor locations
    calculateGoodnessGrid(&tGrid, &bGrid, &gGrid, &detectionGradient,
                        &distanceGradient, &suppressionReference, bias,
                        sensorDetectionRange, border, border,
                        rowDist, colDist,
                        sensorPeakDetectionProbability,
                        SDofSensorDetectionRange);

    // Check if we should proceed...
    if (gGrid.data.sum() <= 0) {
        printError(
               "No positive coefficients found in the goodness Grid. Aborting",
               0, acousticParams["timestamp"]);
    }
    vizEnd = clock();
    goodnessGridComputationTime =
            static_cast<double>(end - begin) / CLOCKS_PER_SEC;
    std::cout << "Copying goodness grid...\n";
    Grid UGGrid(&gGrid, "Unsuppressed Goodness");

    // Find optimal placements
    std::cout << "\nPlacing Sensors...\n";
    Eigen::MatrixXd bestSensors;
    bestSensors.resize(numOptimalSensors + numProjectedSensors, 4);

    // Grab the top n sensor r,c locations and recovery rates.
    selectTopSensorLocations(&tGrid, &bGrid, &gGrid, &UGGrid,
                   &bestSensors, &userSensorList, &suppressionReference,
                   &detectionGradient, &distanceGradient,
                   numOptimalSensors + numProjectedSensors,
                   sensorDetectionRange, bias, suppressionRangeFactor,
                   sensorPeakDetectionProbability, SDofSensorDetectionRange,
                   acousticParams["timestamp"]);

    std::cout << bestSensors << "\n";

    std::cout << "Computing Statistics...\n";
    getStats(&UGGrid, &gGrid, &bestSensors, sensorDetectionRange, &networkSparsity,
              &absRecoveryRate, &uniqueRecoveryRate, &cGrid);

    // Generate graphs
    std::cout<< "\nWriting Graphs...";
    Graph gGraph = Graph(&gGrid);
    Graph tGraph = Graph(&tGrid);
    Graph bGraph = Graph(&bGrid);
    Graph cGraph = Graph(&cGrid);
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
