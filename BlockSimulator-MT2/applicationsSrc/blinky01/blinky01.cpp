/*
 * blinky01.cpp
 *
 *  Created on: 23 mars 2013
 *      Author: dom
 */

#include <iostream>
#include "blinkyBlocksSimulator.h"
#include "blinkyBlocksBlockCode.h"
#include "blinky01BlockCode.h"

using namespace std;
using namespace BlinkyBlocks;

int main(int argc, char **argv) {
	cout << "\033[1;33m" << "Starting Blinky Blocks simulation (main) ..." << "\033[0m" << endl;

	createSimulator(argc, argv, Blinky01BlockCode::buildNewBlockCode);

	{
		using namespace BaseSimulator;

		Simulator *s = Simulator::getSimulator();
		s->printInfo();
	}

	getSimulator()->printInfo();
	BlinkyBlocks::getScheduler()->printInfo();
	BaseSimulator::getWorld()->printInfo();

	//getScheduler()->waitForSchedulerEnd();

	deleteSimulator();

	cout << "\033[1;33m" << "end (main)" << "\033[0m" << endl;

	return(0);
}
