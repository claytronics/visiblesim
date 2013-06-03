/*
 * flavio01.cpp
 *
 *  Created on: 14 févr. 2013
 *      Author: dom
 */
#include <iostream>
#include "multiCoresSimulator.h"
#include "multiCoresBlock.h"
#include "flavio01BlockCode.h"

using namespace std;

int main(int argc, char **argv) {
	cout << "\033[1;33m" << "Starting simulation (main) 1 ..." << "\033[0m" << endl;

	TiXmlDocument doc("config.xml");


	MultiCoresSimulator simulator = MultiCoresSimulator(argc, argv,
			(MultiCoresBlockCode*(*)(MultiCoresBlock*))Flavio01BlockCode::buildNewBlockCode);

	Scheduler::waitForSchedulerEnd();

	cout << "\033[1;33m" << "end (main)" << "\033[0m" << endl;

	return(0);
}
