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
#include "network.h"
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


VMDataMessage::VMDataMessage(uint64_t src, uint64_t size, uint64_t* m):Message() {
		message = new uint64_t[size/sizeof(uint64_t)+1];
		memcpy(message+1, m, size);
		message[0] = size;
		message[2] = src; 
		switch (m[3]) {
			case Front:
				message[4] = Back;
			case Back:
				message[4] = Front;
				break;
			case Left:
				message[4] = Right;
				break;
			case Right:
				message[4] = Left;
				break;
			case Top:
				message[4] = Bottom;
				break;
			case Bottom:
				message[4] = Top;
				break;
			default:
				cerr << "*** ERROR *** : unknown facet" << endl;
				break;
		}
	}
	
	VMDataMessage::~VMDataMessage() {
		delete[] message;
	}
	
	unsigned int VMDataMessage::size() {
		return message[0]+sizeof(uint64_t);
	}

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
		uint64_t size = bb->getBufferPtr()->size;
		
		switch (message[0]) {
			case VM_MESSAGE_SET_COLOR:			
			{
				// <red> <blue> <green> <intensity>
				//info << "blinky01BlockCode: " << hostBlock->blockId << " was asked to start transmitting to " << destId;
				//getScheduler()->trace(info.str());
				Vecteur color(message[3]/255.0, message[4]/255.0, message[5]/255.0, message[6]/255.0);
				bb->setColor(color);
			}	
			break;
			case VM_MESSAGE_SEND_MESSAGE:
			{
				// <face> <content...>
				//info << "FlavioBlockCode " << hostBlock->blockId << " was asked to start transmitting to " << ;
				//getScheduler()->trace(info.str());
				P2PNetworkInterface *interface;
				interface = bb->getInterface((NeighborDirection)message[3]);
				BaseSimulator::getScheduler()->schedule(new NetworkInterfaceEnqueueOutgoingEvent(BaseSimulator::getScheduler()->now(),
					new VMDataMessage(hostBlock->blockId, size, message), interface));
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
		message[4] = (boost::static_pointer_cast<VMAddNeighborEvent>(pev))->target; // target ????
		message[5] = (boost::static_pointer_cast<VMAddNeighborEvent>(pev))->face; // face
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
		message[4] = (boost::static_pointer_cast<VMRemoveNeighborEvent>(pev))->face; // face
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
	case EVENT_RECEIVE_MESSAGE: /*EVENT_NI_RECEIVE: */
		{
		VMDataMessage *m = (VMDataMessage*) (boost::static_pointer_cast<NetworkInterfaceReceiveEvent>(pev))->message.get();
		bb->sendMessageToVM(m->size(), m->message);
		}
		break;
	case EVENT_ACCEL:
		{
		uint64_t message[7];
		message[0] = 6*sizeof(uint64_t);	
		message[1] = VM_MESSAGE_ACCEL;
		message[2] = BaseSimulator::getScheduler()->now(); // timestamp
		message[3] = -1; // souce node
		message[4] = (boost::static_pointer_cast<VMAccelEvent>(pev))->x; // x
		message[5] = (boost::static_pointer_cast<VMAccelEvent>(pev))->y;; // y
		message[6] = (boost::static_pointer_cast<VMAccelEvent>(pev))->z;; // z
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
		message[4] = (boost::static_pointer_cast<VMShakeEvent>(pev))->force; // force
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


