/*
 * blinkyBlocksScheduler.h
 *
 *  Created on: 23 mars 2013
 *      Author: dom
 */

#ifndef BLINKYBLOCKSSCHEDULER_H_
#define BLINKYBLOCKSSCHEDULER_H_

#include "scheduler.h"
#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/interprocess/sync/interprocess_semaphore.hpp>

namespace BlinkyBlocks {

class BlinkyBlocksScheduler : public BaseSimulator::Scheduler {
protected:
	BlinkyBlocksScheduler();
	virtual ~BlinkyBlocksScheduler();
	void* startPaused(/*void *param */);

	boost::interprocess::interprocess_semaphore *sem_schedulerStart;
	boost::thread *schedulerThread;
	int schedulerMode;

public:
	static void createScheduler();
	static void deleteScheduler();
	static BlinkyBlocksScheduler* getScheduler() {
		assert(scheduler != NULL);
		return((BlinkyBlocksScheduler*)scheduler);
	}

	void printInfo() {
		cout << "I'm a BlinkyBlocksScheduler" << endl;
	}
	void start(int mode);

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
