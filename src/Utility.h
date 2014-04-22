#pragma once
void printError(std::string msg, int errcode, long timestamp);
void printGraph(std::string title, int XDist, int YDist, std::string dataFilePath,int width, int height);
double* seq(double start, double end, int size);
bool fexists(std::string filename);
