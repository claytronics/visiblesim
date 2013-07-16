/*
 * blinkyBlocksEvents.cpp
 *
 *  Created on: 12 juin 2013
 *      Author: andre
 */

#include "blinkyBlocksEvents.h"
#include "blinkyBlocksScheduler.h"

namespace BlinkyBlocks {

//===========================================================================================================
//
//          VMSetIdEvent  (class)
//
//===========================================================================================================

VMSetIdEvent::VMSetIdEvent(uint64_t t, BlinkyBlocksBlock *conBlock): BlockEvent(t, conBlock) {
	EVENT_CONSTRUCTOR_INFO();
	eventType = EVENT_SET_ID;
}

VMSetIdEvent::VMSetIdEvent(VMSetIdEvent *ev) : BlockEvent(ev) {
	EVENT_CONSTRUCTOR_INFO();
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

VMStopEvent::VMStopEvent(uint64_t t, BlinkyBlocksBlock *conBlock): BlockEvent(t, conBlock) {
	EVENT_CONSTRUCTOR_INFO();
	eventType = EVENT_STOP;
}

VMStopEvent::VMStopEvent(VMStopEvent *ev) : BlockEvent(ev) {
	EVENT_CONSTRUCTOR_INFO();
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

VMAddNeighborEvent::VMAddNeighborEvent(uint64_t t, BlinkyBlocksBlock *conBlock, uint64_t f, uint64_t ta): BlockEvent(t, conBlock) {
	EVENT_CONSTRUCTOR_INFO();
	eventType = EVENT_ADD_NEIGHBOR;
	face = f;
	target = ta;
}

VMAddNeighborEvent::VMAddNeighborEvent(VMAddNeighborEvent *ev) : BlockEvent(ev) {
	EVENT_CONSTRUCTOR_INFO();
	face = ev->face;
	target = ev->target;
}

VMAddNeighborEvent::~VMAddNeighborEvent() {
	EVENT_DESTRUCTOR_INFO();
}

void VMAddNeighborEvent::consumeBlockEvent() {
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

VMRemoveNeighborEvent::VMRemoveNeighborEvent(uint64_t t, BlinkyBlocksBlock *conBlock, uint64_t f): BlockEvent(t, conBlock) {
	EVENT_CONSTRUCTOR_INFO();
	eventType = EVENT_REMOVE_NEIGHBOR;
	face = f;
}

VMRemoveNeighborEvent::VMRemoveNeighborEvent(VMRemoveNeighborEvent *ev) : BlockEvent(ev) {
	EVENT_CONSTRUCTOR_INFO();
	face = ev->face;
}

VMRemoveNeighborEvent::~VMRemoveNeighborEvent() {
	EVENT_DESTRUCTOR_INFO();
}

void VMRemoveNeighborEvent::consumeBlockEvent() {
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

VMTapEvent::VMTapEvent(uint64_t t, BlinkyBlocksBlock *conBlock): BlockEvent(t, conBlock) {
	EVENT_CONSTRUCTOR_INFO();
	eventType = EVENT_TAP;
}

VMTapEvent::VMTapEvent(VMTapEvent *ev) : BlockEvent(ev) {
	EVENT_CONSTRUCTOR_INFO();
}

VMTapEvent::~VMTapEvent() {
	EVENT_DESTRUCTOR_INFO();
}

void VMTapEvent::consumeBlockEvent() {
	EVENT_CONSUME_INFO();
	concernedBlock->scheduleLocalEvent(EventPtr(new VMTapEvent(this)));
}

const string VMTapEvent::getEventName() {
	return("VMTap Event");
}


//===========================================================================================================
//
//          VMAccelEvent  (class)
//
//===========================================================================================================

VMAccelEvent::VMAccelEvent(uint64_t t, BlinkyBlocksBlock *conBlock, uint64_t xx, uint64_t yy, uint64_t zz): BlockEvent(t, conBlock) {
	EVENT_CONSTRUCTOR_INFO();
	eventType = EVENT_ACCEL;
	x = xx;
	y = yy;
	z = zz;
}

VMAccelEvent::VMAccelEvent(VMAccelEvent *ev) : BlockEvent(ev) {
	EVENT_CONSTRUCTOR_INFO();
	x = ev->x;
	y = ev->y;
	z = ev->z;
}

VMAccelEvent::~VMAccelEvent() {
	EVENT_DESTRUCTOR_INFO();
}

void VMAccelEvent::consumeBlockEvent() {
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

VMShakeEvent::VMShakeEvent(uint64_t t, BlinkyBlocksBlock *conBlock, uint64_t f): BlockEvent(t, conBlock) {
	EVENT_CONSTRUCTOR_INFO();
	eventType = EVENT_SHAKE;
	force = f;
}

VMShakeEvent::VMShakeEvent(VMShakeEvent *ev) : BlockEvent(ev) {
	EVENT_CONSTRUCTOR_INFO();
	force = ev->force;
}

VMShakeEvent::~VMShakeEvent() {
	EVENT_DESTRUCTOR_INFO();
}

void VMShakeEvent::consumeBlockEvent() {
	EVENT_CONSUME_INFO();
	concernedBlock->scheduleLocalEvent(EventPtr(new VMShakeEvent(this)));
}

const string VMShakeEvent::getEventName() {
	return("VMShake Event");
}

//===========================================================================================================
//
//          VMDebugMessageEvent  (class)
//
//===========================================================================================================

VMDebugMessageEvent::VMDebugMessageEvent(uint64_t t, BlinkyBlocksBlock *conBlock, VMDebugMessage *mes): BlockEvent(t, conBlock) {
	EVENT_CONSTRUCTOR_INFO();
	eventType = EVENT_DEBUG_MESSAGE;
	message = VMDebugMessagePtr(mes);
}

VMDebugMessageEvent::VMDebugMessageEvent(VMDebugMessageEvent *ev) : BlockEvent(ev) {
	EVENT_CONSTRUCTOR_INFO();
	message = ev->message;
}

VMDebugMessageEvent::~VMDebugMessageEvent() {
	EVENT_DESTRUCTOR_INFO();
	message.reset();
}

void VMDebugMessageEvent::consumeBlockEvent() {
	EVENT_CONSUME_INFO();
	concernedBlock->scheduleLocalEvent(EventPtr(new VMDebugMessageEvent(this)));
}

const string VMDebugMessageEvent::getEventName() {
	return("VMDebugMessage Event");
}

//===========================================================================================================
//
//          VMDebugPauseSimEvent  (class)
//
//===========================================================================================================

VMDebugPauseSimEvent::VMDebugPauseSimEvent(uint64_t t): Event(t) {
	EVENT_CONSTRUCTOR_INFO();
	eventType = EVENT_DEBUG_PAUSE_SIMULATION;
}

VMDebugPauseSimEvent::VMDebugPauseSimEvent(VMDebugPauseSimEvent *ev) : Event(ev) {
	EVENT_CONSTRUCTOR_INFO();
}

VMDebugPauseSimEvent::~VMDebugPauseSimEvent() {
	EVENT_DESTRUCTOR_INFO();
}

void VMDebugPauseSimEvent::consume() {
	EVENT_CONSUME_INFO();
	OUTPUT << "pause sim degin" << endl;
	getScheduler()->sem_schedulerStart->wait();
	OUTPUT << "pause sim end" << endl;
}

const string VMDebugPauseSimEvent::getEventName() {
	return("VMDebugPauseSim Event");
}

//===========================================================================================================
//
//          VMStartComputationEvent  (class)
//
//===========================================================================================================

VMStartComputationEvent::VMStartComputationEvent(uint64_t t, BlinkyBlocksBlock *conBlock, uint64_t dur): BlockEvent(t, conBlock) {
	EVENT_CONSTRUCTOR_INFO();
	eventType = EVENT_VM_START_COMPUTATION;
	duration = dur;
}

VMStartComputationEvent::VMStartComputationEvent(VMStartComputationEvent *ev) : BlockEvent(ev) {
	EVENT_CONSTRUCTOR_INFO();
	duration = ev->duration;
}

VMStartComputationEvent::~VMStartComputationEvent() {
	EVENT_DESTRUCTOR_INFO();
}

void VMStartComputationEvent::consumeBlockEvent() {
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

VMEndComputationEvent::VMEndComputationEvent(uint64_t t, BlinkyBlocksBlock *conBlock) : BlockEvent(t, conBlock) {
	EVENT_CONSTRUCTOR_INFO();
	eventType = EVENT_VM_END_COMPUTATION;
}

VMEndComputationEvent::VMEndComputationEvent(VMEndComputationEvent *ev) : BlockEvent(ev) {
	EVENT_CONSTRUCTOR_INFO();
}

VMEndComputationEvent::~VMEndComputationEvent() {
	EVENT_DESTRUCTOR_INFO();
}

void VMEndComputationEvent::consumeBlockEvent() {
	EVENT_CONSUME_INFO();
	concernedBlock->scheduleLocalEvent(EventPtr(new VMEndComputationEvent(this)));
	return;
}

const string VMEndComputationEvent::getEventName() {
	return("VMEndComputation Event");
}

//===========================================================================================================
//
//          VMWaitForCommandEvent  (class)
//
//===========================================================================================================

VMWaitForCommandEvent::VMWaitForCommandEvent(uint64_t t, BlinkyBlocksBlock *conBlock, uint64_t tt): BlockEvent(t, conBlock) {
	EVENT_CONSTRUCTOR_INFO();
	eventType = EVENT_VM_WAIT_FOR_COMMAND;
	timeOut = tt;
}

VMWaitForCommandEvent::VMWaitForCommandEvent(VMWaitForCommandEvent *ev) : BlockEvent(ev) {
	EVENT_CONSTRUCTOR_INFO();
	timeOut = ev->timeOut;
}

VMWaitForCommandEvent::~VMWaitForCommandEvent() {
	EVENT_DESTRUCTOR_INFO();
}

void VMWaitForCommandEvent::consumeBlockEvent() {
	EVENT_CONSUME_INFO();
	concernedBlock->scheduleLocalEvent(EventPtr(new VMWaitForCommandEvent(this)));
}

const string VMWaitForCommandEvent::getEventName() {
	return("VMWaitForCommand Event");
}

//===========================================================================================================
//
//          VMTimeOutWaitForCommandEvent  (class)
//
//===========================================================================================================

VMTimeOutWaitForCommandEvent::VMTimeOutWaitForCommandEvent(uint64_t t, BlinkyBlocksBlock *conBlock): BlockEvent(t, conBlock) {
	EVENT_CONSTRUCTOR_INFO();
	eventType = EVENT_VM_TIMEOUT_WAIT_FOR_COMMAND;
	cancelled = false;
}

VMTimeOutWaitForCommandEvent::VMTimeOutWaitForCommandEvent(VMTimeOutWaitForCommandEvent *ev) : BlockEvent(ev) {
	EVENT_CONSTRUCTOR_INFO();
	cancelled = ev->cancelled;
}

VMTimeOutWaitForCommandEvent::~VMTimeOutWaitForCommandEvent() {
	EVENT_DESTRUCTOR_INFO();
}

void VMTimeOutWaitForCommandEvent::consumeBlockEvent() {
	EVENT_CONSUME_INFO();
	if (!cancelled) {
		concernedBlock->scheduleLocalEvent(EventPtr(new VMTimeOutWaitForCommandEvent(this)));
	}
}

const string VMTimeOutWaitForCommandEvent::getEventName() {
	return("VMTimeOutWaitForCommand Event");
}

} // BlinkyBlocks namespace
