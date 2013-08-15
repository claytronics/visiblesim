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

Blinky01BlockCode::Blinky01BlockCode(BlinkyBlocksBlock *host): BlinkyBlocksBlockCode(host) {
	OUTPUT << "Blinky01BlockCode constructor" << endl;
	hasWork = true; // mode fastest 1 & 2
	willHaveWork = true; // mode fastest 2
	computing = false; // mode fastest 2
	generator = boost::rand48(hostBlock->blockId); // mode fastest 2
}

int Blinky01BlockCode::getRandom() {
	int x = 0;
	do {
		x = generator() %  300 + 35;
	} while (x 	< 35);
	return x;
}

Blinky01BlockCode::~Blinky01BlockCode() {
	OUTPUT << "Blinky01BlockCode destructor" << endl;
}

void Blinky01BlockCode::startup() {	
	BlinkyBlocksBlock *bb = (BlinkyBlocksBlock*) hostBlock;
	stringstream info;
	currentLocalDate = BaseSimulator::getScheduler()->now();
	info << "  Starting Blinky01BlockCode in block " << hostBlock->blockId;
	BlinkyBlocks::getScheduler()->trace(info.str(),hostBlock->blockId);
	if (BlinkyBlocks::getScheduler()->getMode() == SCHEDULER_MODE_FASTEST_2) {
		BlinkyBlocks::getScheduler()->schedule(new VMResumeComputationEvent(BaseSimulator::getScheduler()->now()+1, bb, getRandom()));
	}
}

void Blinky01BlockCode::handleCommand(VMCommand &command) {
	BlinkyBlocksBlock *bb = (BlinkyBlocksBlock*) hostBlock;
	uint64_t dateToSchedule;
	
	OUTPUT << "Blinky01BlockCode: type: " << VMCommand::getString(command.getType()) << " size: " << command.getSize() << endl;
	//cout << bb->blockId << " message received: date: " <<command.getTimestamp() << ", type: " << VMCommand::getString(command.getType()) << endl;
	//assert(hasWork); // mode 1
	
	currentLocalDate = command.getTimestamp();
	
	if (BlinkyBlocks::getScheduler()->getMode() == SCHEDULER_MODE_FASTEST_1) {
		dateToSchedule = currentLocalDate;
	} else {
		dateToSchedule = BlinkyBlocks::getScheduler()->now();
	}
	
	switch (command.getType()) {
		case VM_COMMAND_SET_COLOR:	
			{
			// format: <size> <command> <timestamp> <src> <red> <blue> <green> <intensity>
			SetColorVMCommand c(command.getData());
			Vecteur color = c.getColor();
			BlinkyBlocks::getScheduler()->schedule(new VMSetColorEvent(dateToSchedule, bb, color));
			}
			break;
		case VM_COMMAND_SEND_MESSAGE:
			{
			P2PNetworkInterface *interface;
			SendMessageVMCommand c(command.getData());
			interface = bb->getInterfaceDestId(c.getDestId());
			if (interface == NULL) {
				ERRPUT << "Interface not found" << endl;
				return;
			}
			BlinkyBlocks::getScheduler()->schedule(new VMSendMessageEvent(dateToSchedule, bb,
					new ReceiveMessageVMCommand(c), interface));
			}
			break;
		case VM_COMMAND_DEBUG:
			{
			// Copy the message because it will be queued
			command.copyData();
			handleDebugCommand(command.getData());
			}
			break;
		case VM_COMMAND_COMPUTATION_PAUSE:
			{
			ComputationPauseVMCommand c(command.getData());
			computing = false;
			endComputingTime = c.getTimestamp();
			}
			break;
		case VM_COMMAND_WORK_END:
			{
			WorkEndVMCommand c(command.getData());
			if (BlinkyBlocks::getScheduler()->getMode() == SCHEDULER_MODE_FASTEST_2) {
				computing = false;
				if (c.getNbProcessedMsg() == bb->vm->nbSentCommands) {
					willHaveWork = false;
				}
				endComputingTime = c.getTimestamp();		
			} else {
				if (c.getNbProcessedMsg() == bb->vm->nbSentCommands) {
					hasWork = false;
				}
			}
			}
			break;
		case VM_COMMAND_TIME_INFO:
			;
			break;
		default:
			ERRPUT << "*** ERROR *** : unsupported message received from VM (" << command.getType() <<")" << endl;
			break;
	}
}

bool Blinky01BlockCode::mustBeQueued(VMCommand &command) {
	if(hostBlock->getState() == BlinkyBlocksBlock::COMPUTING) {
		return command.getType() == VM_COMMAND_SEND_MESSAGE or command.getType() ==  VM_COMMAND_SET_COLOR;
	} else {
		return false;
	}
}

void Blinky01BlockCode::handleDeterministicMode(VMCommand &command){
	currentLocalDate = max(BaseSimulator::getScheduler()->now(), currentLocalDate);
	if(!hasWork && command.getType() != VM_COMMAND_STOP && command.getType() != VM_COMMAND_RESUME_COMPUTATION) {
		hasWork = true;
#ifdef TEST_DETER
		//cout << hostBlock->blockId << " has work again at " << BaseSimulator::getScheduler()->now() << endl;
#endif
		if (BlinkyBlocks::getScheduler()->getMode() == SCHEDULER_MODE_FASTEST_2) {
			BlinkyBlocks::getScheduler()->schedule(new VMResumeComputationEvent(std::max(BaseSimulator::getScheduler()->now(), endComputingTime), (BlinkyBlocksBlock*)hostBlock, getRandom()));
		}
	}
}
void Blinky01BlockCode::processLocalEvent(EventPtr pev) {
	stringstream info;
	BlinkyBlocksBlock *bb = (BlinkyBlocksBlock*) hostBlock;
	BlinkyBlocksVM *vm = bb->vm;

	assert(vm != NULL);
	
	info.str("");
	
	OUTPUT << "Blinky01BlockCode: process event " << pev->getEventName() << "(" << pev->eventType << ")" << endl;
	//cout << bb->blockId << " processLocalEvent: date: "<< BaseSimulator::getScheduler()->now() << " process event " << pev->getEventName() << "(" << pev->eventType << ")" << ", random number : " << pev->randomNumber << endl;

#ifdef TEST_DETER
	cout << bb->blockId << " processLocalEvent: date: "<< BaseSimulator::getScheduler()->now() << " process event " << pev->getEventName() << "(" << pev->eventType << ")" << ", random number : " << pev->randomNumber << endl;
#endif
	switch (pev->eventType) {
		case EVENT_SET_ID:
			{
			commandType c [5];			
			switch(BlinkyBlocks::getScheduler()->getMode()) {
				case SCHEDULER_MODE_FASTEST_1:
				case SCHEDULER_MODE_FASTEST_2:
				{
					SetDeterministicModeVMCommand determinismCommand(c, bb->blockId, BlinkyBlocks::getScheduler()->getMode());
					vm->sendCommand(determinismCommand);
				}
				break;
				default:
				break;
			}			
			SetIdVMCommand idCommand(c, bb->blockId);	
			vm->sendCommand(idCommand);
			OUTPUT << "ID sent to the VM " << hostBlock->blockId << endl;
			info << "ID sent";
			}
			break;
		case EVENT_STOP:
			{
			if(BlinkyBlocksVM::isInDebuggingMode()) {
				//getDebugger()->sendTerminateMsg(bb->blockId);
			} else {
				StopVMCommand command(vm->outBuffer, bb->blockId);
				vm->sendCommand(command);
			}
			bb->stopVM();
			info << "VM stopped";
			}
			break;
		case EVENT_ADD_NEIGHBOR:
			{
			AddNeighborVMCommand command(vm->outBuffer, bb->blockId, (boost::static_pointer_cast<VMAddNeighborEvent>(pev))->target,
				(boost::static_pointer_cast<VMAddNeighborEvent>(pev))->face);
			vm->sendCommand(command);
			info << "Add neighbor "<< (boost::static_pointer_cast<VMAddNeighborEvent>(pev))->target << " at face " << BlinkyBlocks::NeighborDirection::getString(BlinkyBlocks::NeighborDirection::getOpposite((boost::static_pointer_cast<VMAddNeighborEvent>(pev))->face));
			}
			break;
		case EVENT_REMOVE_NEIGHBOR:
			{
			RemoveNeighborVMCommand command(vm->outBuffer, bb->blockId, (boost::static_pointer_cast<VMRemoveNeighborEvent>(pev))->face);
			vm->sendCommand(command);
			info << "Remove neighbor at face " << BlinkyBlocks::NeighborDirection::getString(BlinkyBlocks::NeighborDirection::getOpposite((boost::static_pointer_cast<VMAddNeighborEvent>(pev))->face));
			}
			break;
		case EVENT_TAP:
			{
			TapVMCommand command(vm->outBuffer, bb->blockId);
			vm->sendCommand(command);
			info << "tapped";
			}
			break;
		case EVENT_SET_COLOR:
			{
			Vecteur color = (boost::static_pointer_cast<VMSetColorEvent>(pev))->color;
			bb->setColor(color);
#ifdef TEST_DETER
			cout << bb->blockId << " SET_COLOR_EVENT" << endl;
#endif
			info << "set color "<< color << endl;
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
			command->setTimestamp(BlinkyBlocks::getScheduler()->now());
			vm->sendCommand(*command);
#ifdef TEST_DETER
			cout << "message received from " << command->sourceInterface->hostBlock->blockId << endl;
#endif
			info << "message received at face " << NeighborDirection::getString(bb->getDirection(command->sourceInterface->connectedInterface)) << " from " << command->sourceInterface->hostBlock->blockId;
			}
			break;
		case EVENT_ACCEL:
			{
			AccelVMCommand command(vm->outBuffer, bb->blockId, (boost::static_pointer_cast<VMAccelEvent>(pev))->x, (boost::static_pointer_cast<VMAccelEvent>(pev))->y,
			(boost::static_pointer_cast<VMAccelEvent>(pev))->z);
			vm->sendCommand(command);
			info << "accel";
			}
			break;
		case EVENT_SHAKE:
			{
			ShakeVMCommand command(vm->outBuffer, bb->blockId, (boost::static_pointer_cast<VMShakeEvent>(pev))->force);
			vm->sendCommand(command);
			info << "shake";
			}
			break;
		case EVENT_SET_DETERMINISTIC:
			{
			SetDeterministicModeVMCommand command(vm->outBuffer, bb->blockId, BlinkyBlocks::getScheduler()->getMode());
			vm->sendCommand(command);
			OUTPUT << "VM set in deterministic mode " << hostBlock->blockId << endl;
			info << "VM set in deterministic mode";
			}
			break;
		case EVENT_VM_RESUME_COMPUTATION:
			{
			uint64_t duration = (boost::static_pointer_cast<VMResumeComputationEvent>(pev))->duration;

			computing = true;
			bb->setState(BlinkyBlocksBlock::COMPUTING);			
			ResumeComputationVMCommand command(vm->outBuffer, bb->blockId, duration);
			vm->sendCommand(command);
			//availabilityDate = BlinkyBlocks::getScheduler()->now()+duration;
			
//			info << "starting computation (will last for " << duration << ")" ;
#ifdef TEST_DETER
//			cout << "blinky01BlockCode " << bb->blockId << " starting computation (will last for " << duration << ")" << endl;
#endif
			BlinkyBlocks::getScheduler()->schedule(new VMExpectedComputationPauseEvent(BlinkyBlocks::getScheduler()->now()+duration, bb));
			}
			break;
		case EVENT_VM_EXPECTED_COMPUTATION_PAUSE:
			{
//			info.str("");
			//cout << "blinky01BlockCode (" << BlinkyBlocks::getScheduler()->now() << ") " << bb->blockId << " wait for end message " << endl;		
			while (computing) {
					BlinkyBlocks::waitForOneVMCommand();
			}
//			info << "finished its computation";
#ifdef TEST_DETER
//			cout << "blinky01BlockCode (" << BlinkyBlocks::getScheduler()->now() << ") " << " finished computation at " << endComputingTime << endl;
#endif
			BlinkyBlocks::getScheduler()->schedule(new VMEffectiveComputationPauseEvent(max(BlinkyBlocks::getScheduler()->now(), endComputingTime), bb));
			}
			break;
		case EVENT_VM_EFFECTIVE_COMPUTATION_PAUSE:
			{
			//cout << "blinky01BlockCode (" << BlinkyBlocks::getScheduler()->now() << ") " << " effective end of computation " << endl;
			bb->setState(BlinkyBlocksBlock::ALIVE);
			vm->handleQueuedCommands();
			hasWork = willHaveWork;
			if(hasWork) {
				BlinkyBlocks::getScheduler()->schedule(new VMResumeComputationEvent(BaseSimulator::getScheduler()->now()+1, bb, getRandom()));
			}
			//info << "effective end of computation";
			}
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
