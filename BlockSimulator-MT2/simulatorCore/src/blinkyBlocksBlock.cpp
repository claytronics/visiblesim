/*
 * blinkyBlocksBlock.cpp
 *
 *  Created on: 23 mars 2013
 *      Author: dom
 */

#include <iostream>
#include "blinkyBlocksBlock.h"

using namespace std;

namespace BlinkyBlocks {

BlinkyBlocksBlock::BlinkyBlocksBlock(int bId, BlinkyBlocksBlockCode *(*blinkyBlocksBlockCodeBuildingFunction)(BlinkyBlocksBlock*)) : BaseSimulator::BuildingBlock(bId) {
	cout << "BlinkyBlocksBlock constructor" << endl;
	buildNewBlockCode = blinkyBlocksBlockCodeBuildingFunction;
	blockCode = (BaseSimulator::BlockCode*)buildNewBlockCode(this);
}


BlinkyBlocksBlock::~BlinkyBlocksBlock() {
	cout << "BlinkyBlocksBlock destructor" << endl;
}

}
