/*
 * blinky03BlockCode.cpp
 *
 *  Created on: 26 mars 2033
 *      Author: dom
 */

#include <iostream>
#include <sstream>
#include <boost/asio.hpp> 
#include "scheduler.h"
#include "network.h"
#include "blinky03BlockCode.h"
#include "blinkyBlocksEvents.h"
#include "blinkyBlocksDebugger.h"
#include "trace.h"

using namespace std;
using namespace BlinkyBlocks;
using boost::asio::ip::tcp;

Blinky03BlockCode::Blinky03BlockCode(BlinkyBlocksBlock *host): BlinkyBlocksBlockCode(host) {
	OUTPUT << "Blinky03BlockCode constructor" << endl;
	// to make the fastest mode working without any meld program :
	hasWork = false;
	polling = true;
	currentLocalDate = 0;
}

Blinky03BlockCode::~Blinky03BlockCode() {
	OUTPUT << "Blinky03BlockCode destructor" << endl;
}

void Blinky03BlockCode::init() {}

void Blinky03BlockCode::startup() {
	stringstream info;
	currentLocalDate = BaseSimulator::getScheduler()->now();
	BlinkyBlocksBlock *bb = (BlinkyBlocksBlock*) hostBlock;
	commandType c[5];
	BlinkyBlocksVM *vm;
	
	info << "  Starting Blinky03BlockCode in block " << hostBlock->blockId;
		
	bb->lockVM();
	vm = bb->vm;
	if((vm != NULL)) {
		if(!vm->idSent) {
			vm->idSent = true;
			SetIdVMCommand idCommand(c, bb->blockId);	
			vm->sendCommand(idCommand);
		}
	}
	bb->unlockVM();
	
	BlinkyBlocks::getScheduler()->trace(info.str(),hostBlock->blockId);
	
	if (hostBlock->blockId == 1) {
		BlinkyBlocks::getScheduler()->schedule(new VMSetColorEvent(BaseSimulator::getScheduler()->now(), (BlinkyBlocksBlock*) hostBlock, 1.0,0.0,0.0,1.0));
	} else {
		BlinkyBlocks::getScheduler()->schedule(new VMSetColorEvent(BaseSimulator::getScheduler()->now()+250, (BlinkyBlocksBlock*) hostBlock, 0.0,0.0,1.0,1.0));
	}
}

void Blinky03BlockCode::handleCommand(VMCommand &command) {}

void Blinky03BlockCode::handleDeterministicMode(VMCommand &command){}

void Blinky03BlockCode::processLocalEvent(EventPtr pev) {
	stringstream info;
	BlinkyBlocksBlock *bb = (BlinkyBlocksBlock*) hostBlock;
	
	OUTPUT << bb->blockId << " processLocalEvent: date: "<< BaseSimulator::getScheduler()->now() << " process event " << pev->getEventName() << "(" << pev->eventType << ")" << ", random number : " << pev->randomNumber << endl;
	info << "";
	switch (pev->eventType) {
		case EVENT_STOP:
		case EVENT_ADD_NEIGHBOR:
		case EVENT_REMOVE_NEIGHBOR:
		case EVENT_SET_DETERMINISTIC:
		case EVENT_END_POLL: 
		break;
		
		case EVENT_TAP:
			info << "tapped";
			break;
		case EVENT_SET_COLOR:
			{
			Vecteur color = (boost::static_pointer_cast<VMSetColorEvent>(pev))->color;
			bb->setColor(color);
			info << "set color "<< color;
			}
			break;
		case EVENT_SEND_MESSAGE:
			{
			MessagePtr message = (boost::static_pointer_cast<VMSendMessageEvent>(pev))->message;
			P2PNetworkInterface *interface = (boost::static_pointer_cast<VMSendMessageEvent>(pev))->sourceInterface;
			BlinkyBlocks::getScheduler()->schedule(new NetworkInterfaceEnqueueOutgoingEvent(BaseSimulator::getScheduler()->now(),
			message, interface));
			info << "sends a message at face " << NeighborDirection::getString(bb->getDirection(interface))  << " to " << interface->connectedInterface->hostBlock->blockId;
			}
			break;
		case EVENT_RECEIVE_MESSAGE: /*EVENT_NI_RECEIVE: */
			{
			ReceiveMessageVMCommand *command = (ReceiveMessageVMCommand*) (boost::static_pointer_cast<NetworkInterfaceReceiveEvent>(pev))->message.get();
			info << "message received at face " << NeighborDirection::getString(bb->getDirection(command->sourceInterface->connectedInterface)) << " from " << command->sourceInterface->hostBlock->blockId;
			}
			break;
		case EVENT_ACCEL:
			{
			//AccelVMCommand command(outBuffer, bb->blockId, (boost::static_pointer_cast<VMAccelEvent>(pev))->x, (boost::static_pointer_cast<VMAccelEvent>(pev))->y,
			//(boost::static_pointer_cast<VMAccelEvent>(pev))->z);
			info << "accel";
			}
			break;
		case EVENT_SHAKE:
			{
			//ShakeVMCommand command(outBuffer, bb->blockId, (boost::static_pointer_cast<VMShakeEvent>(pev))->force);
			info << "shake";
			}
			break;
		default:
			ERRPUT << "*** ERROR *** : unknown local event" << endl;
			break;
		}
		if (info.str() != "" ) {
			BlinkyBlocks::getScheduler()->trace(info.str(),hostBlock->blockId);
		}
}

BlinkyBlocks::BlinkyBlocksBlockCode* Blinky03BlockCode::buildNewBlockCode(BlinkyBlocksBlock *host) {
	return(new Blinky03BlockCode(host));
}
