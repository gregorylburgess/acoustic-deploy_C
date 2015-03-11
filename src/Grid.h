/** Copyright 2015 Greg Burgess. **/
#ifndef SRC_GRID_H_
#define SRC_GRID_H_
#include <Dense>
#include <string>

namespace std {
/**
 * Represents a matrix of data.
 */
class Grid {
 public:
    int rows, cols;
    Eigen::MatrixXd data;
    string name;

    Grid();
    Grid(int Rows, int Cols, string name);
    Grid(Grid* mat, string name);
    Grid(Eigen::MatrixXd dat, string newName);
    void clearNA();
    void printData();
    void replace(double find, double replace);
    void setAll(double val);
    double nanCheck(double x);
};
}  // namespace std
#endif  // SRC_GRID_H_
