/** Copyright 2015 Greg Burgess. **/
#ifndef SRC_UTILITY_H_
#define SRC_UTILITY_H_
#include <set>
#include <string>
#include <utility>
namespace std {
void printError(string msg, int errcode, string timestamp);
void printGraph(string title, int XDist, int YDist,
                string dataFilePath, int width, int height);
double* seq(double start, double end, int size);
bool fexists(string filename);
void printSet(set<pair<int, int> > *pairs);
}  // namespace std
#endif  // SRC_UTILITY_H_
