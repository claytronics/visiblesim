/*
 * blinkyBlocksBlock.cpp
 *
 *  Created on: 23 mars 2013
 *      Author: dom
 */

#include <iostream>
#include "blinkyBlocksBlock.h"
#include "buildingBlock.h"
#include "blinkyBlocksWorld.h"
#include "blinkyBlocksSimulator.h"
#include "blinkyBlocksEvents.h"
#include "trace.h"

using namespace std;

namespace BlinkyBlocks {
static const GLfloat tabColors[12][4]={{1.0,0.0,0.0,1.0},{1.0,0.647058824,0.0,1.0},{1.0,1.0,0.0,1.0},{0.0,1.0,0.0,1.0},
									{0.0,0.0,1.0,1.0},{0.274509804,0.509803922,0.705882353,1.0},{0.815686275,0.125490196,0.564705882,1.0},{0.5,0.5,0.5,1.0},
{0.980392157,0.5,0.456,1.0},{0.549019608,0.5,0.5,1.0},{0.980392157,0.843137255,0.0,1.0},{0.094117647,0.545098039,0.094117647,1.0}};

int getOppositeDirection(int d) {
switch (NeighborDirection(d)) {
		case Front:
			return Back;
			break;
		case Back:
			return Front;
			break;
		case Left:
			return Right;
			break;
		case Right:
			return Left;
			break;
		case Top:
			return Bottom;
			break;
		case Bottom:
			return Top;
			break;
		default:
			ERRPUT << "*** ERROR *** : unknown face" << endl;
			return -1;
			break;
	}
}

BlinkyBlocksBlock::BlinkyBlocksBlock(int bId, BlinkyBlocksBlockCode *(*blinkyBlocksBlockCodeBuildingFunction)(BlinkyBlocksBlock*)) : BaseSimulator::BuildingBlock(bId) {
	OUTPUT << "BlinkyBlocksBlock constructor" << endl;
	for (int i=0; i<6; i++) {
		tabInterfaces[i] = new P2PNetworkInterface(this);
	}
	vm = new BlinkyBlocksVM(this);
	buildNewBlockCode = blinkyBlocksBlockCodeBuildingFunction;
	blockCode = (BaseSimulator::BlockCode*)buildNewBlockCode(this);
}

BlinkyBlocksBlock::~BlinkyBlocksBlock() {
	OUTPUT << "BlinkyBlocksBlock destructor " << blockId << endl;
	//delete[] tabInterfaces;
	stopVM();
}

void BlinkyBlocksBlock::stopVM() {
	if (vm != NULL) {
		delete vm;
		vm = NULL;
	}
}

void BlinkyBlocksBlock::setPosition(const Vecteur &p) {
	position=p;
	getWorld()->updateGlData(this);
}

void BlinkyBlocksBlock::setColor(const Vecteur &c) {
	color=c;
	getWorld()->updateGlData(this);
}

void BlinkyBlocksBlock::setColor(int num) {
	const GLfloat *col = tabColors[num%12];
	color.set(col[0],col[1],col[2],col[3]);
	getWorld()->updateGlData(this);
}

NeighborDirection BlinkyBlocksBlock::getDirection(P2PNetworkInterface *given_interface) {
	if( !given_interface) {
		return NeighborDirection(0);
	}
	for( int i(0); i < 6; ++i) {
		if( tabInterfaces[i] == given_interface) return NeighborDirection(i);
	}
	return NeighborDirection(0);
}

void BlinkyBlocksBlock::tap() {
	getScheduler()->schedule(new VMTapEvent(getScheduler()->now(), this));
	//getScheduler()->scheduleLock(new VMTapEvent(getScheduler()->now(), this));
	OUTPUT << "tap scheduled" << endl;
}
  
void BlinkyBlocksBlock::accel(int x, int y, int z) {
	getScheduler()->schedule(new VMAccelEvent(getScheduler()->now(), this, x, y, z));
	//getScheduler()->scheduleLock(new VMAccelEvent(getScheduler()->now(), this, x, y, z));
}
	
void BlinkyBlocksBlock::shake(int f) {
	getScheduler()->schedule(new VMShakeEvent(getScheduler()->now(), this, f));
	//getScheduler()->scheduleLock(new VMShakeEvent(getScheduler()->now(), this, f));
}
	
string getStringNeighborDirection(uint64_t d) {
	switch(d) {
		case Front:
			return string("Front");
			break;
		case Back:
			return string("Back");
			break;
		case Left:
			return string("Left");
			break;
		case Right:
			return string("Right");
			break;
		case Top:
			return string("Top");
			break;
		case Bottom:
			return string("Bottom");
			break;
		default:
			cerr << "Unknown direction" << endl;
			return string("Unknown");
			break;
	}
} 
  
void BlinkyBlocksBlock::addNeighbor(P2PNetworkInterface *ni, BuildingBlock* target) {
	OUTPUT << "Simulator: "<< blockId << " add neighbor " << target->blockId << " on " << getStringNeighborDirection(getDirection(ni)) << endl;
	getScheduler()->schedule(new VMAddNeighborEvent(getScheduler()->now(), this, getOppositeDirection(getDirection(ni)), target->blockId));
	//getScheduler()->scheduleLock(new VMAddNeighborEvent(getScheduler()->now(), this, getDirection(ni), target->blockId));
}

void BlinkyBlocksBlock::removeNeighbor(P2PNetworkInterface *ni) {
	OUTPUT << "Simulator: "<< blockId << " remove neighbor on " << getStringNeighborDirection(getDirection(ni)) << endl;
	getScheduler()->schedule(new VMRemoveNeighborEvent(getScheduler()->now(), this, getOppositeDirection(getDirection(ni))));
	//getScheduler()->scheduleLock(new VMRemoveNeighborEvent(getScheduler()->now(), this, getDirection(ni)));
}
  
void BlinkyBlocksBlock::stop() {
	OUTPUT << "Simulator: stop VM" << endl;
	setState(Stopped);
	getScheduler()->schedule(new VMStopEvent(getScheduler()->now(), this));
	//getScheduler()->scheduleLock(new VMStopEvent(getScheduler()->now(), this));
}

}
