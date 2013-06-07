/*
 * blinkyBlockWorld.cpp
 *
 *  Created on: 23 mars 2013
 *      Author: dom
 */

#include <iostream>
#include <stdlib.h>
#include "blinkyBlocksWorld.h"
#include "vecteur.h"
#include "blinkyBlocksBlock.h"

using namespace std;

typedef struct VMMessage_tt {
        uint64_t messageType;
        uint64_t param1;
        uint64_t param2;
        uint64_t param3;
} VMMessage_t;

namespace BlinkyBlocks {

BlinkBlocksWorld::BlinkBlocksWorld() {
}

BlinkBlocksWorld::BlinkBlocksWorld(int gw,int gh) : ios() {
	cout << "BlinkBlocksWorld constructor" << endl;
	gridWidth = gw;
	gridHeight = gh;
	acceptor =  new tcp::acceptor(ios, tcp::endpoint(tcp::v4(), 7800));
}

void BlinkBlocksWorld::addBlock(int blockId, BlinkyBlocksBlockCode *(*blinkyBlocksBlockCodeBuildingFunction)(BlinkyBlocksBlock*),const Vecteur &pos,const Vecteur &col) {
	// Start the VM
	pid_t VMPid;
	char* cmd[] = {(char*)"VMEmulator", (char*)"-f", (char*)"program.meld", NULL };
	VMPid = fork();	
	if(VMPid < 0) { cerr << "Error when starting the VM" << endl;}
    if(VMPid == 0) { //cout <<"\033[1;36m" << endl; 
		execv("VMEmulator", const_cast<char**>(cmd));
	}

	// Wait for an incoming connection	
	boost::shared_ptr<tcp::socket> socket(new tcp::socket(ios));	
	cout << "VM "<< blockId << " will connect" << endl;
	acceptor->accept(*(socket.get()));
	cout << "VM "<< blockId << " connected" << endl;
	
	// Send the id to the block
	VMMessage_t m1;
	m1.messageType = 0;
	m1.param1 = blockId;
	boost::asio::write(*(socket.get()), boost::asio::buffer((void*)&m1,sizeof(m1)));
	cout << "message written to VM "<< blockId << endl;

	BlinkyBlocksBlock *blinkyBlock = new BlinkyBlocksBlock(blockId, socket, VMPid, blinkyBlocksBlockCodeBuildingFunction);
	buildingBlocksMap.insert(std::pair<int,BaseSimulator::BuildingBlock*>(blinkyBlock->blockId, (BaseSimulator::BuildingBlock*)blinkyBlock));
	
}

BlinkBlocksWorld::~BlinkBlocksWorld() {
	cout << "BlinkBlocksWorld destructor" << endl;
}


void BlinkBlocksWorld::createWorld(int gw,int gh) {
	world = new BlinkBlocksWorld(gw, gh);
}

void BlinkBlocksWorld::deleteWorld() {
	delete((BlinkBlocksWorld*)world);
}

} // MultiCores namespace
