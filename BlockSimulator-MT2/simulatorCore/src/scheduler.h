/*
 * scheduler.h
 *
 *  Created on: 23 mars 2013
 *      Author: dom
 */

#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include <iostream>
#include <sstream>
#include <map>
#include <inttypes.h>
#include <assert.h>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include "events.h"

using namespace std;

#define SCHEDULER_MODE_FASTEST	1
#define SCHEDULER_MODE_REALTIME	2

namespace BaseSimulator {

class Scheduler {
protected:
	static Scheduler *scheduler;

	uint64_t currentDate;
	uint64_t maximumDate;
	multimap<uint64_t,EventPtr> eventsMap;
	int eventsMapSize, largestEventsMapSize;
	boost::interprocess::interprocess_mutex mutex_schedule;
	boost::interprocess::interprocess_mutex mutex_trace;

	Scheduler();
	virtual ~Scheduler();

public:
	static Scheduler* getScheduler() {
		assert(scheduler != NULL);
		return(scheduler);
	}
	static void deleteScheduler() {
		delete(scheduler);
		// Modif Ben
		scheduler=NULL;
		// End Modif
	}

	void printInfo() {
		cout << "I'm a Scheduler" << endl;
	}

	virtual bool schedule(Event *ev);
	uint64_t now();
	virtual void trace(string message,int id=-1);
	void removeEventsToBlock(BuildingBlock *bb);

	void lock();
	void unlock();
	
	//virtual void pauseSimulation(int timestamp) {};
	//virtual void unPauseSimulation() {};

	virtual void start(int) {};
};

inline void deleteScheduler() {
	Scheduler::deleteScheduler();
}

inline Scheduler* getScheduler() { return(Scheduler::getScheduler()); }

} // BaseSimulator namespace

#endif /* SCHEDULER_H_ */
