/*
 * blinkyBlocksBlock.cpp
 *
 *  Created on: 23 mars 2013
 *      Author: dom
 */

#include <iostream>
#include "blinkyBlocksBlock.h"
//#include <sys/types.h>
#include <sys/wait.h>

using namespace std;

namespace BlinkyBlocks {

BlinkyBlocksBlock::BlinkyBlocksBlock(int bId, boost::shared_ptr<tcp::socket> s, pid_t p, BlinkyBlocksBlockCode *(*blinkyBlocksBlockCodeBuildingFunction)(BlinkyBlocksBlock*)) : BaseSimulator::BuildingBlock(bId) {
	cout << "BlinkyBlocksBlock constructor" << endl;
	buildNewBlockCode = blinkyBlocksBlockCodeBuildingFunction;
	blockCode = (BaseSimulator::BlockCode*)buildNewBlockCode(this);
 	socket = s;
	pid = p;
}


BlinkyBlocksBlock::~BlinkyBlocksBlock() {
	cout << "BlinkyBlocksBlock destructor" << endl;
	waitpid(pid, NULL, 0);
}

}
