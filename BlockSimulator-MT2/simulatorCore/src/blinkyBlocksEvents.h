/*
 * blinkyBlocksEvents.h
 *
 *  Created on: 12 juin 2013
 *      Author: andre
 */

#ifndef BLINKYBLOCKSEVENTS_H_
#define BLINKYBLOCKSEVENTS_H_

#include "blinkyBlocksBlock.h"
#include "events.h"
#include "uniqueEventsId.h"

namespace BlinkyBlocks {

//===========================================================================================================
//
//          VMSetIdEvent  (class)
//
//===========================================================================================================

class VMSetIdEvent : public BlockEvent {
public:

	VMSetIdEvent(uint64_t, BlinkyBlocksBlock *conBlock);
	VMSetIdEvent(VMSetIdEvent *ev);
	~VMSetIdEvent();
	void consumeBlockEvent();
	const virtual string getEventName();
};

//===========================================================================================================
//
//          VMStopEvent  (class)
//
//===========================================================================================================

class VMStopEvent : public BlockEvent {
public:

	VMStopEvent(uint64_t, BlinkyBlocksBlock *conBlock);
	VMStopEvent(VMStopEvent *ev);
	~VMStopEvent();
	void consumeBlockEvent();
	const virtual string getEventName();
};

//===========================================================================================================
//
//          VMAddNeighborEvent  (class)
//
//===========================================================================================================

class VMAddNeighborEvent : public BlockEvent {
public:
	uint64_t face;
	uint64_t target;
	
	VMAddNeighborEvent(uint64_t, BlinkyBlocksBlock *conBlock, uint64_t f, uint64_t ta);
	VMAddNeighborEvent(VMAddNeighborEvent *ev);
	~VMAddNeighborEvent();
	void consumeBlockEvent();
	const virtual string getEventName();
};

//===========================================================================================================
//
//          VMRemoveNeighborEvent  (class)
//
//===========================================================================================================

class VMRemoveNeighborEvent : public BlockEvent {
public:
	uint64_t face;
	
	VMRemoveNeighborEvent(uint64_t, BlinkyBlocksBlock *conBlock, uint64_t f);
	VMRemoveNeighborEvent(VMRemoveNeighborEvent *ev);
	~VMRemoveNeighborEvent();
	void consumeBlockEvent();
	const virtual string getEventName();
};

//===========================================================================================================
//
//          VMTapEvent  (class)
//
//===========================================================================================================

class VMTapEvent : public BlockEvent {
public:

	VMTapEvent(uint64_t, BlinkyBlocksBlock *conBlock);
	VMTapEvent(VMTapEvent *ev);
	~VMTapEvent();
	void consumeBlockEvent();
	const virtual string getEventName();
};


//===========================================================================================================
//
//          VMReceiveMessageEvent  (class)
//
//===========================================================================================================
/*
class VMReceiveMessageEvent : public Event {
public:
	BlinkyBlocksBlock *concernedBlock;
	uint64_t sender;
	uint64_t face;
	uint64_t size;
	uint64_t *message;
	
	VMReceiveMessageEvent(uint64_t, BlinkyBlocksBlock *conBlock, uint64_t se, uint64_t f, uint64_t si, uint64_t* m);
	VMReceiveMessageEvent(VMReceiveMessageEvent *ev);
	~VMReceiveMessageEvent();
	void consume();
	const virtual string getEventName();
}; */


//===========================================================================================================
//
//          VMAccelEvent  (class)
//
//===========================================================================================================

class VMAccelEvent : public BlockEvent {
public:
	uint64_t x;
	uint64_t y;
	uint64_t z;
	
	VMAccelEvent(uint64_t, BlinkyBlocksBlock *conBlock, uint64_t xx, uint64_t yy, uint64_t zz);
	VMAccelEvent(VMAccelEvent *ev);
	~VMAccelEvent();
	void consumeBlockEvent();
	const virtual string getEventName();
};

//===========================================================================================================
//
//          VMShakeEvent  (class)
//
//===========================================================================================================

class VMShakeEvent : public BlockEvent {
public:
	uint64_t force;
	
	VMShakeEvent(uint64_t, BlinkyBlocksBlock *conBlock, uint64_t f);
	VMShakeEvent(VMShakeEvent *ev);
	~VMShakeEvent();
	void consumeBlockEvent();
	const virtual string getEventName();
};

//===========================================================================================================
//
//          VMDebugMessageEvent  (class)
//
//===========================================================================================================

class VMDebugMessageEvent : public BlockEvent {

public:
	MessagePtr message;
	
	VMDebugMessageEvent(uint64_t, BlinkyBlocksBlock *conBlock, Message *mes);
	VMDebugMessageEvent(VMDebugMessageEvent *ev);
	~VMDebugMessageEvent();
	void consumeBlockEvent();
	const virtual string getEventName();
};


} // BlinkyBlocks namespace


#endif /* BLINKYBLOCKSEVENTS_H_ */
