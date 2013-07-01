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
	
	set<int> undefinedBlocksSet;

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
	void pauseSimulation(int timestamp);
	void unPauseSimulation();
	
	void addUndefinedBlock(int id) {
		undefinedBlocksSet.insert(id);
	}
	void removeUndefinedBlock(int id) {
		undefinedBlocksSet.erase(id);
	}
	bool undefinedBlocksSetIsEmpty() {
		return(undefinedBlocksSet.empty());
	}
	
	bool isPaused() {
		bool r = sem_schedulerStart->try_wait();
		if (r) {
			sem_schedulerStart->post();
		}
		return !r;
	}
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
