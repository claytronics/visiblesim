/*
 * network.cpp
 *
 *  Created on: 24 mars 2013
 *      Author: dom
 */

#include <iostream>
#include <sstream>

using namespace std;

#include "scheduler.h"
#include "network.h"

unsigned int Message::nextId = 0;
unsigned int Message::nbMessages = 0;

unsigned int P2PNetworkInterface::nextId = 0;
unsigned int P2PNetworkInterface::defaultDataRate=1000000;

//===========================================================================================================
//
//          Message  (class)
//
//===========================================================================================================

Message::Message() {
	id = nextId;
	nextId++;
	nbMessages++;
	MESSAGE_CONSTRUCTOR_INFO();
}

Message::~Message() {
	MESSAGE_DESTRUCTOR_INFO();
	nbMessages--;
}

unsigned int Message::getNbMessages() {
	return(nbMessages);
}

string Message::getMessageName() {
	return("generic message");
}


//===========================================================================================================
//
//          P2PNetworkInterface  (class)
//
//===========================================================================================================

P2PNetworkInterface::P2PNetworkInterface(BaseSimulator::BuildingBlock *b) {
#ifndef NDEBUG
	cout << "P2PNetworkInterface constructor" << endl;
#endif
	hostBlock = b;
//	localId = block->getNextP2PInterfaceLocalId();
	connectedInterface = NULL;
	availabilityDate = 0;
	globalId = nextId;
	nextId++;
//	messageBeingTransmitted.reset();
	dataRate = defaultDataRate;
}

P2PNetworkInterface::~P2PNetworkInterface() {
#ifndef NDEBUG
	cout << "P2PNetworkInterface destructor" << endl;
#endif
}

bool P2PNetworkInterface::addToOutgoingBuffer(MessagePtr msg) {
	stringstream info;

	if (connectedInterface != NULL) {
		outgoingQueue.push_back(msg);
		if (availabilityDate < BaseSimulator::getScheduler()->now()) availabilityDate = BaseSimulator::getScheduler()->now();
		if (outgoingQueue.size() == 1 && messageBeingTransmitted == NULL) { //TODO
			BaseSimulator::getScheduler()->schedule(new NetworkInterfaceStartTransmittingEvent(availabilityDate,this));
		}
		return(true);
	} else {
		info.str("");
		info << "*** WARNING *** [block " << hostBlock->blockId << ",interface " << globalId <<"] : trying to enqueue a Message but no interface connected";
		BaseSimulator::getScheduler()->trace(info.str());
		return(false);
	}
}

void P2PNetworkInterface::send() {
	MessagePtr msg;
	stringstream info;
	uint64_t transmissionDuration;

	if (!connectedInterface) {
		info << "*** WARNING *** [block " << hostBlock->blockId << ",interface " << globalId <<"] : trying to send a Message but no interface connected";
		BaseSimulator::getScheduler()->trace(info.str());
		return;
	}

	if (outgoingQueue.size()==0) {
		info << "*** ERROR *** [block " << hostBlock->blockId << ",interface " << globalId <<"] : The outgoing buffer of this interface should not be empty !";
		BaseSimulator::getScheduler()->trace(info.str());
		exit(EXIT_FAILURE);
	}

	msg = outgoingQueue.front();

	outgoingQueue.pop_front();
	transmissionDuration = (msg->size()*8000000ULL)/dataRate;

	messageBeingTransmitted = msg;
	messageBeingTransmitted->sourceInterface = this;
	messageBeingTransmitted->destinationInterface = connectedInterface;

	availabilityDate = BaseSimulator::getScheduler()->now()+transmissionDuration;
	//info << "*** sending (interface " << localId << " of block " << hostBlock->blockId << ")";
	//getScheduler()->trace(info.str());

	BaseSimulator::getScheduler()->schedule(new NetworkInterfaceStopTransmittingEvent(BaseSimulator::getScheduler()->now()+transmissionDuration, this));
}

void P2PNetworkInterface::connect(P2PNetworkInterface *ni) {
	// test ajouté par Ben, gestion du cas : connect(NULL)
	if (ni) {
		if (ni->connectedInterface != NULL) {
			cout << "ERROR : connecting to an already connected P2PNetwork interface" << endl;
		}
		ni->connectedInterface = this;
	}
	connectedInterface = ni;
}
