/*
 * blinkyBlocksEvents.cpp
 *
 *  Created on: 12 juin 2013
 *      Author: andre
 */

#include "blinkyBlocksEvents.h"

namespace BlinkyBlocks {

//===========================================================================================================
//
//          VMSetIdEvent  (class)
//
//===========================================================================================================

VMSetIdEvent::VMSetIdEvent(uint64_t t, BlinkyBlocksBlock *conBlock):Event(t) {
	EVENT_CONSTRUCTOR_INFO();
	eventType = EVENT_SET_ID;
	concernedBlock = conBlock;
}

VMSetIdEvent::VMSetIdEvent(VMSetIdEvent *ev) : Event(ev) {
	EVENT_CONSTRUCTOR_INFO();
	concernedBlock = ev->concernedBlock;
	eventType = ev->eventType;
}

VMSetIdEvent::~VMSetIdEvent() {
	EVENT_DESTRUCTOR_INFO();
}

void VMSetIdEvent::consume() {
	EVENT_CONSUME_INFO();
	concernedBlock->scheduleLocalEvent(EventPtr(new VMSetIdEvent(this)));
}

const string VMSetIdEvent::getEventName() {
	return("VMSetId Event");
}

//===========================================================================================================
//
//          VMTapEvent  (class)
//
//===========================================================================================================

VMTapEvent::VMTapEvent(uint64_t t, BlinkyBlocksBlock *conBlock):Event(t) {
	EVENT_CONSTRUCTOR_INFO();
	eventType = EVENT_TAP;
	concernedBlock = conBlock;
}

VMTapEvent::VMTapEvent(VMTapEvent *ev) : Event(ev) {
	EVENT_CONSTRUCTOR_INFO();
	concernedBlock = ev->concernedBlock;
	eventType = ev->eventType;
}

VMTapEvent::~VMTapEvent() {
	EVENT_DESTRUCTOR_INFO();
}

void VMTapEvent::consume() {
	EVENT_CONSUME_INFO();
	concernedBlock->scheduleLocalEvent(EventPtr(new VMTapEvent(this)));
}

const string VMTapEvent::getEventName() {
	return("VMTap Event");
}

/*
//===========================================================================================================
//
//          VMStartComputationEvent  (class)
//
//===========================================================================================================

VMStartComputationEvent::VMStartComputationEvent(uint64_t t, MultiCoresBlock *conBlock, uint64_t dur):Event(t) {
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

const string VMStartComputationEvent::getEventName() {
	return("VMStartComputation Event");
}

//===========================================================================================================
//
//          VMEndComputationEvent  (class)
//
//===========================================================================================================

VMEndComputationEvent::VMEndComputationEvent(uint64_t t, MultiCoresBlock *block) : Event(t) {
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

const string VMEndComputationEvent::getEventName() {
	return("VMEndComputation Event");
}

//===========================================================================================================
//
//          VMStartTransmissionEvent  (class)
//
//===========================================================================================================

VMStartTransmissionEvent::VMStartTransmissionEvent(uint64_t t, MultiCoresBlock *conBlock, int dbId, unsigned int mSize):Event(t) {
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

const string VMStartTransmissionEvent::getEventName() {
	return("VMStartTransmission Event");
}
*/
} // BlinkyBlocks namespace
