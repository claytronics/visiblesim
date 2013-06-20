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
	cout << "BlinkyBlocksBlock destructor " << blockId << endl;
	//delete[] tabInterfaces;
	socket->cancel();
	socket->close();
	socket.reset();
	delete[] buffer.message;
	kill(pid, SIGTERM);
	waitpid(pid, NULL, 0);
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

void BlinkyBlocksBlock::readMessageHandler(const boost::system::error_code& error, std::size_t bytes_transferred) {
	cout << "handler called" << endl;
	if(error) {
		cerr << "an error occurred while receiving a tcp message from VM " << blockId << " (socket closed ?) " <<endl;
		return;
	}
    delete[] buffer.message;
    buffer.message = new uint64_t[buffer.size/sizeof(uint64_t)];
    try {
		boost::asio::read(getSocket(),boost::asio::buffer((void*)buffer.message, buffer.size) );
	} catch (std::exception& e) {
		cerr << "connection to the VM "<< blockId << " lost" << endl;
	}
    ((BlinkyBlocksBlockCode*)blockCode)->handleNewMessage();
    this->readMessageFromVM();
}
  
void BlinkyBlocksBlock::readMessageFromVM() {
	if (socket == NULL) {
		cerr << "the simulator is not connected to the VM "<< blockId << endl;
		return;
	}
	try {
	boost::asio::async_read(getSocket(), 
		boost::asio::buffer(&buffer.size, sizeof(uint64_t)),
		boost::bind(&BlinkyBlocksBlock::readMessageHandler, this, boost::asio::placeholders::error,
		boost::asio::placeholders::bytes_transferred));
	} catch (std::exception& e) {
		cerr << "connection to the VM "<< blockId << " lost" << endl;
	}
}
  
void BlinkyBlocksBlock::sendMessageToVM(uint64_t size, uint64_t* message){
	if (socket == NULL) {
		cerr << "the simulator is not connected to the VM "<< blockId << endl;
		return;
	}
	try {
		boost::asio::write(getSocket(), boost::asio::buffer((void*)message,size));
	} catch (std::exception& e) {
		cerr << "connection to the VM "<< blockId << " lost" << endl;
	}
}
  
void BlinkyBlocksBlock::tap() {
	getScheduler()->schedule(new VMTapEvent(getScheduler()->now(), this));
	//getScheduler()->scheduleLock(new VMTapEvent(getScheduler()->now(), this));
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
	cout << "Simulator: "<< blockId << " add neighbor " << target->blockId << " on " << getStringNeighborDirection(getDirection(ni)) << endl;
	getScheduler()->schedule(new VMAddNeighborEvent(getScheduler()->now(), this, getDirection(ni), target->blockId));
	//getScheduler()->scheduleLock(new VMAddNeighborEvent(getScheduler()->now(), this, getDirection(ni), target->blockId));
}

void BlinkyBlocksBlock::removeNeighbor(P2PNetworkInterface *ni) {
	cout << "Simulator: "<< blockId << " remove neighbor on " << getStringNeighborDirection(getDirection(ni)) << endl;
	getScheduler()->schedule(new VMRemoveNeighborEvent(getScheduler()->now(), this, getDirection(ni)));
	//getScheduler()->scheduleLock(new VMRemoveNeighborEvent(getScheduler()->now(), this, getDirection(ni)));
}
  
void BlinkyBlocksBlock::stop() {
	cout << "Simulator: stop VM" << endl;
	getScheduler()->schedule(new VMStopEvent(getScheduler()->now(), this));
	//getScheduler()->scheduleLock(new VMStopEvent(getScheduler()->now(), this));
}

}
