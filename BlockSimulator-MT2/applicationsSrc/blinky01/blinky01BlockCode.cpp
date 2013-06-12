/*
 * blinky01BlockCode.cpp
 *
 *  Created on: 26 mars 2013
 *      Author: dom
 */

#include <iostream>
#include <sstream>
#include "blinky01BlockCode.h"
#include "scheduler.h"
#include <boost/asio.hpp> 
#include "blinkyBlocksEvents.h"

using namespace std;
using namespace BlinkyBlocks;
using boost::asio::ip::tcp;

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
	BaseSimulator::getScheduler()->schedule(new VMSetIdEvent(BaseSimulator::getScheduler()->now(), (BlinkyBlocksBlock*)hostBlock));
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
			case VM_MESSAGE_SET_COLOR:			
			{
				// <red> <blue> <green> <intensity>
				Vecteur color(message[3]/255.0, message[4]/255.0, message[5]/255.0, message[6]/255.0);
				bb->setColor(color);
			}	
			break;
			case VM_MESSAGE_SEND_MESSAGE:
			{
				// to do
				cout << "message transmission not supported yet" << endl;
			}
			default:
				cerr << "*** ERROR *** : unsupported message received from VM" << endl;
				break;
		}
	}

void Blinky01BlockCode::processLocalEvent(EventPtr pev) {
	BlinkyBlocksBlock *bb = (BlinkyBlocksBlock*) hostBlock;
	switch (pev->eventType) {
	case EVENT_SET_ID:
		{
		uint64_t message[5];
		message[0] = 4*sizeof(uint64_t);	
		message[1] = VM_MESSAGE_SET_ID;
		message[2] = BaseSimulator::getScheduler()->now(); // timestamp
		message[3] = -1; // souce node
		message[4] = hostBlock->blockId;
		bb->sendMessageToVM(5*sizeof(uint64_t), message);
		cout << "ID sent to the VM " << hostBlock->blockId << endl;
		}
		break;
	case EVENT_STOP:
		{			
		uint64_t message[4];
		message[0] = 3*sizeof(uint64_t);	
		message[1] = VM_MESSAGE_STOP;
		message[2] = BaseSimulator::getScheduler()->now();
		message[3] = -1; // souce node
		bb->sendMessageToVM(4*sizeof(uint64_t), message);
		}
		break;
	case EVENT_ADD_NEIGHBOR:
		{
		uint64_t message[6];
		message[0] = 5*sizeof(uint64_t);	
		message[1] = VM_MESSAGE_ADD_NEIGHBOR;
		message[2] = BaseSimulator::getScheduler()->now(); // timestamp
		message[3] = -1; // souce node
		message[4] = 0; // target ????
		message[5] = 0; // face
		bb->sendMessageToVM(6*sizeof(uint64_t), message);
		}
		break;
	case EVENT_REMOVE_NEIGHBOR:
		{
		uint64_t message[5];
		message[0] = 4*sizeof(uint64_t);	
		message[1] = VM_MESSAGE_REMOVE_NEIGHBOR;
		message[2] = BaseSimulator::getScheduler()->now(); // timestamp
		message[3] = -1; // souce node
		message[4] = 0; // face
		bb->sendMessageToVM(5*sizeof(uint64_t), message);
		}
		break;
	case EVENT_TAP:
		{
		uint64_t message[4];
		message[0] = 3*sizeof(uint64_t);	
		message[1] = VM_MESSAGE_TAP;
		message[2] = BaseSimulator::getScheduler()->now(); // timestamp
		message[3] = -1; // souce node
		bb->sendMessageToVM(4*sizeof(uint64_t), message);
		}
		break;
	case EVENT_RECEIVE_MESSAGE:
		{
		cout << "message transmission not supported yet" << endl;
		/*uint64_t message[5];
		message[0] = 4*sizeof(uint64_t);	
		message[1] = VM_MESSAGE_TAP;
		message[2] = BaseSimulator::getScheduler()->now(); // timestamp
		message[3] = -1; // souce node
		message[4] = 0; // face
		// mappe avec le contenu envoyer
		bb->sendMessageToVM(5*sizeof(uint64_t), message); */
		}
		break;
	case EVENT_ACCEL:
		{
		uint64_t message[7];
		message[0] = 6*sizeof(uint64_t);	
		message[1] = VM_MESSAGE_ACCEL;
		message[2] = BaseSimulator::getScheduler()->now(); // timestamp
		message[3] = -1; // souce node
		message[4] = 0; // x
		message[5] = 0; // y
		message[6] = 0; // z
		bb->sendMessageToVM(7*sizeof(uint64_t), message);
		}
		break;
	case EVENT_SHAKE:
		{
		uint64_t message[5];
		message[0] = 4*sizeof(uint64_t);	
		message[1] = VM_MESSAGE_SET_ID;
		message[2] = BaseSimulator::getScheduler()->now(); // timestamp
		message[3] = -1; // souce node
		message[4] = 0;// force
		bb->sendMessageToVM(5*sizeof(uint64_t), message);
		}
		break;
	default:
		cerr << "*** ERROR *** : unknown local event" << endl;
		break;
	}
}

BlinkyBlocks::BlinkyBlocksBlockCode* Blinky01BlockCode::buildNewBlockCode(BlinkyBlocksBlock *host) {
	return(new Blinky01BlockCode(host));
}


