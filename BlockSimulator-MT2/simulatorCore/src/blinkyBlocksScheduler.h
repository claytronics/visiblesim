/*
 * blinkyBlocksScheduler.h
 *
 *  Created on: 23 mars 2013
 *      Author: dom
 */

#ifndef BLINKYBLOCKSSCHEDULER_H_
#define BLINKYBLOCKSSCHEDULER_H_

#include "scheduler.h"
#include "network.h"
#include "blinkyBlocksBlock.h"
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/interprocess/sync/interprocess_semaphore.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include "trace.h"
#include <set>

using namespace boost;

namespace BlinkyBlocks {

class BlinkyBlocksScheduler : public BaseSimulator::Scheduler {
protected:
	BlinkyBlocksScheduler();
	virtual ~BlinkyBlocksScheduler();
	void* startPaused(/*void *param */);
	
	//boost::interprocess::interprocess_mutex mutex_schedule;
	boost::thread *schedulerThread;
	int schedulerMode;
	
	set<int> EndComputationBlocksSet;

public:

	boost::interprocess::interprocess_semaphore *sem_schedulerStart;
	static void createScheduler();
	static void deleteScheduler();
	static BlinkyBlocksScheduler* getScheduler() {
		assert(scheduler != NULL);
		return((BlinkyBlocksScheduler*)scheduler);
	}

	void printInfo() {
		OUTPUT << "I'm a BlinkyBlocksScheduler" << endl;
	}
	
	void start(int mode);

	void waitForSchedulerEnd() {
		schedulerThread->join();
	}
	
	//void lock();
	//void unlock();
	//bool scheduleLock(Event *ev) {lock(); bool ret = schedule(ev); unlock(); return ret;};
	
	// stop for good
	void stop();	
	void pause(int timestamp);
	void unPause();
	
	void addEndComputationBlock(int id) {
		EndComputationBlocksSet.insert(id);
	}
	void removeEndComputationBlock(int id) {
		EndComputationBlocksSet.erase(id);
	}
	bool EndComputationBlocksSetIsEmpty() {
		return(EndComputationBlocksSet.empty());
	}
	
	bool hasBlockEndComputation(int id) {
//		printEndComputationBlocksSet();
		set<int>::iterator res;
		res = EndComputationBlocksSet.find(id);
		if (res != EndComputationBlocksSet.end()) {
			return(true);
		} else {
			return(false);
		}
	}
	
	void printEndComputationBlocksSet() {
		set<int>::iterator it = EndComputationBlocksSet.begin();
		cout << "undefined Blocks set : ";
		while (it != EndComputationBlocksSet.end()) {
			cout << (*it) << " ";
			it++;
		}
		cout << endl;
	}
	
	// NOT TESTED
	bool isPaused() {
		bool r = sem_schedulerStart->try_wait();
		if (r) {
			sem_schedulerStart->post();
		}
		return !r;
	}
	
	bool schedule(Event *ev);
	
	inline int getMode() { return schedulerMode; }
		
};

inline void createScheduler() {
	BlinkyBlocksScheduler::createScheduler();
}

inline void deleteScheduler() {
	BlinkyBlocksScheduler::deleteScheduler();
}

inline BlinkyBlocksScheduler* getScheduler() { return(BlinkyBlocksScheduler::getScheduler()); }

} // BlinkyBlocks namespace

#endif /* BLINKYBLOCKSSCHEDULER_H_ */
