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
//          VMStopEvent  (class)
//
//===========================================================================================================

VMStopEvent::VMStopEvent(uint64_t t, BlinkyBlocksBlock *conBlock):Event(t) {
	EVENT_CONSTRUCTOR_INFO();
	eventType = EVENT_STOP;
	concernedBlock = conBlock;
}

VMStopEvent::VMStopEvent(VMStopEvent *ev) : Event(ev) {
	EVENT_CONSTRUCTOR_INFO();
	concernedBlock = ev->concernedBlock;
	eventType = ev->eventType;
}

VMStopEvent::~VMStopEvent() {
	EVENT_DESTRUCTOR_INFO();
}

void VMStopEvent::consume() {
	EVENT_CONSUME_INFO();
	concernedBlock->scheduleLocalEvent(EventPtr(new VMStopEvent(this)));
}

const string VMStopEvent::getEventName() {
	return("VMStop Event");
}

//===========================================================================================================
//
//          VMAddNeighborEvent  (class)
//
//===========================================================================================================

VMAddNeighborEvent::VMAddNeighborEvent(uint64_t t, BlinkyBlocksBlock *conBlock, uint64_t f, uint64_t ta):Event(t) {
	EVENT_CONSTRUCTOR_INFO();
	eventType = EVENT_ADD_NEIGHBOR;
	concernedBlock = conBlock;
	face = f;
	target = ta;
}

VMAddNeighborEvent::VMAddNeighborEvent(VMAddNeighborEvent *ev) : Event(ev) {
	EVENT_CONSTRUCTOR_INFO();
	concernedBlock = ev->concernedBlock;
	eventType = ev->eventType;
}

VMAddNeighborEvent::~VMAddNeighborEvent() {
	EVENT_DESTRUCTOR_INFO();
}

void VMAddNeighborEvent::consume() {
	EVENT_CONSUME_INFO();
	concernedBlock->scheduleLocalEvent(EventPtr(new VMAddNeighborEvent(this)));
}

const string VMAddNeighborEvent::getEventName() {
	return("VMAddNeighbor Event");
}

//===========================================================================================================
//
//          VMRemoveNeighborEvent  (class)
//
//===========================================================================================================

VMRemoveNeighborEvent::VMRemoveNeighborEvent(uint64_t t, BlinkyBlocksBlock *conBlock, uint64_t f):Event(t) {
	EVENT_CONSTRUCTOR_INFO();
	eventType = EVENT_REMOVE_NEIGHBOR;
	concernedBlock = conBlock;
	face = f;
}

VMRemoveNeighborEvent::VMRemoveNeighborEvent(VMRemoveNeighborEvent *ev) : Event(ev) {
	EVENT_CONSTRUCTOR_INFO();
	concernedBlock = ev->concernedBlock;
	eventType = ev->eventType;
}

VMRemoveNeighborEvent::~VMRemoveNeighborEvent() {
	EVENT_DESTRUCTOR_INFO();
}

void VMRemoveNeighborEvent::consume() {
	EVENT_CONSUME_INFO();
	concernedBlock->scheduleLocalEvent(EventPtr(new VMRemoveNeighborEvent(this)));
}

const string VMRemoveNeighborEvent::getEventName() {
	return("VMRemoveNeighbor Event");
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

//===========================================================================================================
//
//          VMReceiveMessageEvent  (class)
//
//===========================================================================================================
/*
VMReceiveMessageEvent::VMReceiveMessageEvent(uint64_t t, BlinkyBlocksBlock *conBlock, uint64_t se, uint64_t f, uint64_t si, uint64_t*  m):Event(t) {
	EVENT_CONSTRUCTOR_INFO();
	eventType = EVENT_ADD_NEIGHBOR;
	concernedBlock = conBlock;
	sender = se;
	face = f;
	size = si;
	//copy message to do
}

VMReceiveMessageEvent::VMReceiveMessageEvent(VMReceiveMessageEvent *ev) : Event(ev) {
	EVENT_CONSTRUCTOR_INFO();
	concernedBlock = ev->concernedBlock;
	eventType = ev->eventType;
}

VMReceiveMessageEvent::~VMReceiveMessageEvent() {
	EVENT_DESTRUCTOR_INFO();
}

void VMReceiveMessageEvent::consume() {
	EVENT_CONSUME_INFO();
	concernedBlock->scheduleLocalEvent(EventPtr(new VMReceiveMessageEvent(this)));
}

const string VMReceiveMessageEvent::getEventName() {
	return("VMReceiveMessage Event");
} */

//===========================================================================================================
//
//          VMAccelEvent  (class)
//
//===========================================================================================================

VMAccelEvent::VMAccelEvent(uint64_t t, BlinkyBlocksBlock *conBlock, uint64_t xx, uint64_t yy, uint64_t zz):Event(t) {
	EVENT_CONSTRUCTOR_INFO();
	eventType = EVENT_ACCEL;
	concernedBlock = conBlock;
	x = xx;
	y = yy;
	z = zz;
}

VMAccelEvent::VMAccelEvent(VMAccelEvent *ev) : Event(ev) {
	EVENT_CONSTRUCTOR_INFO();
	concernedBlock = ev->concernedBlock;
	eventType = ev->eventType;
}

VMAccelEvent::~VMAccelEvent() {
	EVENT_DESTRUCTOR_INFO();
}

void VMAccelEvent::consume() {
	EVENT_CONSUME_INFO();
	concernedBlock->scheduleLocalEvent(EventPtr(new VMAccelEvent(this)));
}

const string VMAccelEvent::getEventName() {
	return("VMAccel Event");
}

//===========================================================================================================
//
//          VMShakeEvent  (class)
//
//===========================================================================================================

VMShakeEvent::VMShakeEvent(uint64_t t, BlinkyBlocksBlock *conBlock, uint64_t f):Event(t) {
	EVENT_CONSTRUCTOR_INFO();
	eventType = EVENT_SHAKE;
	concernedBlock = conBlock;
	force = f;
}

VMShakeEvent::VMShakeEvent(VMShakeEvent *ev) : Event(ev) {
	EVENT_CONSTRUCTOR_INFO();
	concernedBlock = ev->concernedBlock;
	eventType = ev->eventType;
}

VMShakeEvent::~VMShakeEvent() {
	EVENT_DESTRUCTOR_INFO();
}

void VMShakeEvent::consume() {
	EVENT_CONSUME_INFO();
	concernedBlock->scheduleLocalEvent(EventPtr(new VMShakeEvent(this)));
}

const string VMShakeEvent::getEventName() {
	return("VMShake Event");
}

} // BlinkyBlocks namespace
