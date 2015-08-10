#include "Test.h"
#include "TestGoodness.h"
#include "TestSpeed.h"

bool runTests(){
	return runGoodnessTests();
}

double speedTest(int numThreads) {
    return testSpeed(numThreads);
}
