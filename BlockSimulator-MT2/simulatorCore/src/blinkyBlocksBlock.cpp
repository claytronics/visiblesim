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
#include <sys/wait.h>

using namespace std;

namespace BlinkyBlocks {
static const GLfloat tabColors[12][4]={{1.0,0.0,0.0,1.0},{1.0,0.647058824,0.0,1.0},{1.0,1.0,0.0,1.0},{0.0,1.0,0.0,1.0},
									{0.0,0.0,1.0,1.0},{0.274509804,0.509803922,0.705882353,1.0},{0.815686275,0.125490196,0.564705882,1.0},{0.5,0.5,0.5,1.0},
{0.980392157,0.5,0.456,1.0},{0.549019608,0.5,0.5,1.0},{0.980392157,0.843137255,0.0,1.0},{0.094117647,0.545098039,0.094117647,1.0}};


BlinkyBlocksBlock::BlinkyBlocksBlock(int bId, boost::shared_ptr<tcp::socket> s, pid_t p, BlinkyBlocksBlockCode *(*blinkyBlocksBlockCodeBuildingFunction)(BlinkyBlocksBlock*)) : BaseSimulator::BuildingBlock(bId) {
	cout << "BlinkyBlocksBlock constructor" << endl;
	buildNewBlockCode = blinkyBlocksBlockCodeBuildingFunction;
	socket = s;
	pid = p;
	buffer.message = NULL;
	blockCode = (BaseSimulator::BlockCode*)buildNewBlockCode(this);
	for (int i=0; i<6; i++) {
		tabInterfaces[i] = new P2PNetworkInterface(this);
	}
}


BlinkyBlocksBlock::~BlinkyBlocksBlock() {
	cout << "BlinkyBlocksBlock destructor" << endl;
	//kill(pid, SIGTERM); -- seg fault
	if(waitpid(pid, NULL, WNOHANG) == 0) {
			cout << pid << " killed" <<endl;
			//kill(pid, SIGINT); --seg fault ...
			waitpid(pid, NULL, 0);
	}
}

void BlinkyBlocksBlock::waitVMEnd() {	
	waitpid(pid, NULL, 0);
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

  void BlinkyBlocksBlock::readMessageHandler(const boost::system::error_code& error, std::size_t bytes_transferred){
	if(error) {cout << "socket closed ?" <<endl; return;}
    //cout << "SIM RECEPTION: VM " << blockId << " sends: " << buffer.size << " bytes" << endl;
    delete[] buffer.message;
    buffer.message = new uint64_t[buffer.size/sizeof(uint64_t)];
    boost::asio::read(getSocket(),boost::asio::buffer((void*)buffer.message, buffer.size) );
    //cout << "SIM RECEPTION: VM " << blockId << " message: " << buffer.message[0] << endl;
    ((BlinkyBlocksBlockCode*)blockCode)->handleNewMessage();
    this->readMessageFromVM();
  }
  
  void BlinkyBlocksBlock::readMessageFromVM() {
	boost::asio::async_read(getSocket(), 
      boost::asio::buffer(&buffer.size, sizeof(uint64_t)),
      boost::bind(&BlinkyBlocksBlock::readMessageHandler, this, boost::asio::placeholders::error,
      boost::asio::placeholders::bytes_transferred));  
  }
  
}
