/*
 * blinkyBlocksScheduler.cpp
 *
 *  Created on: 23 mars 2013
 *      Author: dom
 */

#include <iostream>
#include <stdlib.h>
#include "blinkyBlocksScheduler.h"

using namespace std;

namespace BlinkyBlocks {

BlinkyBlocksScheduler::BlinkyBlocksScheduler() {
	cout << "BlinkyBlocksScheduler constructor" << endl;
}

BlinkyBlocksScheduler::~BlinkyBlocksScheduler() {
	cout << "BlinkyBlocksScheduler destructor" << endl;
}


void BlinkyBlocksScheduler::createScheduler() {
	scheduler = new BlinkyBlocksScheduler();
}

void BlinkyBlocksScheduler::deleteScheduler() {
	delete((BlinkyBlocksScheduler*)scheduler);
}

} // BlinkyBlocks namespace
