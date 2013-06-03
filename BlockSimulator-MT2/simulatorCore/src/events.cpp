/*
 * events.cpp
 *
 *  Created on: 26 mars 2013
 *      Author: dom
 */


#include <iostream>
#include <sstream>
#include "events.h"
#include "scheduler.h"
#include "blockCode.h"

int Event::nextId = 0;
unsigned int Event::nbLivingEvents = 0;

using namespace std;

//===========================================================================================================
//
//          Event  (class)
//
//===========================================================================================================

Event::Event(uint64_t t) {
	id = nextId;
	nextId++;
	nbLivingEvents++;
	date = t;
	EVENT_CONSTRUCTOR_INFO();
}

Event::Event(Event *ev) {
	id = nextId;
	nextId++;
	nbLivingEvents++;
	date = ev->date;
	EVENT_CONSTRUCTOR_INFO();
}

Event::~Event() {
	EVENT_DESTRUCTOR_INFO();
	nbLivingEvents--;
}

const string Event::getEventName() {
	return("Generic Event");
}

unsigned int Event::getNextId() {
	return(nextId);
}

unsigned int Event::getNbLivingEvents() {
	return(nbLivingEvents);
}

//===========================================================================================================
//
//          CodeStartEvent  (class)
//
//===========================================================================================================

CodeStartEvent::CodeStartEvent(uint64_t t, BaseSimulator::BuildingBlock *conBlock):Event(t) {
	EVENT_CONSTRUCTOR_INFO();
	eventType = EVENT_CODE_START;
	concernedBlock = conBlock;
}
CodeStartEvent::~CodeStartEvent() {
	EVENT_DESTRUCTOR_INFO();
}

void CodeStartEvent::consume() {
	EVENT_CONSUME_INFO();
	//MODIF NICO
	concernedBlock->blockCode->startup();
	//FIN MODIF NICO
}

const string CodeStartEvent::getEventName() {
	return("CodeStart Event");
}

//===========================================================================================================
//
//          ProcessLocalEvent  (class)
//
//===========================================================================================================

ProcessLocalEvent::ProcessLocalEvent(uint64_t t, BaseSimulator::BuildingBlock *conBlock):Event(t) {
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

const string ProcessLocalEvent::getEventName() {
	return("ProcessLocal Event");
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

const string NetworkInterfaceStartTransmittingEvent::getEventName() {
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

	interface->connectedInterface->hostBlock->scheduleLocalEvent(EventPtr(new NetworkInterfaceReceiveEvent(BaseSimulator::getScheduler()->now(), interface->connectedInterface, interface->messageBeingTransmitted)));
	// TODO add a confirmation event to the sender ?

	interface->messageBeingTransmitted.reset();
	interface->availabilityDate = BaseSimulator::getScheduler()->now();
	if (interface->outgoingQueue.size() > 0) {
		//cout << "one more to send !!" << endl;
		interface->send();
	}
}

const string NetworkInterfaceStopTransmittingEvent::getEventName() {
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

const string NetworkInterfaceReceiveEvent::getEventName() {
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

const string NetworkInterfaceEnqueueOutgoingEvent::getEventName() {
	return("NetworkInterfaceEnqueueOutgoingEvent Event");
}
