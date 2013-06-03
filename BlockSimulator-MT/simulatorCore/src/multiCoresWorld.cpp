/*
 * multiCoresWorld.cpp
 *
 *  Created on: 16 févr. 2013
 *      Author: dom
 */

#include <iostream>
#include "multiCoresSimulator.h"
#include "multiCoresWorld.h"

using namespace std;

MultiCoresWorld::MultiCoresWorld() {
	cout << "\033[36;1m" << "MultiCoresWorld constructor" << "\033[0m" << endl;
}

MultiCoresWorld::~MultiCoresWorld() {
	cout << "\033[36;1m" << "MultiCoresWorld destructor" << "\033[0m" << endl;
}

void MultiCoresWorld::addBlock(int blockId, MultiCoresBlockCode *(*multiCoreBlockCodeBuildingFunction)(MultiCoresBlock*),float posX, float posY, float posZ, float colorR, float colorG, float colorB) {
	MultiCoresBlock *multiCoresBlock;
	multiCoresBlock = new MultiCoresBlock(blockId,multiCoreBlockCodeBuildingFunction);
	blockVect.push_back(multiCoresBlock);
	Scheduler::schedule(new CodeStartEvent(Scheduler::now(),multiCoresBlock));
}
