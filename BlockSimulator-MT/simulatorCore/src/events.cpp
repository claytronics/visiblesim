/*
 * events.cpp
 *
 *  Created on: 16 févr. 2013
 *      Author: dom
 */

#include <iostream>
#include <sstream>
#include "events.h"
#include "scheduler.h"
#include "blockCode.h"

int Event::nextId = 0;
unsigned int Event::nbEvents = 0;

using namespace std;

//===========================================================================================================
//
//          Event  (class)
//
//===========================================================================================================

Event::Event(uint64_t t) {
	id = nextId;
	nextId++;
	nbEvents++;
	date = t;
	EVENT_CONSTRUCTOR_INFO();
}

Event::Event(Event *ev) {
	id = nextId;
	nextId++;
	nbEvents++;
	date = ev->date;
	EVENT_CONSTRUCTOR_INFO();
}

Event::~Event() {
	EVENT_DESTRUCTOR_INFO();
	nbEvents--;
}

string Event::getEventName() {
	return("Generic Event");
}

unsigned int Event::getNextId() {
	return(nextId);
}

unsigned int Event::getNbEvents() {
	return(nbEvents);
}

//===========================================================================================================
//
//          CodeStartEvent  (class)
//
//===========================================================================================================

CodeStartEvent::CodeStartEvent(uint64_t t, BuildingBlock *conBlock):Event(t) {
	EVENT_CONSTRUCTOR_INFO();
	eventType = EVENT_CODE_START;
	concernedBlock = conBlock;
}
CodeStartEvent::~CodeStartEvent() {
	EVENT_DESTRUCTOR_INFO();
}

void CodeStartEvent::consume() {
	EVENT_CONSUME_INFO();
	concernedBlock->blockCode->startup();
}

string CodeStartEvent::getEventName() {
	return("CodeStart Event");
}

//===========================================================================================================
//
//          NetworkInterfaceStartTransmittingEvent  (class)
//
//===========================================================================================================

NetworkInterfaceStartTransmittingEvent::NetworkInterfaceStartTransmittingEvent(uint64_t t, P2PNetworkInterface *ni):Event(t) {
	eventType = EVENT_NI_START_TRANSMITTING;
	interface = ni;
	EVENT_CONSTRUCTOR_INFO();
}
NetworkInterfaceStartTransmittingEvent::~NetworkInterfaceStartTransmittingEvent() {
	EVENT_DESTRUCTOR_INFO();
}

void NetworkInterfaceStartTransmittingEvent::consume() {
	EVENT_CONSUME_INFO();
	interface->send();
}

string NetworkInterfaceStartTransmittingEvent::getEventName() {
	return("NetworkInterfaceStartTransmitting Event");
}

//===========================================================================================================
//
//          NetworkInterfaceStopTransmittingEvent  (class)
//
//===========================================================================================================

NetworkInterfaceStopTransmittingEvent::NetworkInterfaceStopTransmittingEvent(uint64_t t, P2PNetworkInterface *ni):Event(t) {
	eventType = EVENT_NI_STOP_TRANSMITTING;
	interface = ni;
	EVENT_CONSTRUCTOR_INFO();
}
NetworkInterfaceStopTransmittingEvent::~NetworkInterfaceStopTransmittingEvent() {
	EVENT_DESTRUCTOR_INFO();
}

void NetworkInterfaceStopTransmittingEvent::consume() {
	EVENT_CONSUME_INFO();

	interface->connectedInterface->block->scheduleLocalEvent(EventPtr(new NetworkInterfaceReceiveEvent(Scheduler::now(), interface->connectedInterface, interface->messageBeingTransmitted)));
	// TODO add a confirmation event to the sender ?

	interface->messageBeingTransmitted.reset();
	interface->availabilityDate = Scheduler::now();
	if (interface->outgoingQueue.size() > 0) {
		//cout << "one more to send !!" << endl;
		interface->send();
	}
}

string NetworkInterfaceStopTransmittingEvent::getEventName() {
	return("NetworkInterfaceStopTransmitting Event");
}

//===========================================================================================================
//
//          NetworkInterfaceReceiveEvent  (class)
//
//===========================================================================================================

NetworkInterfaceReceiveEvent::NetworkInterfaceReceiveEvent(uint64_t t, P2PNetworkInterface *ni, MessagePtr mes):Event(t) {
	eventType = EVENT_NI_RECEIVE;
	interface = ni;
	message = mes;
	EVENT_CONSTRUCTOR_INFO();
}

NetworkInterfaceReceiveEvent::~NetworkInterfaceReceiveEvent() {
	message.reset();
	EVENT_DESTRUCTOR_INFO();
}

void NetworkInterfaceReceiveEvent::consume() {
	EVENT_CONSUME_INFO();
}

string NetworkInterfaceReceiveEvent::getEventName() {
	return("NetworkInterfaceReceiveEvent Event");
}

//===========================================================================================================
//
//          NetworkInterfaceEnqueueOutgoingEvent  (class)
//
//===========================================================================================================

NetworkInterfaceEnqueueOutgoingEvent::NetworkInterfaceEnqueueOutgoingEvent(uint64_t t, Message *mes, P2PNetworkInterface *ni):Event(t) {
	eventType = EVENT_NI_ENQUEUE_OUTGOING_MESSAGE;
	message = MessagePtr(mes);
	sourceInterface = ni;
	EVENT_CONSTRUCTOR_INFO();
}

NetworkInterfaceEnqueueOutgoingEvent::~NetworkInterfaceEnqueueOutgoingEvent() {
	message.reset();
	EVENT_DESTRUCTOR_INFO();
}

void NetworkInterfaceEnqueueOutgoingEvent::consume() {
	EVENT_CONSUME_INFO();
	sourceInterface->addToOutgoingBuffer(message);
}

string NetworkInterfaceEnqueueOutgoingEvent::getEventName() {
	return("NetworkInterfaceEnqueueOutgoingEvent Event");
}

//===========================================================================================================
//
//          ProcessLocalEvent  (class)
//
//===========================================================================================================

ProcessLocalEvent::ProcessLocalEvent(uint64_t t, BuildingBlock *conBlock):Event(t) {
	EVENT_CONSTRUCTOR_INFO();
	eventType = EVENT_PROCESS_LOCAL_EVENT;
	concernedBlock = conBlock;
}
ProcessLocalEvent::~ProcessLocalEvent() {
	EVENT_DESTRUCTOR_INFO();
}

void ProcessLocalEvent::consume() {
	EVENT_CONSUME_INFO();
	concernedBlock->processLocalEvent();
}

string ProcessLocalEvent::getEventName() {
	return("ProcessLocal Event");
}

//===========================================================================================================
//
//          BlockTimerEvent  (class)
//
//===========================================================================================================

BlockTimerEvent::BlockTimerEvent(uint64_t t, BuildingBlock *block, unsigned int tId) : Event(t) {
	EVENT_CONSTRUCTOR_INFO();
	concernedBlock = block;
	timerId = tId;
	eventType = EVENT_BLOCK_TIMER;
}

BlockTimerEvent::BlockTimerEvent(BlockTimerEvent *ev) : Event(ev) {
	EVENT_CONSTRUCTOR_INFO();
	concernedBlock = ev->concernedBlock;
	timerId = ev->timerId;
	eventType = ev->eventType;
}

BlockTimerEvent::~BlockTimerEvent() {
	EVENT_DESTRUCTOR_INFO();
}

void BlockTimerEvent::consume() {
	EVENT_CONSUME_INFO();
	concernedBlock->scheduleLocalEvent(EventPtr(new BlockTimerEvent(this)));
	return;
}

string BlockTimerEvent::getEventName() {
	return("BlockTimer Event");
}

//===========================================================================================================
//
//          VMStartComputationEvent  (class)
//
//===========================================================================================================

VMStartComputationEvent::VMStartComputationEvent(uint64_t t, BuildingBlock *conBlock, uint64_t dur):Event(t) {
	EVENT_CONSTRUCTOR_INFO();
	eventType = EVENT_VM_START_COMPUTATION;
	concernedBlock = conBlock;
	duration = dur;
}

VMStartComputationEvent::VMStartComputationEvent(VMStartComputationEvent *ev) : Event(ev) {
	EVENT_CONSTRUCTOR_INFO();
	concernedBlock = ev->concernedBlock;
	eventType = ev->eventType;
	duration = ev->duration;
}

VMStartComputationEvent::~VMStartComputationEvent() {
	EVENT_DESTRUCTOR_INFO();
}

void VMStartComputationEvent::consume() {
	EVENT_CONSUME_INFO();
	concernedBlock->scheduleLocalEvent(EventPtr(new VMStartComputationEvent(this)));
}

string VMStartComputationEvent::getEventName() {
	return("VMStartComputation Event");
}

//===========================================================================================================
//
//          VMEndComputationEvent  (class)
//
//===========================================================================================================

VMEndComputationEvent::VMEndComputationEvent(uint64_t t, BuildingBlock *block) : Event(t) {
	EVENT_CONSTRUCTOR_INFO();
	concernedBlock = block;
	eventType = EVENT_VM_END_COMPUTATION;
}

VMEndComputationEvent::VMEndComputationEvent(VMEndComputationEvent *ev) : Event(ev) {
	EVENT_CONSTRUCTOR_INFO();
	concernedBlock = ev->concernedBlock;
	eventType = ev->eventType;
}

VMEndComputationEvent::~VMEndComputationEvent() {
	EVENT_DESTRUCTOR_INFO();
}

void VMEndComputationEvent::consume() {
	EVENT_CONSUME_INFO();
	concernedBlock->scheduleLocalEvent(EventPtr(new VMEndComputationEvent(this)));
	return;
}

string VMEndComputationEvent::getEventName() {
	return("VMEndComputation Event");
}

//===========================================================================================================
//
//          VMStartTransmissionEvent  (class)
//
//===========================================================================================================

VMStartTransmissionEvent::VMStartTransmissionEvent(uint64_t t, BuildingBlock *conBlock, unsigned int dbId, unsigned int mSize):Event(t) {
	EVENT_CONSTRUCTOR_INFO();
	eventType = EVENT_VM_START_TRANSMISSION;
	concernedBlock = conBlock;
	messageSize = mSize;
	destBlockId = dbId;
}

VMStartTransmissionEvent::VMStartTransmissionEvent(VMStartTransmissionEvent *ev) : Event(ev) {
	EVENT_CONSTRUCTOR_INFO();
	concernedBlock = ev->concernedBlock;
	eventType = ev->eventType;
	messageSize = ev->messageSize;
	destBlockId = ev->destBlockId;
}

VMStartTransmissionEvent::~VMStartTransmissionEvent() {
	EVENT_DESTRUCTOR_INFO();
}

void VMStartTransmissionEvent::consume() {
	EVENT_CONSUME_INFO();
	concernedBlock->scheduleLocalEvent(EventPtr(new VMStartTransmissionEvent(this)));
}

string VMStartTransmissionEvent::getEventName() {
	return("VMStartTransmission Event");
}
