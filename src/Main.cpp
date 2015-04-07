//============================================================================
// Name        : Main.cpp
// Author      : Greg Burgess
// Version     : 1.0
// Copyright   : Copyright 2014
// Description : Runs the program.
//============================================================================
#include <stdlib.h>
#include <time.h>
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
    bool test = true, simulateBathy = false;

    if (test) {
        runTests();
        std::cout << "Done";
        return 0;
    }

    acousticParams.insert({ "debug", "0" });

    acousticParams.insert({ "cellSize", "5" });
    acousticParams.insert({ "fishmodel", "0" });
    acousticParams.insert({ "sensorRange", "50" });
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
    int startRow = 100,
        startCol = 0,  // 450,340,200,200 (1km)
        RowDist = 800, ColDist = 1500, height = 800, width = 1500, i = 0,
        bias = 3, sensorRange = 4, peak = 1, sd = 1,
        cellSize = std::stoi(acousticParams["cellSize"]);
    double ousdx = std::stod(acousticParams["ousdx"]),
           ousdy = std::stod(acousticParams["ousdy"]),
           oucor = std::stod(acousticParams["oucor"]),
           mux = std::stod(acousticParams["mux"]),
           muy   = std::stod(acousticParams["muy"]);
    double fishmodel = std::stod(acousticParams["fishmodel"]);
    std::string token;
    border = sensorRange;
    clock_t begin, end, vizBegin, vizEnd;
    double vizDelta, timeSpent;
    begin = clock();
    // Compute contour depth meta data (used for graphical output)
    std::istringstream contourstring(acousticParams["contourDepths"]);
    size_t numContourDepths = count(acousticParams["contourDepths"].begin(),
                                    acousticParams["contourDepths"].end(), ',')
                                    + 1;

    acousticParams.insert({ "numContourDepths",
                            std::to_string(numContourDepths) });
    std::vector<double> contourLevels(numContourDepths);
    i = 0;
    while (getline(contourstring, token, ',')) {
        contourLevels.push_back(std::stod(token));
        i++;
    }
    sort(&contourLevels, &contourLevels + numContourDepths);

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
    std::cout << "Geting topography";
    if (simulateBathy) {
        simulatetopographyGrid(&tGrid, RowDist, ColDist);
    } else {
        getBathy(&tGrid, acousticParams["inputFile"],
                 acousticParams["inputFileType"], size_t(startRow),
                 size_t(startCol), size_t(RowDist), size_t(ColDist),
                 acousticParams["seriesName"], acousticParams["timestamp"]);
    }

    std::cout << "\nGetting Behavior";
    // Fill in Behavior Grid
    populateBehaviorGrid(&tGrid, &bGrid, cellSize, ousdx, ousdy, oucor, mux,
                         muy, fishmodel);
    vizBegin = clock();
    std::cout << "\nGetting Goodness";
    // Calculate good sensor locations
    calculateGoodnessGrid(&tGrid, &bGrid, &gGrid, bias, sensorRange, peak, sd);
    vizEnd = clock();
    vizDelta = static_cast<double>(end - begin) / CLOCKS_PER_SEC;

    Graph gGraph = Graph(&gGrid);
    Graph tGraph = Graph(&tGrid);
    Graph bGraph = Graph(&bGrid);

    // A pointer to the array with the Contour depths
    std::vector<double> *contourPtr = &contourLevels;
    // Generate graphs
    try {
        // Print the matrix & data files for Topography Grid
        tGraph.writeMat();
        tGraph.writeDat();
        // Print the contour file used by all graphs.
        //     (Do this just once as it takes a loooong time).
        // tGraph.printContour(contourPtr);
        // Graph the Topography Grid with contour lines.
        tGraph.printContourGraph(width, height, contourPtr, false);

        // Print the matrix & data files for Bathymetry Grid
        bGraph.writeMat();
        bGraph.writeDat();
        // Graph Behavior Grid with contour lines.
        bGraph.printContourGraph(width, height, contourPtr, false);

        // Print the matrix & data files for Goodness Grid
        gGraph.writeMat();
        gGraph.writeDat();
        // Graph Goodness Grid with contour lines.
        gGraph.printContourGraph(width, height, contourPtr, false);
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
