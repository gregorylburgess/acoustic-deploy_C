//============================================================================
// Name        : TestSpeed.h
// Author      : Greg Burgess
// Version     : 1.0
// Copyright   : Copyright 2014
// Description : Functions for testing parallel speed.
//============================================================================
#ifndef SRC_TESTSPEED_H_
#define SRC_TESTSPEED_H_

void testSpeed(int numThreads);
void testLinearSpeed(Eigen::MatrixXd* sol, Eigen::MatrixXd* g1,Eigen::MatrixXd* g2);
void testParallelSpeed(Eigen::MatrixXd* sol, Eigen::MatrixXd* g1,Eigen::MatrixXd* g2, int numThreads);
