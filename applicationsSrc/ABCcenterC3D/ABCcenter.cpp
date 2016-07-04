#include <iostream>
#include "catoms3DSimulator.h"
#include "catoms3DBlockCode.h"
#include "ABCcenterCode.h"

using namespace std;
using namespace Catoms3D;

int main(int argc, char **argv) {
	createSimulator(argc, argv, ABCcenterCode::buildNewBlockCode);
	Scheduler *scheduler = getScheduler();

	getSimulator()->printInfo();
	scheduler->printInfo();
	BaseSimulator::getWorld()->printInfo();
	deleteSimulator();
	return(0);
}
