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
#include "trace.h"
#include "blinkyBlocksDebugger.h"

using namespace std;
using namespace BlinkyBlocks;
using boost::asio::ip::tcp;

#define VM_MESSAGE_SET_ID						1
#define VM_MESSAGE_STOP							4
#define VM_MESSAGE_ADD_NEIGHBOR					5
#define VM_MESSAGE_REMOVE_NEIGHBOR				6
#define VM_MESSAGE_TAP							7
#define VM_MESSAGE_SET_COLOR					8
#define VM_MESSAGE_SEND_MESSAGE					12
#define VM_MESSAGE_RECEIVE_MESSAGE				13
#define VM_MESSAGE_ACCEL						14
#define VM_MESSAGE_SHAKE						15
#define VM_MESSAGE_DEBUG						16

#define VM_MESSAGE_START_COMPUTATION 			20
#define VM_MESSAGE_END_COMPUTATION 				21
#define VM_MESSAGE_WAIT_FOR_MESSAGE				22

string getStringMessage(uint64_t t) {
	switch(t) {
		case VM_MESSAGE_SET_COLOR:
			return string("VM_MESSAGE_SET_COLOR");
			break;
		case VM_MESSAGE_SEND_MESSAGE:
			return string("VM_MESSAGE_SEND_MESSAGE");
			break;
		case VM_MESSAGE_DEBUG:
			return string("VM_MESSAGE_DEBUG");
			break;
		default:
			ERRPUT << "Unknown received-message type" << endl;
			return string("Unknown");
			break;
		}
}

VMDataMessage::VMDataMessage(uint64_t src, uint64_t* m): Message() {
	uint64_t size = m[0] + sizeof(uint64_t);
	message = new uint64_t[size/sizeof(uint64_t)+1];
	memcpy(message, m, size);
	message[1] = VM_MESSAGE_RECEIVE_MESSAGE;
	//message[2] = 0; // timestamp
	//message[3] = m[5];
	//message[5] = m[3];
	message[5] = src;
	message[3] = m[5];
	
	/*message[3] = src;
	switch (m[4]) {
		case Front:
			message[4] = Back;
			break;
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
			ERRPUT << "*** ERROR *** : unknown face" << endl;
			break;
	}*/
}
	
VMDataMessage::~VMDataMessage() {
	delete[] message;
}
	
unsigned int VMDataMessage::size() {
	return message[0] + sizeof(uint64_t);
}

Blinky01BlockCode::Blinky01BlockCode(BlinkyBlocksBlock *host): BlinkyBlocksBlockCode(host) {
	OUTPUT << "Blinky01BlockCode constructor" << endl;
	computing = false;
	waitingForMessage = false;
	//waitingForVM = true;
}

Blinky01BlockCode::~Blinky01BlockCode() {
	OUTPUT << "Blinky01BlockCode destructor" << endl;
}

void Blinky01BlockCode::startup() {
	stringstream info;
	info << "  Starting Blinky01BlockCode in block " << hostBlock->blockId;
	BlinkyBlocks::getScheduler()->trace(info.str(),hostBlock->blockId);
	((BlinkyBlocksBlock*)hostBlock)->vm->asyncReadMessage();
}

void Blinky01BlockCode::handleNewMessage() {
	BlinkyBlocksBlock *bb = (BlinkyBlocksBlock*) hostBlock;
	OUTPUT << "Blinky01BlockCode: type: " << getStringMessage(bb->vm->getBufferPtr()->message[1]) << " size: " << bb->vm->getBufferPtr()->message[0] << endl;
	uint64_t* message = bb->vm->getBufferPtr()->message;
	switch (message[1]) {
		case VM_MESSAGE_SET_COLOR:			
			{
			// format: <size> <timestamp> <src> <command> <red> <blue> <green> <intensity>
			Vecteur color((float)message[4]/255.0, (float)message[5]/255.0, (float)message[6]/255.0, (float)message[7]/255.0);
			bb->setColor(color);
			}	
			break;
		case VM_MESSAGE_SEND_MESSAGE:
			{
			// format: <size> <timestamp> <src> <command> <face> <content...>
			
			//cout << "receive a message: " << message[0] << " " << message[1] << " " << message[2] << " "<< message[3] << " " << message[4] << " " << message[5] << " " << message[6] << endl;
			P2PNetworkInterface *interface;
			//interface = bb->getInterface((NeighborDirection)message[4]);
			interface = bb->getInterfaceDestId(message[5]);
			//cout << "dest: " << message[<
			//message[4] = bb->getDirection(interface);
			//cout << "message modified: " << message[0] << " " << message[1] << " " << message[2] << " "<< message[3] << " " << message[4] << " " << message[5] << endl;
			if (interface == NULL) {
				OUTPUT << "interface not found" << endl;
				return;
			}
			//cout << bb->blockId << "-->" << interface->hostBlock->blockId << endl;
			BaseSimulator::getScheduler()->schedule(new NetworkInterfaceEnqueueOutgoingEvent(BaseSimulator::getScheduler()->now(),
					new VMDataMessage(hostBlock->blockId, message), interface));
					/*BaseSimulator::getScheduler()->scheduleLock(new NetworkInterfaceEnqueueOutgoingEvent(BaseSimulator::getScheduler()->now(),
					new VMDataMessage(hostBlock->blockId, size, message), interface));*/
			}
			break;
		case VM_MESSAGE_DEBUG:
			// debug message handler
			handleDebugMessage(message);
			break;
		case VM_MESSAGE_START_COMPUTATION: // format: <size> <timestamp> <src> <command> <duration>
			if (!BlinkyBlocks::getScheduler()->isBlockUndefined(bb->blockId)) {
				stringstream info;
				info.str("");
				info << "Block " << bb->blockId << " received a COMPUTATION_LOCK whereas it was not in undefined state !";
				BlinkyBlocks::getScheduler()->trace(info.str());
			}
			BlinkyBlocks::getScheduler()->schedule(new VMStartComputationEvent(BlinkyBlocks::getScheduler()->now(), bb, message[4]));
			BlinkyBlocks::getScheduler()->removeUndefinedBlock(bb->blockId);
			break;
		case VM_MESSAGE_WAIT_FOR_MESSAGE:
			if (!BlinkyBlocks::getScheduler()->isBlockUndefined(bb->blockId)) {
				stringstream info;
				info.str("");
				info << "Block " << bb->blockId << " received a WAIT_FOR_MESSAGE whereas it was not in undefined state !";
				BlinkyBlocks::getScheduler()->trace(info.str());
			}
			BlinkyBlocks::getScheduler()->schedule(new VMWaitForMessageEvent(BlinkyBlocks::getScheduler()->now(), bb, message[4]));
			BlinkyBlocks::getScheduler()->removeUndefinedBlock(bb->blockId);
			//getScheduler()->printUndefinedBlocksSet(); */
			break;
		default:
			ERRPUT << "*** ERROR *** : unsupported message received from VM (" << message[1] <<")" << endl;
			break;
	}
}
// WARNING: VMs appear to always use source node...
void Blinky01BlockCode::processLocalEvent(EventPtr pev) {
	stringstream info;

	BlinkyBlocksBlock *bb = (BlinkyBlocksBlock*) hostBlock;
	OUTPUT << "Blinky01BlockCode: " << pev->getEventName() << "(" << pev->eventType << ")" << endl;
	switch (pev->eventType) {
	case EVENT_SET_ID:
		{
		uint64_t message[4];
		message[0] = 3*sizeof(uint64_t);	
		message[1] = VM_MESSAGE_SET_ID;
		message[2] = BaseSimulator::getScheduler()->now(); // timestamp
		message[3] = hostBlock->blockId; // BUG VM: souce node is not send here
		//message[4] = hostBlock->blockId;
		bb->vm->sendMessage(4*sizeof(uint64_t), message);
		OUTPUT << "ID sent to the VM " << hostBlock->blockId << endl;
		BlinkyBlocks::getScheduler()->trace("ID sent to the VM",hostBlock->blockId);
		//waitingForVM = true;
		//BlinkyBlocks::getScheduler()->addUndefinedBlock(hostBlock->blockId);
		}
		break;
	case EVENT_STOP:
		{
		uint64_t message[4];
		message[0] = 3*sizeof(uint64_t);	
		message[1] = VM_MESSAGE_STOP;
		message[2] = BaseSimulator::getScheduler()->now();
		message[3] = hostBlock->blockId;
		bb->vm->sendMessage(4*sizeof(uint64_t), message);
		bb->stopVM();
		}
		break;
	case EVENT_ADD_NEIGHBOR:
		{
		uint64_t message[6];
		message[0] = 5*sizeof(uint64_t);	
		message[1] = VM_MESSAGE_ADD_NEIGHBOR;
		message[2] = BaseSimulator::getScheduler()->now(); // timestamp
		message[3] = bb->blockId; // souce node
		message[4] = (boost::static_pointer_cast<VMAddNeighborEvent>(pev))->target;
		message[5] = (boost::static_pointer_cast<VMAddNeighborEvent>(pev))->face;
		bb->vm->sendMessage(6*sizeof(uint64_t), message);
		//waitingForVM = true;
		//BlinkyBlocks::getScheduler()->addUndefinedBlock(hostBlock->blockId);
		}
		break;
	case EVENT_REMOVE_NEIGHBOR:
		{
		uint64_t message[5];
		message[0] = 4*sizeof(uint64_t);	
		message[1] = VM_MESSAGE_REMOVE_NEIGHBOR;
		message[2] = BaseSimulator::getScheduler()->now(); // timestamp
		message[3] = bb->blockId; // souce node
		message[4] = (boost::static_pointer_cast<VMRemoveNeighborEvent>(pev))->face;
		bb->vm->sendMessage(5*sizeof(uint64_t), message);
		//waitingForVM = true;
		//BlinkyBlocks::getScheduler()->addUndefinedBlock(hostBlock->blockId);
		}
		break;
	case EVENT_TAP:
		{
		uint64_t message[4];
		message[0] = 3*sizeof(uint64_t);	
		message[1] = VM_MESSAGE_TAP;
		message[2] = BaseSimulator::getScheduler()->now(); // timestamp
		message[3] = bb->blockId; // souce node
		bb->vm->sendMessage(4*sizeof(uint64_t), message);
		//waitingForVM = true;
		//BlinkyBlocks::getScheduler()->addUndefinedBlock(hostBlock->blockId);
		}
		break;
	case EVENT_RECEIVE_MESSAGE: /*EVENT_NI_RECEIVE: */
		{
		VMDataMessage *m = (VMDataMessage*) (boost::static_pointer_cast<NetworkInterfaceReceiveEvent>(pev))->message.get();
		//cout << "message receive by " << bb->blockId << ":" << m->message[0] << " " << m->message[1] << " " << m->message[2] << " " << m->message[3] << " " << m->message[4] << " " << m->message[5] << " " << m->message[6] << endl;
		bb->vm->sendMessage(m->size(), m->message);
		info << "message received from " << m->sourceInterface->hostBlock->blockId;
		BlinkyBlocks::getScheduler()->trace(info.str(),hostBlock->blockId);
		}
		break;
	case EVENT_ACCEL:
		{
		uint64_t message[7];
		message[0] = 6*sizeof(uint64_t);	
		message[1] = VM_MESSAGE_ACCEL;
		message[2] = BaseSimulator::getScheduler()->now(); // timestamp
		message[3] = bb->blockId; // souce node
		message[4] = (boost::static_pointer_cast<VMAccelEvent>(pev))->x;
		message[5] = (boost::static_pointer_cast<VMAccelEvent>(pev))->y;
		message[6] = (boost::static_pointer_cast<VMAccelEvent>(pev))->z;
		bb->vm->sendMessage(7*sizeof(uint64_t), message);
		//waitingForVM = true;
		//BlinkyBlocks::getScheduler()->addUndefinedBlock(hostBlock->blockId);
		}
		break;
	case EVENT_SHAKE:
		{
		uint64_t message[5];
		message[0] = 4*sizeof(uint64_t);	
		message[1] = VM_MESSAGE_SET_ID;
		message[2] = BaseSimulator::getScheduler()->now(); // timestamp
		message[3] = bb->blockId; // souce node
		message[4] = (boost::static_pointer_cast<VMShakeEvent>(pev))->force;
		bb->vm->sendMessage(5*sizeof(uint64_t), message);
		//waitingForVM = true;
		//BlinkyBlocks::getScheduler()->addUndefinedBlock(hostBlock->blockId);
		}
		break;
	case EVENT_DEBUG_MESSAGE:
		{
		// forward the debugging message
		VMDebugMessage *m = (VMDebugMessage*) (boost::static_pointer_cast<VMDebugMessageEvent>(pev))->message.get();
		bb->vm->sendMessage(m->size, m->message);
		}
		break;
	case EVENT_VM_START_COMPUTATION:
		{
		uint64_t duration;
		
		if (computing) {
			info.str("");
			info << "*** ERROR *** block " << bb->blockId << " got a COMPUTATION_LOCK from VM but it was already computing !";
			BlinkyBlocks::getScheduler()->trace(info.str());
		}
		computing = true;
		/*
		if (!waitingForVM) {
			info.str("");
			info << "*** ERROR *** block " << bb->blockId << " got a COMPUTATION_LOCK from VM but was not waiting for one";
			BlinkyBlocks::getScheduler()->trace(info.str());
		}*/
		//waitingForVM = false;
		duration = (boost::static_pointer_cast<VMStartComputationEvent>(pev))->duration;
		//availabilityDate = BlinkyBlocks::getScheduler()->now()+duration;
		info.str("");
		info << "blinky01BlockCode " << bb->blockId << " starting computation (will last for " << duration << ")" ;
		BlinkyBlocks::getScheduler()->trace(info.str());
		BlinkyBlocks::getScheduler()->schedule(new VMEndComputationEvent(BlinkyBlocks::getScheduler()->now()+duration, bb));
		}
		break;
	case EVENT_VM_END_COMPUTATION:
		{
		uint64_t message[5];
		
		computing = false;
		info.str("");
		info << "blinky01BlockCode " << bb->blockId << " finished its computation";
		BlinkyBlocks::getScheduler()->trace(info.str());				
		message[0] = 3*sizeof(uint64_t);	
		message[1] = VM_MESSAGE_END_COMPUTATION;
		message[2] = BlinkyBlocks::getScheduler()->now(); // timestamp
		message[3] = bb->blockId; // souce node
		BlinkyBlocks::getScheduler()->addUndefinedBlock(hostBlock->blockId);
		bb->vm->sendMessage(4*sizeof(uint64_t), message);
		//waitingForVM = true;
		}
		break;
	case EVENT_VM_WAIT_FOR_MESSAGE:
		{
		uint64_t timeOut;
		
		waitingForMessage = true;
		timeOut = (boost::static_pointer_cast<VMWaitForMessageEvent>(pev))->timeOut;
		info.str("");
		info << "blinky01BlockCode " << bb->blockId << " starting wait for a message (will last for " << timeOut << ")" ;
		BlinkyBlocks::getScheduler()->trace(info.str());
		BlinkyBlocks::getScheduler()->schedule(new VMTimeOutWaitForMessageEvent(BlinkyBlocks::getScheduler()->now()+timeOut, bb));
		}
		break;
	case EVENT_VM_TIMEOUT_WAIT_FOR_MESSAGE:
		{
			// message not received
			waitingForMessage = false; // + send a message ?
			info.str("");
			info << "blinky01BlockCode " << bb->blockId << " does not wait any more for a message";
			BlinkyBlocks::getScheduler()->trace(info.str());
		}
		break;
	default:
		ERRPUT << "*** ERROR *** : unknown local event" << endl;
		break;
	}
}

BlinkyBlocks::BlinkyBlocksBlockCode* Blinky01BlockCode::buildNewBlockCode(BlinkyBlocksBlock *host) {
	return(new Blinky01BlockCode(host));
}
