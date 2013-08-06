/*
 * blinky01BlockCode.cpp
 *
 *  Created on: 26 mars 2013
 *      Author: dom
 */

#include <iostream>
#include <sstream>
#include <boost/asio.hpp> 
#include "scheduler.h"
#include "network.h"
#include "blinky01BlockCode.h"
#include "blinkyBlocksEvents.h"
#include "blinkyBlocksDebugger.h"
#include "trace.h"

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

#define VM_SET_DETERMINISTIC_MODE				20
#define VM_MESSAGE_START_COMPUTATION 			21
#define VM_MESSAGE_END_COMPUTATION 				22

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
		case VM_MESSAGE_START_COMPUTATION:
			return string("VM_MESSAGE_START_COMPUTATION");
			break;
		case VM_MESSAGE_END_COMPUTATION:
			return string("VM_MESSAGE_END_COMPUTATION");
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
	message[5] = src;
	message[3] = m[5];
}

VMDataMessage::VMDataMessage(VMDataMessage *m) : Message() {
	uint64_t size = m->message[0] + sizeof(uint64_t);
	message = new uint64_t[size/sizeof(uint64_t)+1];
	memcpy(message, m->message, size);
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
	hasWork = true;
}

Blinky01BlockCode::~Blinky01BlockCode() {
	OUTPUT << "Blinky01BlockCode destructor" << endl;
}

void Blinky01BlockCode::startup() {	
	BlinkyBlocksBlock *bb = (BlinkyBlocksBlock*) hostBlock;
	stringstream info;
	info << "  Starting Blinky01BlockCode in block " << hostBlock->blockId;
	BlinkyBlocks::getScheduler()->trace(info.str(),hostBlock->blockId);
	if (BlinkyBlocks::getScheduler()->getMode() == SCHEDULER_MODE_FASTEST) {
		BlinkyBlocks::getScheduler()->schedule(new VMStartComputationEvent(BaseSimulator::getScheduler()->now()+1, bb, 300));
	}
}

void Blinky01BlockCode::handleNewMessage(uint64_t *message) {
	BlinkyBlocksBlock *bb = (BlinkyBlocksBlock*) hostBlock;
	OUTPUT << "Blinky01BlockCode: type: " << getStringMessage(message[1]) << " size: " << message[0] << endl;
	switch (message[1]) {
		case VM_MESSAGE_SET_COLOR:			
			{
			// format: <size> <command> <timestamp> <src> <red> <blue> <green> <intensity>
			BlinkyBlocks::getScheduler()->schedule(new VMSetColorEvent(BaseSimulator::getScheduler()->now(), bb,
					(float)message[4]/255.0, (float)message[5]/255.0, (float)message[6]/255.0, (float)message[7]/255.0 ));
			}	
			break;
		case VM_MESSAGE_SEND_MESSAGE:
			{
			// format: <size> <command> <timestamp> <src> <destFace=0: not used> <destId> < <content...>
			P2PNetworkInterface *interface;
			interface = bb->getInterfaceDestId(message[5]);
			if (interface == NULL) {
				OUTPUT << "interface not found" << endl;
				return;
			}
			BlinkyBlocks::getScheduler()->schedule(new VMSendMessageEvent(BlinkyBlocks::getScheduler()->now(), bb,
					new VMDataMessage(hostBlock->blockId, message), interface));
			}
			break;
		case VM_MESSAGE_DEBUG:
			{
			// Copy the message because it will be queued
			uint64_t *m = new uint64_t[message[0]+sizeof(uint64_t)];
			memcpy(m, message, message[0]+sizeof(uint64_t));
			handleDebugMessage(m);
			}
			break;
		case VM_MESSAGE_END_COMPUTATION:
			// format: <size> <command> <timestamp> <src> <hasNoWork>
			computing = false;
			hasWork = (bool) message[4];
			endComputingTime = message[2];
			break;
		default:
			ERRPUT << "*** ERROR *** : unsupported message received from VM (" << message[1] <<")" << endl;
			break;
	}
}

bool Blinky01BlockCode::mustBeQueued() {
	BlinkyBlocksBlock *bb = (BlinkyBlocksBlock*) hostBlock;
	uint64_t* message = bb->vm->getBufferPtr()->message;
	if(hostBlock->getState() == BlinkyBlocksBlock::COMPUTING) {
		return message[1] == VM_MESSAGE_SEND_MESSAGE or message[1] ==  VM_MESSAGE_SET_COLOR;
	} else {
		return false;
	}
}

void Blinky01BlockCode::handleDeterministicMode(){
	if(BlinkyBlocks::getScheduler()->getMode() == SCHEDULER_MODE_FASTEST && !hasWork) {
		hasWork = true;
		BlinkyBlocks::getScheduler()->schedule(new VMStartComputationEvent(std::max(BaseSimulator::getScheduler()->now(), endComputingTime)+1, (BlinkyBlocksBlock*)hostBlock, 50));
	}
}
void Blinky01BlockCode::processLocalEvent(EventPtr pev) {
	stringstream info;
	BlinkyBlocksBlock *bb = (BlinkyBlocksBlock*) hostBlock;
	
	info.str("");
	
	OUTPUT << "Blinky01BlockCode: process event " << pev->getEventName() << "(" << pev->eventType << ")" << endl;
	//cout << "Blinky01BlockCode: "<< bb->blockId << " process event " << pev->getEventName() << "(" << pev->eventType << ")" << endl;

	switch (pev->eventType) {
		case EVENT_SET_ID:
			{
			uint64_t message[4];
			message[0] = 3*sizeof(uint64_t);	
			message[1] = VM_MESSAGE_SET_ID;
			message[2] = BaseSimulator::getScheduler()->now(); // timestamp
			message[3] = hostBlock->blockId;
			bb->vm->sendMessage(4*sizeof(uint64_t), message);
			OUTPUT << "ID sent to the VM " << hostBlock->blockId << endl;
			info << "ID sent";
			}
			break;
		case EVENT_SET_DETERMINISTIC:
			{
			uint64_t message[5];
			message[0] = 4*sizeof(uint64_t);	
			message[1] = VM_SET_DETERMINISTIC_MODE;
			message[2] = BaseSimulator::getScheduler()->now(); // timestamp
			message[3] = hostBlock->blockId;
			message[4] = BlinkyBlocks::getScheduler()->getMode();
			bb->vm->sendMessage(5*sizeof(uint64_t), message);
			OUTPUT << "VM set in deterministic mode " << hostBlock->blockId << endl;
			info << "VM set in deterministic mode";
			}
			break;
		case EVENT_STOP:
			{
			if(bb->vm == NULL) {return;}
			if(BlinkyBlocksVM::isInDebuggingMode()) {
				getDebugger()->sendTerminateMsg(bb->blockId);
			} else {
				uint64_t message[4];
				message[0] = 3*sizeof(uint64_t);	
				message[1] = VM_MESSAGE_STOP;
				message[2] = BaseSimulator::getScheduler()->now();
				message[3] = hostBlock->blockId;
				bb->vm->sendMessage(4*sizeof(uint64_t), message);
			}
			bb->stopVM();
			info << "VM stopped";
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
			handleDeterministicMode();
			info << "Add neighbor "<< message[4] << " at face " << BlinkyBlocks::NeighborDirection::getString(BlinkyBlocks::NeighborDirection::getOpposite(message[5]));
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
			handleDeterministicMode();
			info << "Remove neighbor at face " << BlinkyBlocks::NeighborDirection::getString(BlinkyBlocks::NeighborDirection::getOpposite(message[4]));
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
			handleDeterministicMode();
			info << "tapped";
			}
			break;
		case EVENT_SET_COLOR:
			{
			Vecteur color = (boost::static_pointer_cast<VMSetColorEvent>(pev))->color;
			bb->setColor(color);
			//cout << bb->blockId << "SET_COLOR_EVENT" << endl;
			info << "set color "<< color << endl;
			}
			break;
		case EVENT_SEND_MESSAGE:
			{
			VMDataMessage *message = new VMDataMessage((VMDataMessage*)(boost::static_pointer_cast<VMSendMessageEvent>(pev))->message.get());
			P2PNetworkInterface *interface = (boost::static_pointer_cast<VMSendMessageEvent>(pev))->sourceInterface;
			BlinkyBlocks::getScheduler()->schedule(new NetworkInterfaceEnqueueOutgoingEvent(BaseSimulator::getScheduler()->now(),
				message, interface));
			info << "sends a message at face " << NeighborDirection::getString(bb->getDirection(interface))  << " to " << interface->connectedInterface->hostBlock->blockId;
			}
			break;
		case EVENT_RECEIVE_MESSAGE: /*EVENT_NI_RECEIVE: */
			{
			VMDataMessage *m = (VMDataMessage*) (boost::static_pointer_cast<NetworkInterfaceReceiveEvent>(pev))->message.get();
			bb->vm->sendMessage(m->size(), m->message);
			//cout << "message received from " << m->sourceInterface->hostBlock->blockId << endl;
			handleDeterministicMode();			
			info << "message received at face " << NeighborDirection::getString(bb->getDirection(m->sourceInterface->connectedInterface)) << " from " << m->sourceInterface->hostBlock->blockId;
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
			handleDeterministicMode();
			info << "accel";
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
			handleDeterministicMode();
			info << "shake";
			}
			break;
		case EVENT_VM_START_COMPUTATION:
			{
			uint64_t message[5];
			uint64_t duration;

			computing = true;
			bb->setState(BlinkyBlocksBlock::COMPUTING);
			duration = (boost::static_pointer_cast<VMStartComputationEvent>(pev))->duration;
			
			message[0] = 4*sizeof(uint64_t);	
			message[1] = VM_MESSAGE_START_COMPUTATION;
			message[2] = BlinkyBlocks::getScheduler()->now(); // timestamp
			message[3] = bb->blockId; // souce node
			message[4] = duration;
			bb->vm->sendMessage(5*sizeof(uint64_t), message);
			//availabilityDate = BlinkyBlocks::getScheduler()->now()+duration;
			
			info << "starting computation (will last for " << duration << ")" ;
			//cout << "blinky01BlockCode " << bb->blockId << " starting computation (will last for " << duration << ")" << endl;
			BlinkyBlocks::getScheduler()->schedule(new VMExpectedEndComputationEvent(BlinkyBlocks::getScheduler()->now()+duration, bb));
			}
			break;
		case EVENT_VM_EXPECTED_END_COMPUTATION:
			{
			info.str("");
			//cout << "blinky01BlockCode (" << BlinkyBlocks::getScheduler()->now() << ") " << bb->blockId << " wait for end message " << endl;		
			while (computing) {
					//sleep(1);
					BlinkyBlocks::waitForOneVMMessage();
					//BlinkyBlocks::checkForReceivedVMMessages();
			}
			info << "finished its computation";
			//cout << "blinky01BlockCode (" << BlinkyBlocks::getScheduler()->now() << ") " << " finished computation at " << endComputingTime << endl;
			BlinkyBlocks::getScheduler()->schedule(new VMEffectiveEndComputationEvent(endComputingTime+1, bb));
			}
			break;
		case EVENT_VM_EFFECTIVE_END_COMPUTATION:
			{
			//cout << "blinky01BlockCode (" << BlinkyBlocks::getScheduler()->now() << ") " << " effective end of computation " << endl;
			bb->setState(BlinkyBlocksBlock::ALIVE);
			bb->vm->handleQueuedMessages();
			if(hasWork)
				BlinkyBlocks::getScheduler()->schedule(new VMStartComputationEvent(BaseSimulator::getScheduler()->now()+1, bb, 50));
			}
			info << "effective end of computation";
			break;
		default:
			ERRPUT << "*** ERROR *** : unknown local event" << endl;
			break;
		}
		BlinkyBlocks::getScheduler()->trace(info.str(),hostBlock->blockId);
}

BlinkyBlocks::BlinkyBlocksBlockCode* Blinky01BlockCode::buildNewBlockCode(BlinkyBlocksBlock *host) {
	return(new Blinky01BlockCode(host));
}
