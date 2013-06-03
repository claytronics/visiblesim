/*
 * events.h
 *
 *  Created on: 16 févr. 2013
 *      Author: dom
 */

#ifndef EVENTS_H_
#define EVENTS_H_

#include <string>
#include <sstream>
#include <boost/shared_ptr.hpp>
#include "buildingBlock.h"

using namespace std;

#define EVENT_GENERIC						1
#define EVENT_CODE_START					2
#define EVENT_NI_START_TRANSMITTING			3
#define EVENT_NI_STOP_TRANSMITTING			4
#define EVENT_NI_RECEIVE					5
#define EVENT_NI_ENQUEUE_OUTGOING_MESSAGE 	6
#define EVENT_PROCESS_LOCAL_EVENT			7
#define EVENT_BLOCK_TIMER					8

#define EVENT_VM_START_COMPUTATION			1000
#define EVENT_VM_END_COMPUTATION			1001
#define EVENT_VM_START_TRANSMISSION			1002

class Event;

typedef boost::shared_ptr<Event> EventPtr;

#ifdef DEBUG_EVENTS
#define EVENT_CONSTRUCTOR_INFO()			(cout << getEventName() << " constructor (" << id << ")" << endl)
#define EVENT_DESTRUCTOR_INFO()				(cout << getEventName() << " destructor (" << id << ")" << endl)
#else
#define EVENT_CONSTRUCTOR_INFO()
#define EVENT_DESTRUCTOR_INFO()
#endif

#ifdef DEBUG_CONSUME_EVENTS
#define EVENT_CONSUME_INFO()				({ stringstream debuginfo1; debuginfo1 << "consuming event " << id << " (" << getEventName() << ")"; Scheduler::trace(debuginfo1.str());})
#else
#define EVENT_CONSUME_INFO()
#endif


//===========================================================================================================
//
//          Event  (class)
//
//===========================================================================================================


class Event {
protected:
	static int nextId;
	static unsigned int nbEvents;
public:

	int id;				// unique ID of the event (mainly for debugging purpose)
	uint64_t date;		// time at which the event will be processed. 0 means simulation start
	int eventType;		// see the various types at the beginning of this file

	Event(uint64_t t);
	Event(Event *ev);
	virtual ~Event();

	virtual void consume() = 0;
	virtual string getEventName();

	static unsigned int getNextId();
	static unsigned int getNbEvents();
};

//===========================================================================================================
//
//          CodeStartEvent  (class)
//
//===========================================================================================================

class CodeStartEvent : public Event {
public:
	BuildingBlock *concernedBlock;

	CodeStartEvent(uint64_t, BuildingBlock *conBlock);
	~CodeStartEvent();
	void consume();
	virtual string getEventName();
};

//===========================================================================================================
//
//          NetworkInterfaceStartTransmittingEvent  (class)
//
//===========================================================================================================

class NetworkInterfaceStartTransmittingEvent : public Event {
public:
	P2PNetworkInterface *interface;

	NetworkInterfaceStartTransmittingEvent(uint64_t, P2PNetworkInterface *ni);
	~NetworkInterfaceStartTransmittingEvent();
	void consume();
	virtual string getEventName();
};

//===========================================================================================================
//
//          NetworkInterfaceStopTransmittingEvent  (class)
//
//===========================================================================================================

class NetworkInterfaceStopTransmittingEvent : public Event {
public:
	P2PNetworkInterface *interface;

	NetworkInterfaceStopTransmittingEvent(uint64_t, P2PNetworkInterface *ni);
	~NetworkInterfaceStopTransmittingEvent();
	void consume();
	virtual string getEventName();
};


//===========================================================================================================
//
//          NetworkInterfaceReceiveEvent  (class)
//
//===========================================================================================================

class NetworkInterfaceReceiveEvent : public Event {
public:
	P2PNetworkInterface *interface;
	MessagePtr message;
	NetworkInterfaceReceiveEvent(uint64_t,P2PNetworkInterface *ni, MessagePtr mes);
	~NetworkInterfaceReceiveEvent();
	void consume();
	virtual string getEventName();
};

//===========================================================================================================
//
//          NetworkInterfaceEnqueueOutgoingEvent  (class)
//
//===========================================================================================================

class NetworkInterfaceEnqueueOutgoingEvent : public Event {
public:
	MessagePtr message;
	P2PNetworkInterface *sourceInterface;

	NetworkInterfaceEnqueueOutgoingEvent(uint64_t, Message *mes, P2PNetworkInterface *ni);
	~NetworkInterfaceEnqueueOutgoingEvent();
	void consume();
	virtual string getEventName();
};

//===========================================================================================================
//
//          ProcessLocalEvent  (class)
//
//===========================================================================================================

class ProcessLocalEvent : public Event {
public:
	BuildingBlock *concernedBlock;

	ProcessLocalEvent(uint64_t, BuildingBlock *conBlock);
	~ProcessLocalEvent();
	void consume();
	virtual string getEventName();
};

//===========================================================================================================
//
//          BlockTimerEvent  (class)
//
//===========================================================================================================

class BlockTimerEvent: public Event {
private:
	BuildingBlock *concernedBlock;
public:
	unsigned int timerId;

	BlockTimerEvent(uint64_t t, BuildingBlock *block, unsigned int tId);
	BlockTimerEvent(BlockTimerEvent *ev);
	~BlockTimerEvent();
	void consume();
	virtual string getEventName();
};

//===========================================================================================================
//
//          VMStartComputationEvent  (class)
//
//===========================================================================================================

class VMStartComputationEvent : public Event {
public:
	BuildingBlock *concernedBlock;
	uint64_t duration;

	VMStartComputationEvent(uint64_t, BuildingBlock *conBlock, uint64_t dur);
	VMStartComputationEvent(VMStartComputationEvent *ev);
	~VMStartComputationEvent();
	void consume();
	virtual string getEventName();
};

//===========================================================================================================
//
//          VMEndComputationEvent  (class)
//
//===========================================================================================================

class VMEndComputationEvent : public Event {
public:
	BuildingBlock *concernedBlock;
	uint64_t duration;

	VMEndComputationEvent(uint64_t, BuildingBlock *conBlock);
	VMEndComputationEvent(VMEndComputationEvent *ev);
	~VMEndComputationEvent();
	void consume();
	virtual string getEventName();
};

//===========================================================================================================
//
//          VMStartTransmissionEvent  (class)
//
//===========================================================================================================

class VMStartTransmissionEvent : public Event {
public:
	BuildingBlock *concernedBlock;
	unsigned int messageSize;
	unsigned int destBlockId;

	VMStartTransmissionEvent(uint64_t, BuildingBlock *conBlock,  unsigned int dbId, unsigned int mSize);
	VMStartTransmissionEvent(VMStartTransmissionEvent *ev);
	~VMStartTransmissionEvent();
	void consume();
	virtual string getEventName();
};

#endif /* EVENTS_H_ */
