/*
 * catoms2DBlock.cpp
 *
 *  Created on: 12 janvier 2014
 *      Author: Benoît
 */

#include <iostream>
#include "catoms2DBlock.h"
#include "buildingBlock.h"
#include "catoms2DWorld.h"
#include "catoms2DSimulator.h"
#include "trace.h"

using namespace std;

namespace Catoms2D {

string NeighborDirection::getString(int d) {
	switch(d) {
		case BottomLeft:
			return string("Bottom-left");
			break;
		case Left:
			return string("Left");
			break;
		case TopLeft:
			return string("Top-left");
			break;
		case BottomRight:
			return string("Bottom-right");
			break;
		case Right:
			return string("Right");
			break;
		case TopRight:
			return string("Top-right");
			break;
		default:
			cerr << "Unknown direction" << endl;
			return string("Unknown");
			break;
	}
}

int NeighborDirection::getOpposite(int d) {
switch (Direction(d)) {
		case BottomLeft:
			return TopRight;
			break;
		case Left:
			return Right;
			break;
		case TopLeft:
			return BottomRight;
			break;
		case BottomRight:
			return TopLeft;
			break;
		case Right:
			return Left;
			break;
		case TopRight:
			return BottomLeft;
			break;
		default:
			ERRPUT << "*** ERROR *** : unknown face" << endl;
			return -1;
			break;
	}
}

Catoms2DBlock::Catoms2DBlock(int bId, Catoms2DBlockCode *(*catoms2DBlockCodeBuildingFunction)(Catoms2DBlock*)) : BaseSimulator::BuildingBlock(bId) {
	OUTPUT << "Catoms2DBlock constructor" << endl;
	for (int i=0; i<6; i++) {
		tabInterfaces[i] = new P2PNetworkInterface(this);
	}
	buildNewBlockCode = catoms2DBlockCodeBuildingFunction;
	blockCode = (BaseSimulator::BlockCode*)buildNewBlockCode(this);
 }

Catoms2DBlock::~Catoms2DBlock() {
	OUTPUT << "Catoms2DBlock destructor " << blockId << endl;
}

void Catoms2DBlock::setPosition(const Vecteur &p) {
	position=p;
	getWorld()->updateGlData(this);
}

void Catoms2DBlock::setColor(const Color &c) {
	color = c;
	getWorld()->updateGlData(this,Vecteur(ptrGlBlock->position[0],ptrGlBlock->position[1],ptrGlBlock->position[2]),ptrGlBlock->angle);
}

NeighborDirection::Direction Catoms2DBlock::getDirection(P2PNetworkInterface *given_interface) {
	if( !given_interface) {
		return NeighborDirection::Direction(0);
	}
	for( int i(0); i < 6; ++i) {
		if( tabInterfaces[i] == given_interface) return NeighborDirection::Direction(i);
	}
	return NeighborDirection::Direction(0);
}

P2PNetworkInterface *Catoms2DBlock::getP2PNetworkInterfaceByRelPos(const PointRel3D &pos) {
	// NOT TESTED. FALSE I THINK, DEPENDS ON THE PARITY OF THE LINE
    if (pos.x==-1 && pos.y==0) return tabInterfaces[NeighborDirection::Left];
    else if (pos.x==1 && pos.y==0) return tabInterfaces[NeighborDirection::Right];

    else if (pos.y==-1 && pos.x==1) return tabInterfaces[NeighborDirection::BottomRight];
    else if (pos.y==1 && pos.x==1) return tabInterfaces[NeighborDirection::TopRight];

    else if (pos.z==-1) return tabInterfaces[NeighborDirection::BottomLeft];
    else if (pos.z==1) return tabInterfaces[NeighborDirection::TopLeft];

    return NULL;
}

std::ostream& operator<<(std::ostream &stream, Catoms2DBlock const& bb) {
  stream << bb.blockId << "\tcolor: " << bb.color;
  return stream;
}

}
