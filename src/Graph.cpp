//============================================================================
// Name        : Graph.cpp
// Author      : Greg Burgess
// Version     : 1.0
// Copyright   : Copyright 2014
// Description : A class definition for a Graph object.
//============================================================================
#include <stdlib.h>
#include <Dense>
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
void Graph::printContourFile(std::vector <std::string> *contourLevels) {
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
 * @param width The width of the resulting image.
 * @param height The height of the resulting image.
 * @param *contourLevels A pointer to an array of integers holding
 *                contour levels (depth as a negative integer).
 * @param logScaleGraphColoring If true, changes graph coloring to log
 *                scale, otherwise a linear scale is used.
 */
void  Graph::printContourGraph(int width, int height,
                                std::vector <std::string> *contourLevels,
                                bool plotSensorIcons,
                                Eigen::MatrixXd* userSensors,
                                Eigen::MatrixXd* optimalSensors,
                                int numProjectedSensors,
                                bool logScaleGraphColoring) {
    std::cout << "\n\nPrinting " << grid->name << " graph...\n";
    int i = 0,
        sensorIconRadius = 2,
        numOptimalSenors = optimalSensors->rows() - numProjectedSensors;
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
            optimalSensorIconColor = "blue",
            userSensorIconColor = "purple",
            projectedSensorIconColor = "pink",
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
    ss << "set yrange [" << ystart << ":" << grid->rows - 2 * border - 0.5  <<
            "];";
    yrange = ss.str();
    ss.str("");
    ss.clear();

    ss << "set xrange [" << xstart << ":" << grid->cols - 2 * border - 0.5  <<
            "];";
    xrange = ss.str();
    ss.str("");
    ss.clear();

    // Plot sensor icons and titles
    if (plotSensorIcons) {
        int numOfLevels = std::stoi(acousticParams["numContourDepths"]);
        Eigen::MatrixXd optimal = optimalSensors->block(0, 0,
                                                  numOptimalSenors, 3);
        Eigen::MatrixXd projected = optimalSensors->block(numOptimalSenors,
                                                     0, numProjectedSensors, 3);
        // Define labels for sensors
        addLabel(&ss, userSensors, sensorLabelColor);
        addLabel(&ss, &optimal, sensorLabelColor);
        addLabel(&ss, &projected, sensorLabelColor);

        // plot command
        ss << "plot \"" << inputMatFile << "\" matrix with image,\\";

        // Add contour lines
        for (i = 0; i  <  numOfLevels; i ++) {
            ss << "\n\"" << contourDataFile.c_str() << "\" index " << i <<
                    " with line title \""  <<
                    (*contourLevels)[numOfLevels - i -1]  << "m\" ls " <<
                    i + 1  << ",\\";
        }
        // Add data ranges
        ss << "\n";
        addDataRange(&ss, userSensorIconColor, false, "User Sensor");
        ss << ",\\\n";
        addDataRange(&ss, optimalSensorIconColor, false, "Optimal Sensor");
        ss << ",\\\n";
        addDataRange(&ss, projectedSensorIconColor, false, "Projected Sensor");
        ss << "\n";
        // Enumerate points
        if (userSensors->rows() > 0) {
            enumeratePoints(&ss, userSensors, sensorIconRadius);
            ss << "EOF\n";
        }
        if (optimal.rows() > 0) {
            enumeratePoints(&ss, &optimal, sensorIconRadius);
            ss << "EOF\n";
        }
        if (projected.rows() > 0) {
            enumeratePoints(&ss, &projected, sensorIconRadius);
            ss << "EOF";
        }
     }

    // finalize string
    if (debug) {
        std::cout << ss.str();
    }

    plotData = ss.str();
    std::cout << plotData;
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
 * Appends label definitions for each entry in sensors to a stringstream.
 * @param ss A pointer to the stringstream object to append to.
 * @param sensors A pointer to an Eigen matrix where each row represents a
 *                sensor, the first column holds the sensor's row location, the
 *                second column holds the sensor's col location.
 * @param sensorLabelColor A string holding a color to paint the label text.
 *                         See the GNUPlot manual for more info.
 */
void Graph::addLabel(std::stringstream* ss,  Eigen::MatrixXd* sensors,
                     std::string sensorLabelColor) {
    int i = 0;
    // Define labels for sensors
    for (i = 0; i < sensors->rows(); i ++) {
        *ss << "set label \"" << i << "\" at " << (*sensors)(i, 1) << "," <<
               (*sensors)(i, 0)<< " front center tc rgb \"" <<
               sensorLabelColor << "\"\n";
    }
}

/**
 * Appends data range definitions for each entry in sensors to a stringstream.
 * @param ss A pointer to the stringstream object to append to.
 * @param sensorIconColor A string holding the color to paint the sensor icons
 *                        in this data range.  See the GNUPlot manual for more
 *                        info.
 * @param keyLabel A string holding the name of the data range.
 *
 */
void Graph::addDataRange(std::stringstream* ss, std::string sensorIconColor, bool solidFill,
                         std::string keyLabel) {
    std::string solid = "\"";
    if (solidFill) {
        solid = "\" fs solid";
    }
    /**ss << " \'-\' using 1:2:3 with circles lc rgb \"" << sensorIconColor  <<
          "\" fs solid title \"" << keyLabel << "\"";
    */
    *ss << " \'-\' using 1:2:3 with circles lc rgb \"" << sensorIconColor  <<
              solid << " title \"" << keyLabel << "\"";
}

/**
 * Appends data point definitions for each entry in sensors to a stringstream.
 * @param ss A pointer to the stringstream object to append to.
 * @param sensors A pointer to an Eigen matrix where each row represents a
 *                sensor, the first column holds the sensor's row location, the
 *                second column holds the sensor's col location.
 * @param iconRadius An integer holding the radius of the sensor icon.
 */
void Graph::enumeratePoints(std::stringstream* ss, Eigen::MatrixXd* sensors,
                            int iconRadius) {
    int i = 0;
    // Specify each circle location as a triplet (row, col, radius)
    for (i = 0; i  <  sensors->rows(); i ++) {
          //  circle format is x-loc y-loc radius; so transpose points.
         *ss << " " << (*sensors)(i, 1) << " " << (*sensors)(i, 0) << " " <<
                       iconRadius << "\n";
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
    int i = 0, j = 0,
            dblBorder = 2 * border,
            rows = grid->rows - dblBorder,
            cols = grid->cols - dblBorder;
    double val  = 0;
    Eigen::MatrixXd temp;
    temp.resize(rows, cols);
    temp = grid->data;
    out.open(("data/" + grid->name + ".dat").c_str());
    for (i = border; i < rows; i++) {
        for (j = border; j < cols; j++) {
            val = temp(i, j) - dblBorder;
            // Yes this is backwards.  No it's not an error.  THANKS GPUPlot...
            out << std::setprecision(3) << j-border << " " << i-border <<
                    " " << val << "\r\n";
        }
        out << "\r\n";
    }

    out.close();
}
