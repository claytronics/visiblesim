/*
 * blinky01BlockCode.cpp
 *
 *  Created on: 26 mars 2013
 *      Author: dom
 */

#include <iostream>
#include <sstream>
#include "blinky01BlockCode.h"
//#include <boost/asio.hpp> 

using namespace std;
using namespace BlinkyBlocks;
//using boost::asio::ip::udp;
//using boost::asio::ip::tcp;

#define VM_MESSAGE_SET_ID				        1
#define VM_MESSAGE_STOP							4
#define VM_MESSAGE_ADD_NEIGHBOR					5
#define VM_MESSAGE_REMOVE_NEIGHBOR		        6
#define VM_MESSAGE_TAP							7
#define VM_MESSAGE_SET_COLOR					8
#define VM_MESSAGE_SEND_MESSAGE					9
#define VM_MESSAGE_RECEIVE_MESSAGE				10
#define VM_MESSAGE_ACCEL						11
#define VM_MESSAGE_SHAKE						12


Blinky01BlockCode::Blinky01BlockCode(BlinkyBlocksBlock *host):BlinkyBlocksBlockCode(host) {
	cout << "Blinky01BlockCode constructor" << endl;
	// Send the id to the block
	uint64_t message[3];
	message[0] = 2*sizeof(uint64_t);	
	message[1] = VM_MESSAGE_SET_ID;
	message[2] = host->blockId;
	boost::asio::write(host->getSocket(), boost::asio::buffer((void*)message,3*sizeof(uint64_t)));
}

Blinky01BlockCode::~Blinky01BlockCode() {
	cout << "Blinky01BlockCode destructor" << endl;
}

void Blinky01BlockCode::startup() {
	stringstream info;
	info << "  Starting Blinky01BlockCode in block " << hostBlock->blockId;
	BlinkyBlocks::getScheduler()->trace(info.str());
	((BlinkyBlocksBlock*)hostBlock)->readMessageFromVM();
}

	void Blinky01BlockCode::handleNewMessage() {
		BlinkyBlocksBlock *bb = (BlinkyBlocksBlock*) hostBlock;
		cout << "Scheduler: message size: " << bb->getBufferPtr()->size << endl;
		cout << "Scheduler: param1: " << bb->getBufferPtr()->message[0] << endl;
		uint64_t* message = bb->getBufferPtr()->message;
		
		switch (message[0]) {
			case VM_MESSAGE_SET_COLOR:			{
				// <red> <blue> <green> <intensity>
				Vecteur color(message[1]/255.0, message[2]/255.0, message[3]/255.0, message[4]/255.0);
				bb->setColor(color);
			}	
				break;
			/*
			case VM_MESSAGE_TYPE_CREATE_LINK:
				info.str("");
				info << " Linking block " << mes.param1 << " with " << mes.param2;
				Scheduler::trace(info.str());
				MultiCoresBlock *b0, *b1;
				b0 = getWorld()->getBlockById(mes.param1);
				b1 = getWorld()->getBlockById(mes.param2);
				b0->addP2PNetworkInterfaceAndConnectTo(b1);
				break;
			*/
			default:
				cout << "*** ERROR *** : unsupported message received from VM" << endl;
				break;
		}
	}

void Blinky01BlockCode::processLocalEvent(EventPtr pev) {

}

BlinkyBlocks::BlinkyBlocksBlockCode* Blinky01BlockCode::buildNewBlockCode(BlinkyBlocksBlock *host) {
	return(new Blinky01BlockCode(host));
}


