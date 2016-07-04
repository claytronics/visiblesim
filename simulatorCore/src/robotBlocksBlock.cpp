/*
 * robotBlocksBlock.cpp
 *
 *  Created on: 12 janvier 2014
 *      Author: Benoît
 */

#include <iostream>

#include "robotBlocksBlock.h"
#include "robotBlocksWorld.h"
#include "robotBlocksSimulator.h"
#include "trace.h"

using namespace std;

namespace RobotBlocks {

RobotBlocksBlock::RobotBlocksBlock(int bId, BlockCodeBuilder bcb) : BaseSimulator::BuildingBlock(bId, bcb) {
    OUTPUT << "RobotBlocksBlock constructor" << endl;

    for (int i=0; i<6; i++) {
		P2PNetworkInterfaces.push_back(new P2PNetworkInterface(this));
    }

}

RobotBlocksBlock::~RobotBlocksBlock() {
    OUTPUT << "RobotBlocksBlock destructor " << blockId << endl;
}

void RobotBlocksBlock::setPrevNext(int prev,int next) {
    getWorld()->updateGlData(this,prev,next);
}

void RobotBlocksBlock::setPrevNext(const P2PNetworkInterface *prev,const P2PNetworkInterface *next) {
    int prevId=0,nextId=0;
    if (prev) {
		RobotBlocksBlock*rb = (RobotBlocksBlock*)(prev->hostBlock);
		prevId = rb->blockId;
    }
    if (next) {
		RobotBlocksBlock*rb = (RobotBlocksBlock*)(next->hostBlock);
		nextId = rb->blockId;
    }
    //cout << (prev?prev->hostBlock->blockId:-1) << "," << (next?next->hostBlock->blockId:-1) << endl;
    getWorld()->updateGlData(this,prevId,nextId);
}

SCLattice::Direction RobotBlocksBlock::getDirection(P2PNetworkInterface *given_interface) {
    if( !given_interface) {
		return SCLattice::Direction(0);
    }
    for( int i(0); i < 6; ++i) {
		if(P2PNetworkInterfaces[i] == given_interface) return SCLattice::Direction(i);
    }
    return SCLattice::Direction(0);
}

P2PNetworkInterface *RobotBlocksBlock::getP2PNetworkInterfaceByRelPos(const PointRel3D &pos) {
    if (pos.x==-1) return P2PNetworkInterfaces[SCLattice::Left];
    else if (pos.x==1) return P2PNetworkInterfaces[SCLattice::Right];
    else if (pos.y==-1) return P2PNetworkInterfaces[SCLattice::Front];
    else if (pos.y==1) return P2PNetworkInterfaces[SCLattice::Back];
    else if (pos.z==-1) return P2PNetworkInterfaces[SCLattice::Bottom];
    else if (pos.z==1) return P2PNetworkInterfaces[SCLattice::Top];

    return NULL;
}

std::ostream& operator<<(std::ostream &stream, RobotBlocksBlock const& bb) {
    stream << bb.blockId << "\tcolor: " << bb.color;
    return stream;
}

}
