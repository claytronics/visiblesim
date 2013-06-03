/*
 * multiCoresBlock.cpp
 *
 *  Created on: 15 févr. 2013
 *      Author: dom
 */

#include <iostream>
#include "multiCoresBlock.h"
#include "simulator.h"

using namespace std;

MultiCoresSimulator *MultiCoresBlock::simulator = NULL;

MultiCoresBlock::MultiCoresBlock(MultiCoresBlockCode *(*multiCoreBlockCodeBuildingFunction)(MultiCoresBlock*)) : BuildingBlock() {
	cout << "MultiCoresBlock constructor" << endl;
	buildNewBlockCode = multiCoreBlockCodeBuildingFunction;
	blockCode = (BlockCode*)buildNewBlockCode(this);
	undefinedState = true;
	simulator = (MultiCoresSimulator*)Simulator::getInstance();
}

MultiCoresBlock::MultiCoresBlock(unsigned int bId, MultiCoresBlockCode *(*multiCoreBlockCodeBuildingFunction)(MultiCoresBlock*)) : BuildingBlock(bId){
	cout << "MultiCoresBlock constructor(copy)" << endl;
	buildNewBlockCode = multiCoreBlockCodeBuildingFunction;
	blockCode = (BlockCode*)buildNewBlockCode(this);
	undefinedState = true;
	simulator = (MultiCoresSimulator*)Simulator::getInstance();
}

MultiCoresBlock::~MultiCoresBlock() {
	cout << "MultiCoresBlock destructor" << endl;
}
