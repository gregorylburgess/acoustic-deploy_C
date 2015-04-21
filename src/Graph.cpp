//============================================================================
// Name        : Graph.cpp
// Author      : Greg Burgess
// Version     : 1.0
// Copyright   : Copyright 2014
// Description : A class definition for a Graph object.
//============================================================================
#include <stdlib.h>
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include "Grid.h"
#include "Utility.h"
#include "Graph.h"
#include "GlobalVars.h"
#include "gnuplot_i.hpp"

/** Graph Constructor.  Creates a Graph object from a Grid object.
 *  @param Grid* g A pointer to a Grid object.
 */
Graph::Graph(Grid* g) {
    contour = false;
    grid = g;
    contourDataFile = "data/contour.dat";
    inputDatFile = "data/" + grid->name + ".dat";
    inputMatFile = "data/" + grid->name + ".mat";
}

/**
 * Creates a data file containing contour lines, which can be overlaid onto
 * another graph.
 * Calling this BEFORE calling printContourGraph() makes a Contour graph,
 * otherwise, no contour lines are drawn.
 * @param contourLevels A pointer to an array of doubles holding contour
 *          levels (depth as a negative integer).
 */
void Graph::printContour(std::vector <std::string> *contourLevels) {
    std::cout << "\n\nWriting contour file...\n";
    Gnuplot plots;
    std::stringstream ss;
    std::string setOutput  = "set table \"" + contourDataFile + "\"",
           setContour = "set contour surface",
           cntrparam,
           noSurface = "unset surface",
           pm3d = "set pm3d map",
           plot;
    int i = 0, numLevels = stoi(acousticParams["numContourDepths"]);
    // Print to a table instead of a gif
    plots.cmd(setOutput);
    // Enable contour lines
    plots.cmd(setContour);
    // Build a comma separated list of contour levels
    ss << "set cntrparam levels discrete ";
    for (i = 0; i < numLevels-1; i++) {
        ss << (*contourLevels)[i] << ",";
    }
    ss << (*contourLevels)[numLevels-1];
    cntrparam = ss.str();
    plots.cmd(cntrparam);
    ss.str("");
    ss.clear();
    // Don't plot the map, just the contour lines
    plots.cmd(noSurface);
    // pm3d is required for contour lines
    // plots.cmd(pm3d);
    // Plot the lines (this takes ~15s per contour level)
    ss << "splot \"" << inputDatFile << "\"";
    plot = ss.str();
    if (acousticParams["debug"] ==  "1") {
        std::cout << setOutput << "\n";
        std::cout << setContour << "\n";
        std::cout << cntrparam << "\n";
        std::cout << noSurface << "\n";
        std::cout << pm3d << "\n";
        std::cout << plot << "\n";
    }
    plots.cmd(plot);
}


/**
 * Prints a graph of a given file with contour data as a .png file.
 * If a contour file was written [by calling printContour()], it prints
 * a contour graph. Otherwise, no contour lines are drawn.
 * @param int width The width of the resulting image.
 * @param int height The height of the resulting image.
 * @param double *contourLevels A pointer to an array of integers holding
 *                contour levels (depth as a negative integer).
 * @param bool logScaleGraphColoring If true, changes graph coloring to log
 *                scale, otherwise a linear scale is used.
 */
void  Graph::printContourGraph(int width, int height,
                                std::vector <std::string> *contourLevels,
                                bool logScaleGraphColoring) {
    std::cout << "\n\nPrinting " << grid->name << " graph...\n";
    int i = 0, numOfLevels = stoi(acousticParams["numContourDepths"]);
    double  xstart = -.5,
            ystart = -.5;
    std::stringstream ss;
    std::string  filetype = ".gif",
            outfile = "img/" + grid->name + filetype,
            setOuput = "set output \'" + outfile + "\';",
            // setScale = "set logscale cb;",
            setScale = "set autoscale y",
            setCBRange = "set cbrange[" + std::to_string(
                            grid->data.minCoeff()) + ":" +
                            std::to_string(grid->data.maxCoeff()) + "];",
            sensorLabelColor = "white",
            setLegendColor = "set key textcolor rgb \"#00aa00\"",
            setLegendFont = "set key font \",12\"",
            sensorIconColor = "blue",
            xrange,
            yrange,
            size = "set terminal gif size " + std::to_string(width) + "," +
                     std::to_string(height) + ";",
            plotData,
            pallete;

    if (acousticParams["debug"] ==  "1") {
        std::cout << "Output File: " << outfile << "\n";
        std::cout << "Data File: " << inputDatFile << "\n";
        std::cout << "Matrix File: " << inputMatFile << "\n";
        std::cout << setScale << "\n";
        std::cout << setCBRange << "\n";
    }

    if (!fexists(inputDatFile)) {
        std::cout << "Input .dat File not Found!\n";
        throw 1;
    }

    if (!fexists(inputMatFile)) {
        std::cout << "Input .mat File not Found!\n";
        throw 1;
    }
    // Set y-axis scale type & min/max values
    ss << setScale << "\n";
    // ss << setCBRange;

    // Set x/y range values
    ss << "set yrange [" << ystart << ":" << grid->rows - 2*border - 0.5  <<
            "];";
    yrange = ss.str();
    ss.str("");
    ss.clear();

    ss << "set xrange [" << xstart << ":" << grid->cols - 2*border - 0.5  <<
            "];";
    xrange = ss.str();
    ss.str("");
    ss.clear();
    // TODO(Greg)
    /*
    // Define labels for sensors
    for () {
    	xloc = ;
    	yloc = ;
    	ss << "set label \"" + i + "\" at " << xloc << "," << yloc << " front
    	                     center tc rgb \"" << sensorLabelColor << "\"\n";
    }
    */

    // plot command
    ss << "plot \"" << inputMatFile << "\" matrix with image,";

    // add contour lines
    for (i = 0; i  <  numOfLevels; i ++) {
        ss << " \"" << contourDataFile << "\" index " << i  <<
                " with line title \""  <<
                (*contourLevels)[numOfLevels - i -1]  << "m\" ls " << i + 1  <<
                ",";
    }
    // TODO(Greg)
    /*
    // add sensor icons
     ss << "\'-\' using 1:2:3 with circles lc rgb \"" << sensorIconColor  <<
           "\" fs solid title \"Sensors\"";

     // Specify each circle location as a triplet
    for () {
      	  //  circle format is x-loc y-loc radius
    	 ss << ""
     }
     */

    // finalize string
    plotData = ss.str();
    ss.str("");
    ss.clear();
        try {
            Gnuplot plots;
            plots.cmd(setOuput);
            plots.cmd(size);
            plots.cmd("set term gif small;");
            plots.cmd("set style line 1 lt 1 lw 1;");
            plots.cmd(setLegendColor);
            plots.cmd(setLegendFont);
            if (logScaleGraphColoring) {
                plots.cmd("set log cb;");
            }
            plots.cmd(xrange);
            plots.cmd(yrange);
            // plots.cmd(size);
            if (acousticParams["debug"] ==  "1") {
                std::cout << setOuput << "\n";
                std::cout << "set terminal gif;\n";
                std::cout << xrange << "\n";
                std::cout << yrange << "\n";
                std::cout << size << "\n";
                std::cout << pallete << "\n";
                std::cout << plotData << "\n";
            }
            plots.cmd(plotData);
        }
        catch (GnuplotException &ge) {
            std::cout << ge.what() << std::endl;
        }
}

/**
 * Writes data in this object's grid to a text file as an ascii matrix (.mat)
 * separated by whitespace.
 */
void Graph::writeMat() {
    std::ofstream out;
    int rows = grid->rows - 2 * border,
        cols = grid->cols - 2 * border;
    out.open(("data/" + grid->name + ".mat").c_str());
    out << grid->data.block(border, border, rows, cols);
    out.close();
}

/**
 * Writes data in this object's grid to a data file (.dat), as a list of x,y,z
 * points delimited by newlines.
 */
void Graph::writeDat() {
    std::ofstream out;
    int i = 0;
    int j = 0;
    int rows = grid->rows - 2 * border;
    int cols = grid->cols - 2 * border;
    double val  = 9;
    Eigen::MatrixXd temp;
    temp.resize(rows, cols);
    temp = grid->data;
    out.open(("data/" + grid->name + ".dat").c_str());
    for (i = border; i < rows; i++) {
        for (j = border; j < cols; j++) {
            val = temp(i, j);
            // Yes this is backwards.  No it's not an error.  THANKS GPUPlot...
            out << std::setprecision(3) << j-border << " " << i-border << " " <<
                    val << "\r\n";
        }
        out << "\r\n";
    }

    out.close();
}
