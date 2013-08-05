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
	priority = PRIORITY_EVENT_SET_ID;
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
	priority = PRIORITY_EVENT_STOP;
}

VMStopEvent::VMStopEvent(VMStopEvent *ev) : BlockEvent(ev) {
	EVENT_CONSTRUCTOR_INFO();
}

VMStopEvent::~VMStopEvent() {
	EVENT_DESTRUCTOR_INFO();
}

void VMStopEvent::consume() {
	EVENT_CONSUME_INFO();
	//concernedBlock->scheduleLocalEvent(EventPtr(new VMStopEvent(this)));
	concernedBlock->blockCode->processLocalEvent(EventPtr(new VMStopEvent(this)));
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
	priority = PRIORITY_EVENT_ADD_NEIGHBOR;
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
	randomNumber = -1;
	priority = PRIORITY_EVENT_REMOVE_NEIGHBOR;
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
	priority = PRIORITY_EVENT_TAP;
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
//          VMSetColorEvent  (class)
//
//===========================================================================================================

VMSetColorEvent::VMSetColorEvent(uint64_t t, BlinkyBlocksBlock *conBlock, float r, float g, float b, float a): BlockEvent(t, conBlock) {
	EVENT_CONSTRUCTOR_INFO();
	eventType = EVENT_SET_COLOR;
	priority = PRIORITY_EVENT_SET_COLOR;
	color = Vecteur(r, g, b, a);
}

VMSetColorEvent::VMSetColorEvent(VMSetColorEvent *ev) : BlockEvent(ev) {
	EVENT_CONSTRUCTOR_INFO();
	color = ev->color;
}

VMSetColorEvent::~VMSetColorEvent() {
	EVENT_DESTRUCTOR_INFO();
}

void VMSetColorEvent::consumeBlockEvent() {
	EVENT_CONSUME_INFO();
	concernedBlock->scheduleLocalEvent(EventPtr(new VMSetColorEvent(this)));
}

const string VMSetColorEvent::getEventName() {
	return("VMSetColor Event");
}

//===========================================================================================================
//
//          VMSendMessageEvent  (class)
//
//===========================================================================================================

VMSendMessageEvent::VMSendMessageEvent(uint64_t t, BlinkyBlocksBlock *conBlock, Message *mes, P2PNetworkInterface *ni):BlockEvent(t, conBlock) {
	eventType = EVENT_SEND_MESSAGE;
	message = MessagePtr(mes);
	sourceInterface = ni;
	priority = PRIORITY_EVENT_SEND_MESSAGE;
	EVENT_CONSTRUCTOR_INFO();
}

VMSendMessageEvent::VMSendMessageEvent(VMSendMessageEvent *ev) : BlockEvent(ev) {
	message = ev->message;
	sourceInterface = ev->sourceInterface;
	EVENT_CONSTRUCTOR_INFO();
}

VMSendMessageEvent::~VMSendMessageEvent() {
	message.reset();
	EVENT_DESTRUCTOR_INFO();
}

void VMSendMessageEvent::consumeBlockEvent() {
	EVENT_CONSUME_INFO();
	concernedBlock->scheduleLocalEvent(EventPtr(new VMSendMessageEvent(this)));
}

const string VMSendMessageEvent::getEventName() {
	return("VMSendMessage Event");
}


//===========================================================================================================
//
//          VMAccelEvent  (class)
//
//===========================================================================================================

VMAccelEvent::VMAccelEvent(uint64_t t, BlinkyBlocksBlock *conBlock, uint64_t xx, uint64_t yy, uint64_t zz): BlockEvent(t, conBlock) {
	EVENT_CONSTRUCTOR_INFO();
	eventType = EVENT_ACCEL;
	priority = PRIORITY_EVENT_ACCEL;
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
	priority = PRIORITY_EVENT_SHAKE;
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
	priority = PRIORITY_EVENT_DEBUG_MESSAGE;
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
	priority = PRIORITY_EVENT_DEBUG_PAUSE_SIMULATION;
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
	getScheduler()->setState(Scheduler::PAUSED);
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
	priority = PRIORITY_EVENT_VM_START_COMPUTATION;
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
//          VMExpectedEndComputationEvent  (class)
//
//===========================================================================================================

VMExpectedEndComputationEvent::VMExpectedEndComputationEvent(uint64_t t, BlinkyBlocksBlock *conBlock) : BlockEvent(t, conBlock) {
	EVENT_CONSTRUCTOR_INFO();
	priority = PRIORITY_EVENT_VM_END_COMPUTATION;
	eventType = EVENT_VM_EXPECTED_END_COMPUTATION;
}

VMExpectedEndComputationEvent::VMExpectedEndComputationEvent(VMExpectedEndComputationEvent *ev) : BlockEvent(ev) {
	EVENT_CONSTRUCTOR_INFO();
}

VMExpectedEndComputationEvent::~VMExpectedEndComputationEvent() {
	EVENT_DESTRUCTOR_INFO();
}

void VMExpectedEndComputationEvent::consumeBlockEvent() {
	EVENT_CONSUME_INFO();
	concernedBlock->scheduleLocalEvent(EventPtr(new VMExpectedEndComputationEvent(this)));
	return;
}

const string VMExpectedEndComputationEvent::getEventName() {
	return("VMExpectedEndComputation Event");
}

//===========================================================================================================
//
//          VMEffectiveEndComputationEvent  (class)
//
//===========================================================================================================

VMEffectiveEndComputationEvent::VMEffectiveEndComputationEvent(uint64_t t, BlinkyBlocksBlock *conBlock) : BlockEvent(t, conBlock) {
	EVENT_CONSTRUCTOR_INFO();
	priority = PRIORITY_EVENT_VM_END_COMPUTATION;
	eventType = EVENT_VM_EFFECTIVE_END_COMPUTATION;
}

VMEffectiveEndComputationEvent::VMEffectiveEndComputationEvent(VMEffectiveEndComputationEvent *ev) : BlockEvent(ev) {
	EVENT_CONSTRUCTOR_INFO();
}

VMEffectiveEndComputationEvent::~VMEffectiveEndComputationEvent() {
	EVENT_DESTRUCTOR_INFO();
}

void VMEffectiveEndComputationEvent::consumeBlockEvent() {
	EVENT_CONSUME_INFO();
	concernedBlock->scheduleLocalEvent(EventPtr(new VMEffectiveEndComputationEvent(this)));
	return;
}

const string VMEffectiveEndComputationEvent::getEventName() {
	return("VMEffectiveEndComputation Event");
}


} // BlinkyBlocks namespace
