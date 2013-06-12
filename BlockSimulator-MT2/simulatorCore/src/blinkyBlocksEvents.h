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

class VMSetIdEvent : public Event {
public:
	BlinkyBlocksBlock *concernedBlock;

	VMSetIdEvent(uint64_t, BlinkyBlocksBlock *conBlock);
	VMSetIdEvent(VMSetIdEvent *ev);
	~VMSetIdEvent();
	void consume();
	const virtual string getEventName();
};

//===========================================================================================================
//
//          VMTapEvent  (class)
//
//===========================================================================================================

class VMTapEvent : public Event {
public:
	BlinkyBlocksBlock *concernedBlock;

	VMTapEvent(uint64_t, BlinkyBlocksBlock *conBlock);
	VMTapEvent(VMTapEvent *ev);
	~VMTapEvent();
	void consume();
	const virtual string getEventName();
};

/*
//===========================================================================================================
//
//          VMStartComputationEvent  (class)
//
//===========================================================================================================

class VMStartComputationEvent : public Event {
public:
	MultiCoresBlock *concernedBlock;
	uint64_t duration;

	VMStartComputationEvent(uint64_t, MultiCoresBlock *conBlock, uint64_t dur);
	VMStartComputationEvent(VMStartComputationEvent *ev);
	~VMStartComputationEvent();
	void consume();
	const virtual string getEventName();
};

//===========================================================================================================
//
//          VMEndComputationEvent  (class)
//
//===========================================================================================================

class VMEndComputationEvent : public Event {
public:
	MultiCoresBlock *concernedBlock;
	uint64_t duration;

	VMEndComputationEvent(uint64_t, MultiCoresBlock *conBlock);
	VMEndComputationEvent(VMEndComputationEvent *ev);
	~VMEndComputationEvent();
	void consume();
	const virtual string getEventName();
};

//===========================================================================================================
//
//          VMStartTransmissionEvent  (class)
//
//===========================================================================================================

class VMStartTransmissionEvent : public Event {
public:
	MultiCoresBlock *concernedBlock;
	unsigned int messageSize;
	int destBlockId;

	VMStartTransmissionEvent(uint64_t, MultiCoresBlock *conBlock, int dbId, unsigned int mSize);
	VMStartTransmissionEvent(VMStartTransmissionEvent *ev);
	~VMStartTransmissionEvent();
	void consume();
	const virtual string getEventName();
};
*/
} // BlinkyBlocks namespace


#endif /* BLINKYBLOCKSEVENTS_H_ */
