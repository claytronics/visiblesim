/*
 * blinkyBlockWorld.cpp
 *
 *  Created on: 23 mars 2013
 *      Author: dom
 */

#include <iostream>
#include <stdlib.h>
#include "blinkyBlocksWorld.h"

using namespace std;

namespace BlinkyBlocks {

BlinkBlocksWorld::BlinkBlocksWorld() {
	cout << "BlinkBlocksWorld constructor" << endl;
}

BlinkBlocksWorld::~BlinkBlocksWorld() {
	cout << "BlinkBlocksWorld destructor" << endl;
}


void BlinkBlocksWorld::createWorld() {
	world = new BlinkBlocksWorld();
}

void BlinkBlocksWorld::deleteWorld() {
	delete((BlinkBlocksWorld*)world);
}

} // MultiCores namespace
