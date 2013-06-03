/*
 * buildingBlock.cpp
 *
 *  Created on: 15 févr. 2013
 *      Author: dom
 */

#include <iostream>
#include <sstream>

using namespace std;

#include "buildingBlock.h"
#include "blockCode.h"
#include "scheduler.h"
#include "events.h"

unsigned int P2PNetworkInterface::nextId = 0;
unsigned int P2PNetworkInterface::defaultDataRate=1000000;

unsigned int BuildingBlock::nextID = 0;
vector<BuildingBlock*> BuildingBlock::buildingBlocksVector;


//===========================================================================================================
//
//          P2PNetworkInterface  (class)
//
//===========================================================================================================

P2PNetworkInterface::P2PNetworkInterface(BuildingBlock *b) {
#ifndef NDEBUG
	cout << "P2PNetworkInterface constructor" << endl;
#endif
	block = b;
	localId = block->getNextP2PInterfaceLocalId();
	connectedInterface = NULL;
	availabilityDate = 0;
	id = nextId;
	nextId++;
	messageBeingTransmitted.reset();
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
		if (availabilityDate < Scheduler::now()) availabilityDate = Scheduler::now();
		if (outgoingQueue.size() == 1 && messageBeingTransmitted == NULL) { //TODO
			Scheduler::schedule(new NetworkInterfaceStartTransmittingEvent(availabilityDate,this));
		}
		return(true);
	} else {
		info.str("");
		info << "*** WARNING *** [block " << block->blockID << ",interface " << id <<"] : trying to enqueue a Message but no interface connected";
		Scheduler::trace(info.str());
		return(false);
	}
}

void P2PNetworkInterface::send() {
	MessagePtr msg;
	stringstream info;
	uint64_t transmissionDuration;

	if (!connectedInterface) {
		info << "*** WARNING *** [block " << block->blockID << ",interface " << id <<"] : trying to send a Message but no interface connected";
		Scheduler::trace(info.str());
		return;
	}

	if (outgoingQueue.size()==0) {
		info << "*** ERROR *** [block " << block->blockID << ",interface " << id <<"] : The outgoing buffer of this interface should not be empty !";
		Scheduler::trace(info.str());
		exit(EXIT_FAILURE);
	}
	msg = outgoingQueue.front();
	outgoingQueue.pop_front();

	transmissionDuration = (msg->size()*8000000ULL)/dataRate;

	messageBeingTransmitted = msg;
	messageBeingTransmitted->sourceInterface = this;
	messageBeingTransmitted->destinationInterface = connectedInterface;

	availabilityDate = Scheduler::now()+transmissionDuration;
	//info << "*** sending (interface " << localId << " of block " << block->blockID << ")";
	//Scheduler::trace(info.str());

	Scheduler::schedule(new NetworkInterfaceStopTransmittingEvent(Scheduler::now()+transmissionDuration, this));
	//Scheduler::schedule(new EvenementInterfaceReseauTermineEnvoi(Scheduler::now()+transmissionDuration,this,msg));
}

void P2PNetworkInterface::connect(P2PNetworkInterface *ni) {
	connectedInterface = ni;
	if (ni->connectedInterface != NULL) {
		cout << "ERROR : connecting to an already connected P2PNetwork interface" << endl;
	}
	ni->connectedInterface = this;
}


//===========================================================================================================
//
//          BuildingBlock  (class)
//
//===========================================================================================================


BuildingBlock::BuildingBlock() {
	cout << "BuildingBlock constructor (id:" << nextID << ")" << endl;
	blockID = nextID;
	buildingBlocksVector.push_back(this);
	nextID++;
	P2PNetworkInterfaceNextLocalId = 0;
}

BuildingBlock::BuildingBlock(unsigned int bId) {
	cout << "BuildingBlock constructor (id:" << bId << ")" << endl;
	blockID = bId;
	buildingBlocksVector.push_back(this);
	nextID++;
	P2PNetworkInterfaceNextLocalId = 0;
}

BuildingBlock::~BuildingBlock() {
	cout << "BuildingBlock destructor" << endl;
}

void BuildingBlock::setBlockCode(BlockCode *bc) {
	blockCode = bc;
}

unsigned int BuildingBlock::getNextP2PInterfaceLocalId() {
	unsigned int id = P2PNetworkInterfaceNextLocalId;
	P2PNetworkInterfaceNextLocalId++;
	return(id);
}

BuildingBlock *BuildingBlock::getBlocByID(unsigned int bId) {
	vector<BuildingBlock*>::const_iterator it = buildingBlocksVector.begin();
//	while (it != buildingBlocksVector.end() && (*it)->blockID != bId) {
//		cout << "block in the protected blocks list : " << (*it)->blockID << endl;
//		it++;
//	}
	buildingBlocksVector.begin();
	while (it != buildingBlocksVector.end() && (*it)->blockID != bId) it++;
	return (it==buildingBlocksVector.end())?NULL:(*it);
}

bool BuildingBlock::addP2PNetworkInterface(BuildingBlock *destBlock) {
	P2PNetworkInterface *ni1, *ni2;
	ni1 = NULL;
	ni2 = NULL;
	if (!getP2PNetworkInterfaceByBlockRef(destBlock)) {
		// creation of the new network interface
		cout << "adding a new interface to block " << destBlock->blockID << endl;
		ni1 = new P2PNetworkInterface(this);
		P2PNetworkInterfaceList.push_back(ni1);
	}

	if (!destBlock->getP2PNetworkInterfaceByBlockRef(this)) {
		// creation of the new network interface
		cout << "adding a new interface to block " << this->blockID << endl;
		ni2 = new P2PNetworkInterface(destBlock);
		destBlock->P2PNetworkInterfaceList.push_back(ni2);
	}

	if (ni1!=NULL && ni2!=NULL) {
		ni1->connect(ni2);
		return (true);
	} else {
		cout << "*** ERROR *** could not connect the new interfaces" << endl;
	}
	return false;
}
bool BuildingBlock::addP2PNetworkInterface(unsigned int destBlockID) {
	// if the link is not in the list
	BuildingBlock *destBlock = BuildingBlock::getBlocByID(destBlockID);
	if (!getP2PNetworkInterfaceByBlockRef(destBlock)) {
		// creation of the new network interface
		P2PNetworkInterface* ni1 = new P2PNetworkInterface(this);
		P2PNetworkInterfaceList.push_back(ni1);
		// if the corresponding interface exists in the connected block, we link the two interfaces
		if (destBlock->addP2PNetworkInterface(this)) {
			P2PNetworkInterface* ni2 = destBlock->getP2PNetworkInterfaceByBlockRef(this);
			ni1->connect(ni2);
		}
	}
	return false;
}

P2PNetworkInterface *BuildingBlock::getP2PNetworkInterfaceByBlockRef(BuildingBlock *destBlock) {
	list <P2PNetworkInterface*>::const_iterator niIt=P2PNetworkInterfaceList.begin();
	while (niIt!=P2PNetworkInterfaceList.end() && (*niIt)->connectedInterface->block!=destBlock) niIt++;
	return (niIt==P2PNetworkInterfaceList.end())?NULL:(*niIt);
}

P2PNetworkInterface*BuildingBlock::getP2PNetworkInterfaceByDestBlockId(unsigned int destBlockID) {
	list <P2PNetworkInterface*>::const_iterator niIt=P2PNetworkInterfaceList.begin();
	while (niIt!=P2PNetworkInterfaceList.end() && (*niIt)->connectedInterface->block->blockID != destBlockID) niIt++;
	return (niIt==P2PNetworkInterfaceList.end())?NULL:(*niIt);
}

P2PNetworkInterface *BuildingBlock::getP2PNetworkInterfaceById(unsigned int id) {
	list <P2PNetworkInterface*>::const_iterator niIt=P2PNetworkInterfaceList.begin();
	while (niIt!=P2PNetworkInterfaceList.end() && (*niIt)->id != id) niIt++;
	return (niIt==P2PNetworkInterfaceList.end())?NULL:(*niIt);
}

P2PNetworkInterface *BuildingBlock::getP2PNetworkInterfaceByLocalId(unsigned int lId) {
	list <P2PNetworkInterface*>::const_iterator niIt=P2PNetworkInterfaceList.begin();
	while (niIt!=P2PNetworkInterfaceList.end() && (*niIt)->localId != lId) niIt++;
	return (niIt==P2PNetworkInterfaceList.end())?NULL:(*niIt);
}


void BuildingBlock::scheduleLocalEvent(EventPtr pev) {
	localEventsList.push_back(pev);

	if (localEventsList.size() == 1) {
		uint64_t date;
		date = this->blockCode->availabilityDate;
		if (date < Scheduler::now()) date=Scheduler::now();
		Scheduler::schedule(new ProcessLocalEvent(date,this));
	}
	return;
}

void BuildingBlock::processLocalEvent() {
	EventPtr pev;

	if (localEventsList.size() == 0) {
		Scheduler::trace("*** ERROR *** The local event list should not be empty !!");
		exit(EXIT_FAILURE);
	}
	pev = localEventsList.front();
	localEventsList.pop_front();

	blockCode->processLocalEvent(pev);

	if (blockCode->availabilityDate < Scheduler::now()) blockCode->availabilityDate = Scheduler::now();
	if (localEventsList.size() > 0) {
		Scheduler::schedule(new ProcessLocalEvent(blockCode->availabilityDate,this));
	}

}
