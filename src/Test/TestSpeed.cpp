//============================================================================
// Name        : TestSpeed.cpp
// Author      : Greg Burgess
// Version     : 1.0
// Copyright   : Copyright 2014
// Description : Functions for testing parallel speed.
//============================================================================
#include <time.h>
#include "TestSpeed.h"

void testSpeed(int numThreads) {
    int rowDist = 1000,
        colDist = 1000,
        bias = 2,
        cellSize = 5,
        ousdx = .1,
        ousdy = .1,
        oucor = 0,
        mux = .5,
        muy = .5,
        fishmodel = 1;
    double parallelTime = 0,
           linearTime = 0;
    clock_t begin, end;

    Grid sGrid(rowDist + 2 * border, colDist + 2 * border, "Solution");
    Grid tGrid(rowDist + 2 * border, colDist + 2 * border, "Topography");
    Grid bGrid(rowDist + 2 * border, colDist + 2 * border, "Behavior");
    simulatetopographyGrid(&tGrid, rowDist, colDist);
    populateBehaviorGrid(&tGrid, &bGrid, bias, cellSize, ousdx, ousdy, oucor,
                        mux, muy, fishmodel);

    // Test Linear Speed
    begin = clock();
    testLinearSpeed(sGrid.getDataPointer(), tGrid.getDataPointer(),
                    bGrid.getDataPointer());
    end = clock();

    linearTime = static_cast<double>(end - begin) / CLOCKS_PER_SEC;
    std::cout << "\nLinear Execution took " << linearTime << " s";

    // Test Parallel Speed
    begin = clock();
    testParallelSpeed(sGrid.getDataPointer(), tGrid.getDataPointer(),
                      bGrid.getDataPointer(), numThreads);
    end = clock();

    parallelTime = static_cast<double>(end - begin) / CLOCKS_PER_SEC;
    std::cout << "\nLinear Execution took " << parallelTime << " s";
}

void testLinearSpeed(Eigen::MatrixXd* sol, Eigen::MatrixXd* g1,
                     Eigen::MatrixXd* g2) {
   sol = g1->cwiseProduct(*g2);
}

void testParallelSpeed(Eigen::MatrixXd* sol, Eigen::MatrixXd* g1,
                       Eigen::MatrixXd* g2, int numThreads) {

}
