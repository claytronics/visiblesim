/*
 * SmartBlocksBlock.cpp
 *
 *  Created on: 12 avril 2013
 *      Author: ben
 */

#include <iostream>
#include "smartBlocksBlock.h"
#include "smartBlocksWorld.h"

using namespace std;

namespace SmartBlocks {
static const GLfloat tabColors[12][4]={{1.0,0.0,0.0,1.0},{1.0,0.647058824,0.0,1.0},{1.0,1.0,0.0,1.0},{0.0,1.0,0.0,1.0},
                  {0.0,0.0,1.0,1.0},{0.274509804,0.509803922,0.705882353,1.0},{0.815686275,0.125490196,0.564705882,1.0},{0.5,0.5,0.5,1.0},
{0.980392157,0.5,0.456,1.0},{0.549019608,0.85,0.5,1.0},{0.980392157,0.843137255,0.0,1.0},{0.094117647,0.545098039,0.094117647,1.0}};


SmartBlocksBlock::SmartBlocksBlock(int bId, SmartBlocksBlockCode *(*smartBlocksBlockCodeBuildingFunction)(SmartBlocksBlock*)) : BaseSimulator::BuildingBlock(bId) {
  cout << "SmartBlocksBlock constructor" << endl;
  buildNewBlockCode = smartBlocksBlockCodeBuildingFunction;
  blockCode = (BaseSimulator::BlockCode*)buildNewBlockCode(this);
  for (int i=North; i<=West; i++) {
    tabInterfaces[i] = new P2PNetworkInterface(this);
  }
}


SmartBlocksBlock::~SmartBlocksBlock() {
  //MODIF NICO
  for (int i=North; i<=West; i++) {
    delete tabInterfaces[i];
  }
  //FIN MODIF NICO
  cout << "SmartBlocksBlock destructor :" << blockId << endl;
}

void SmartBlocksBlock::setPosition(const Vecteur &p) {
  position=p;
  getWorld()->updateGlData(this);
}

void SmartBlocksBlock::setColor(const Vecteur &c) {
  color=c;
  getWorld()->updateGlData(this);
}

void SmartBlocksBlock::setColor(int num) {
  const GLfloat *col = tabColors[num%12];
  color.set(col[0],col[1],col[2],col[3]);
  getWorld()->updateGlData(this);
}

//MODIF NICO
NeighborDirection SmartBlocksBlock::getDirection(P2PNetworkInterface *given_interface)
{
  if( !given_interface) {
    return NeighborDirection( 0);
  }
  for( int i( North); i <= West; ++i) {
    P2PNetworkInterface* p2p = tabInterfaces[ i];
    if( p2p == given_interface) {
      return NeighborDirection( i);
    }
  }
  assert(0);			// should never get here
}
//FIN MODIF NICO

}
