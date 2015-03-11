/** Copyright 2015 Greg Burgess. **/
#ifndef SRC_GLOBALVARS_H_
#define SRC_GLOBALVARS_H_
#pragma once
#include <string>
#include <unordered_map>
#define EIGEN_DEFAULT_TO_ROW_MAJOR true
namespace std {
extern unordered_map <string, string> acousticParams;
extern int border;
}  // namespace std
#endif  // SRC_GLOBALVARS_H_
