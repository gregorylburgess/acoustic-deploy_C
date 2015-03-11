/** Copyright 2015 Greg Burgess. **/
#include <math.h>
#include <string.h>
#include <netcdf.h>
#include <stdlib.h>
#include <Core>
#include <Dense>
#include <fstream>
#include <cmath>
#include <iostream>
#include <string>
#include <vector>
#include "Grid.h"
#include "Utility.h"
#include "GlobalVars.h"
#define _USE_MATH_DEFINES

namespace std {
double zero(double x) {
    if (x > 0) {
        return 1;
    }
    return x;
}

void splitVec(string *s, char delim,
                            vector<string> *elems) {
    stringstream ss(*s);
    string item;
    while (getline(ss, item, delim)) {
        elems->push_back(item);
    }
}


vector<string> split(string *s, char delim) {
    vector<string> elems;
    splitVec(s, delim, &elems);
    return elems;
}

void simulatetopographyGrid(Grid* topographyGrid, int numRows, int numCols) {
    Eigen::VectorXd refx = refx.LinSpaced(numCols, -2*M_PI, 2*M_PI);
    Eigen::VectorXd refy = refx.LinSpaced(numRows, -2*M_PI, 2*M_PI);
    Eigen::MatrixXd X = refx.replicate(1, numRows);
    X.transposeInPlace();
    Eigen::MatrixXd Y = refy.replicate(1, numCols);

    // Eigen can only deal with two matrices at a time,
    // so split the computation:
    // topographyGrid = sin(X) * sin(Y) * abs(X) * abs(Y) -pi
    Eigen::MatrixXd absXY = X.cwiseAbs().cwiseProduct(Y.cwiseAbs());
    Eigen::MatrixXd sins = X.array().sin().cwiseProduct(Y.array().sin());
    Eigen::MatrixXd temp;
    temp.resize(numRows, numCols);
    temp = absXY.cwiseProduct(sins);

    // All this work to create a matrix of pi...
    Eigen::MatrixXd pi;
    pi.resize(numRows, numCols);
    pi.setConstant(M_PI);
    temp = temp - pi;
    // And the final result.
    topographyGrid->data.block(border, border, numRows, numCols) =
                              temp.block(0, 0, numRows, numCols);
    // Ignore positive values.
    topographyGrid->data = topographyGrid->data.unaryExpr(ptr_fun(zero));
    topographyGrid->clearNA();
}


void getBathy(Grid* topographyGrid, string inputFile, string inputFileType,
              size_t startRow, size_t startCol, size_t numRows, size_t numCols,
              string seriesName, string timestamp) {
    // This will be the netCDF ID for the file and data variable.
    Eigen::MatrixXd temp;
    int ncid, varid, retval = -100;

    // NetCDF
    if (inputFileType.compare("netcdf") == 0) {
        // Data will be read in column major, so set up a matrix of inverse
        //    size to recieve it.
        temp.resize(numCols, numRows);
        // Open the file. NC_NOWRITE tells netCDF we want read-only access to
        //    the file.
        if ((retval = nc_open(inputFile.c_str(), NC_NOWRITE, &ncid))) {
            printError("ERROR: Cant open NetCDF File. Invalid inputFile path.",
                        retval, timestamp);
        }

        // Get the varid of the data variable, based on its name.
        if ((retval = nc_inq_varid(ncid, seriesName.c_str(), &varid))) {
            printError("ERROR: Can't access variable id.  Invalid seriesName.",
                        retval, timestamp);
        }
        // Read the data.
        try {
            // for whatever reason, this is in column, row order.
            static size_t start[] = {startRow, startCol};
            static size_t range[] = {numRows, numCols};
            retval = nc_get_vara_double(ncid, varid, start, range, temp.data());
            // TODO(Greg) Figure out a way to read data in row wise to avoid
            //             this transposition.
            topographyGrid->data.block(border, border, numRows, numCols) =
                            temp.transpose().block(0, 0, numRows, numCols);
        }
        catch (int i) {
            printError("ERROR: Error reading data.  Invalid file format.",
                        retval, timestamp);
        }
        // Close the file, freeing all resources.
        if ((retval = nc_close(ncid))) {
            printError("ERROR: Error closing the file.", retval, timestamp);
        }
    } else if (inputFileType.compare("asc") == 0) {
        // ASC
        temp.resize(numRows, numCols);
        ifstream input(inputFile);
        int null = 0;
        size_t numHeaderLines = 5,
               rowsLine = 1,
               colsLine = 0,
               nullLine = 5,
               cursor = 0,
               rows = 0,
               cols = 0,
               startRowIndex = startRow+numHeaderLines,
               endRowIndex = startRowIndex+numRows,
               i = 0,
               j = 0;
        string line = "";
        vector<string> vLine;
        if (input.is_open()) {
            for (cursor = 0; cursor < endRowIndex; cursor ++) {
                getline(input, line);

                if (cursor <= numHeaderLines) {
                    // Get the number of columns in the file
                    if (cursor == colsLine) {
                        vLine = split(&line, ' ');
                        cout << "\ncolsLine=" << vLine[vLine.size()-1];
                        cols = stoi(vLine[vLine.size() - 1]);

                        if (cols < startCol + numCols) {
                            cout << "\nERROR, requested bathymetry grid" <<
                                    "column coordinates are out of bounds.\n";
                            exit(1);
                        }
                    } else if (cursor == rowsLine) {
                        // Get the number of rows in the file.
                        vLine = split(&line, ' ');
                        cout << "rowsLine=" << vLine[vLine.size() - 1];
                        rows = stoi(vLine[vLine.size() - 1]);

                        if (rows < startRow + numRows) {
                            cout << "\nERROR, requested bathymetry grid row" <<
                                    " coordinates are out of bounds.\n";
                            exit(1);
                        }
                    } else if (cursor == nullLine) {
                        // Get the null value substitute
                        vLine = split(&line, ' ');
                        cout << "nullLine=" << vLine[vLine.size() - 1];
                        null = stoi(vLine[vLine.size() - 1]);
                    }
                } else if (cursor >= startRowIndex) {
                    vLine = split(&line, ' ');
                    for (i = startCol; i < startCol + numCols; i ++) {
                        // cout<<"accessing temp("<<cursor-startRowIndex-1<<","
                        // <<i-startCol<<")\n";
                        // cout<<"Cursor:"<<cursor<<"   SRI:"<<startRowIndex<<
                        // "\n";
                        temp(cursor - startRowIndex, i - startCol) =
                                stod(vLine[i]);
                    }
                }
            }

            input.close();

            for (i = 0; i < numRows; i ++) {
                for (j = 0; j < numCols; j ++) {
                    if (temp(i, j) == null) {
                        temp(i, j) = 0;
                    }
                }
            }
            topographyGrid->data.block(border, border, numRows, numCols) =
                            temp.block(0, 0, numRows, numCols);
        } else {
            cout << "\nUnable to open bathymetry file.\n";
        }
    } else {
        // Invalid filetype
        cout << "Bathymetry file type not supported.  Simulating Bathymetry.\n";
        simulatetopographyGrid(topographyGrid, static_cast<int>(numRows),
                               static_cast<int>(numCols));
    }
    topographyGrid->clearNA();
    topographyGrid->data = topographyGrid->data.unaryExpr(ptr_fun(zero));
    if (acousticParams["debug"] == "1") {
        // topographyGrid->printData();
        cout << "startx " << startCol << "\nXDist: "<< numCols <<
                  "\nstartY:  "<< startRow << "\nYDist: " << numRows << "\n";
        cout << "inputFileType: " << inputFileType << "\ninputFile: " <<
                  inputFile << "\nseriesName: " << seriesName << "\n";
        cout << "retval: " << retval << "\n" << "ncid: " << ncid << "\n\n";
    }
    cout << "Exiting bathy";
}
}  // namespace std
