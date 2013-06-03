/*
 * flavio01BlockCode.cpp
 *
 *  Created on: 15 févr. 2013
 *      Author: dom
 */

#include <iostream>
#include <sstream>
#include "flavio01BlockCode.h"
#include "scheduler.h"

using namespace std;

Flavio01BlockCode::Flavio01BlockCode(MultiCoresBlock *host):MultiCoresBlockCode(host) {
	cout << "Flavio01BlockCode constructor" << endl;
	computing = false;
	waitingForVM = true;
}

Flavio01BlockCode::~Flavio01BlockCode() {
	cout << "Flavio01BlockCode destructor" << endl;
}

void Flavio01BlockCode::startup() {
	stringstream info;

	info << "  Starting Flavio01BlockCode in block " << hostBlock->blockID;
	Scheduler::trace(info.str());
}


void Flavio01BlockCode::processLocalEvent(EventPtr pev) {
	MessagePtr message;
	stringstream info;

	//info << "FlavioBlockCode processing a local event : " << pev->getEventName();
	//Scheduler::trace(info.str());

	switch (pev->eventType) {
	case EVENT_NI_RECEIVE:
		{
			unsigned int sourceId;
			message = (boost::static_pointer_cast<NetworkInterfaceReceiveEvent>(pev))->message;
			sourceId = message->sourceInterface->block->blockID;
			info.str("");
			info << "Block " << hostBlock->blockID << " received a message from " << sourceId;
			info << "   size : " << message->size();
			Scheduler::trace(info.str());
			VMMessage response;
			response.messageType = VM_MESSAGE_TYPE_RECEIVE_MESSAGE;
			response.param1 = hostBlock->blockID;
			response.param2 = Scheduler::now();
			response.param3 = message->size();
			((MultiCoresBlock*)hostBlock)->setUndefinedState(true);
			Scheduler::addUndefinedBlock(hostBlock->blockID);
			VM_TRACE_MESSAGE(response);
			Scheduler::sendMessageToVM(response);
			waitingForVM = true;
		}
		break;
	case EVENT_VM_END_COMPUTATION:
		{
			computing = false;
			//boost::shared_ptr<EndComputationEvent>endComputationEvent = (boost::static_pointer_cast<EndComputationEvent>(pev));
			info.str("");
			info << "FlavioBlockCode " << hostBlock->blockID << " finished its computation";
			Scheduler::trace(info.str());
			VMMessage response;
			response.messageType = VM_MESSAGE_TYPE_COMPUTATION_UNLOCK;
			response.param1 = hostBlock->blockID;
			response.param2 = Scheduler::now();
			((MultiCoresBlock*)hostBlock)->setUndefinedState(true);
			Scheduler::addUndefinedBlock(hostBlock->blockID);
			VM_TRACE_MESSAGE(response);
			Scheduler::sendMessageToVM(response);
			waitingForVM = true;
		}
		break;
	case EVENT_VM_START_COMPUTATION:
		uint64_t duration;
		if (computing) {
			info.str("");
			info << "*** ERROR *** block " << hostBlock->blockID << " got a COMPUTATION_LOCK from VM but it was already computing !";
			Scheduler::trace(info.str());
		}
		if (!waitingForVM) {
			info.str("");
			info << "*** ERROR *** block " << hostBlock->blockID << " got a COMPUTATION_LOCK from VM but was not waiting for one";
			Scheduler::trace(info.str());
		}
		computing = true;
		waitingForVM = false;
		duration = (boost::static_pointer_cast<VMStartComputationEvent>(pev))->duration;
		availabilityDate = Scheduler::now()+duration;
		info.str("");
		info << "FlavioBlockCode " << hostBlock->blockID << " starting computation (will last for " << duration << ")" ;
		Scheduler::trace(info.str());
		Scheduler::schedule(new VMEndComputationEvent(Scheduler::now()+duration, hostBlock));
		break;
	case EVENT_VM_START_TRANSMISSION:
		{
			unsigned int destId = (boost::static_pointer_cast<VMStartTransmissionEvent>(pev))->destBlockId;
			unsigned int messageSize = (boost::static_pointer_cast<VMStartTransmissionEvent>(pev))->messageSize;
			info.str("");
			info << "FlavioBlockCode " << hostBlock->blockID << " was asked to start transmitting to " << destId;
			Scheduler::trace(info.str());

			P2PNetworkInterface *interface;
			interface = hostBlock->getP2PNetworkInterfaceByDestBlockId(destId);
			Scheduler::schedule(new NetworkInterfaceEnqueueOutgoingEvent(Scheduler::now(),new VMDataMessage(messageSize), interface));
		}
		break;
	default:
		break;
	}
}

Flavio01BlockCode *Flavio01BlockCode::buildNewBlockCode(MultiCoresBlock *host) {
	return(new Flavio01BlockCode(host));
}

