/*
 * blinky01BlockCode.cpp
 *
 *  Created on: 26 mars 2013
 *      Author: dom
 */

#include <iostream>
#include <sstream>
#include "blinky01BlockCode.h"

using namespace std;
using namespace BlinkyBlocks;

Blinky01BlockCode::Blinky01BlockCode(BlinkyBlocksBlock *host):BlinkyBlocksBlockCode(host) {
	cout << "Blinky01BlockCode constructor" << endl;
}

Blinky01BlockCode::~Blinky01BlockCode() {
	cout << "Blinky01BlockCode destructor" << endl;
}

void Blinky01BlockCode::startup() {
	stringstream info;

	info << "  Starting Blinky01BlockCode in block " << hostBlock->blockId;
	BlinkyBlocks::getScheduler()->trace(info.str());
}


void Blinky01BlockCode::processLocalEvent(EventPtr pev) {

}

BlinkyBlocks::BlinkyBlocksBlockCode* Blinky01BlockCode::buildNewBlockCode(BlinkyBlocksBlock *host) {
	return(new Blinky01BlockCode(host));
}


