/*
 * scheduler.h
 *
 *  Created on: 19 févr. 2013
 *      Author: dom
 */

#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include <map>
#include <set>
#include <boost/interprocess/sync/interprocess_semaphore.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include "events.h"
#include "message.h"

using namespace std;
using namespace boost::interprocess;
using namespace boost;
using boost::asio::ip::udp;

#define SCHEDULER_MODE_FASTEST	1
#define SCHEDULER_MODE_REALTIME	2

#define VM_MESSAGE_TYPE_START_SIMULATION        1
#define VM_MESSAGE_TYPE_END_SIMULATION          2
#define VM_MESSAGE_TYPE_CREATE_LINK             3
#define VM_MESSAGE_TYPE_COMPUTATION_LOCK        4
#define VM_MESSAGE_TYPE_COMPUTATION_UNLOCK      5
#define VM_MESSAGE_TYPE_SEND_MESSAGE            6
#define VM_MESSAGE_TYPE_RECEIVE_MESSAGE         7
#define VM_MESSAGE_TYPE_WAIT_FOR_MESSAGE		8

#ifdef DEBUG_VM_MESSAGES
#define VM_TRACE_MESSAGE(__mes)				Scheduler::trace(__mes.printVMMessage());
#else
#define VM_TRACE_MESSAGE(__mes)
#endif

typedef struct VMMessage_tt {
        uint64_t messageType;
        uint64_t param1;
        uint64_t param2;
        uint64_t param3;
} VMMessage_t_old;

class VMMessage {
public:
    uint64_t messageType;
    uint64_t param1;
    uint64_t param2;
    uint64_t param3;

    string printVMMessage() {
    	stringstream ss;
    	switch(messageType) {
    	case VM_MESSAGE_TYPE_START_SIMULATION:
    		ss << "Message from VM : START_SIMULATION ";
    		ss << "( " << param1 << " blocks)";
    		break;
    	case VM_MESSAGE_TYPE_END_SIMULATION:
    		ss << "Message from VM : END_SIMULATION";
    		break;
    	case VM_MESSAGE_TYPE_CREATE_LINK:
    		ss << "Message from VM : CREATE_LINK ";
    		ss << "from " << param1 << " to " << param2;
    		break;
    	case VM_MESSAGE_TYPE_COMPUTATION_LOCK:
    		ss << "Message from VM : COMPUTATION_LOCK ";
    		ss << "to block " << param1 << " for " << param2 << " microseconds";
    		break;
    	case VM_MESSAGE_TYPE_COMPUTATION_UNLOCK:
    		ss << "Message to VM : COMPUTATION_UNLOCK ";
    		ss << "from block " << param1 << " at t=" << param2;
    		break;
    	case VM_MESSAGE_TYPE_SEND_MESSAGE:
    		ss << "Message from VM : SEND_MESSAGE ";
    		ss << param3 << " bytes from block " << param1 << " to block " << param2;
    		break;
    	case VM_MESSAGE_TYPE_RECEIVE_MESSAGE:
    		ss << "Message to VM : RECEIVE_MESSAGE ";
    		ss << "block " << param1 << " received " << param3 << " bytes at t=" << param2;
    		break;
    	case VM_MESSAGE_TYPE_WAIT_FOR_MESSAGE:
    		ss << "Message from VM : WAIT_FOR_MESSAGE ";
    		ss << "block " << param1;
    		break;
    	default:
    		ss << " *** UNKNOWN MESSAGE TYPE ***";
    		break;
    	}
    	return(ss.str());
    }
};

class VMDataMessage : public Message {
protected:
	unsigned int dataSize;
public:
	VMDataMessage(unsigned int s):Message() {
		dataSize = s;
	}
	virtual unsigned int size() {
		unsigned int baseSize = Message::size();
		return(baseSize+dataSize);
	}
};

class Scheduler {
protected:
	static uint64_t currentDate;
	static uint64_t maximumDate;

	static multimap<uint64_t,EventPtr> eventsList;
	static int listSize, largestListSize;

	static interprocess_semaphore sem_schedulerStart;
	static boost::thread *schedulerThread;
	static void *startPaused();
	static int schedulerMode;

	//static boost::mutex mutScheduler;
	static boost::interprocess::interprocess_mutex mutex_schedule;
	static boost::interprocess::interprocess_mutex mutex_undefinedBlocksSet;

	static set<unsigned int> undefinedBlocksSet;

	static boost::asio::io_service *ios;
	static udp::socket *socket;
	static udp::endpoint VMEndpoint;

public:
	Scheduler();
	~Scheduler();

	static bool schedule(Event *ev);
	static uint64_t now();
	static void trace(string message);
	static void start(int mode);
	static void waitForSchedulerEnd();

	static void lock();
	static void unlock();

	static void addUndefinedBlock(unsigned int id) {
		undefinedBlocksSet.insert(id);
	}
	static void removeUndefinedBlock(unsigned int id) {
		undefinedBlocksSet.erase(id);
	}
	static bool undefinedBlocksSetIsEmpty() {
		return(undefinedBlocksSet.empty());
	}
	static bool isBlockUndefined(unsigned int id) {
//		printUndefinedBlocksSet();
		set<unsigned int>::iterator res;
		res = undefinedBlocksSet.find(id);
		if (res != undefinedBlocksSet.end()) {
			return(true);
		} else {
			return(false);
		}
	}
	static void printUndefinedBlocksSet() {
		set<unsigned int>::iterator it = undefinedBlocksSet.begin();
		cout << "undefined Blocks set : ";
		while (it != undefinedBlocksSet.end()) {
			cout << (*it) << " ";
			it++;
		}
		cout << endl;
	}
	static void waitForVMMessage();
	static void sendMessageToVM(VMMessage message);

};

#endif /* SCHEDULER_H_ */
