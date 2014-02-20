/*
* blinkyBlocksScheduler.cpp
*
*  Created on: 23 mars 2013
*      Author: dom
*/

#include <iostream>
#include <cstdlib>
#include <algorithm>
#include "blinkyBlocksScheduler.h"
#include "blinkyBlocksSimulator.h"
#include "blinkyBlocksWorld.h"
#include "buildingBlock.h"
#include "blockCode.h"
#include "trace.h"
#include "blinkyBlocksEvents.h"

using namespace std;
using namespace boost;
using boost::asio::ip::tcp;

namespace BlinkyBlocks {

BlinkyBlocksScheduler::BlinkyBlocksScheduler() {
	OUTPUT << "BlinkyBlocksScheduler constructor" << endl;
	state = NOTREADY;
	sem_schedulerStart = new boost::interprocess::interprocess_semaphore(0);
	schedulerMode = SCHEDULER_MODE_REALTIME;
	schedulerThread = new thread(bind(&BlinkyBlocksScheduler::startPaused, this));
}

BlinkyBlocksScheduler::~BlinkyBlocksScheduler() {
	OUTPUT << "\033[1;31mBlinkyBlocksScheduler destructor\33[0m" << endl;
	delete schedulerThread;
	delete sem_schedulerStart;
}

void BlinkyBlocksScheduler::createScheduler() {
	scheduler = new BlinkyBlocksScheduler();
}

void BlinkyBlocksScheduler::deleteScheduler() {
	delete((BlinkyBlocksScheduler*)scheduler);
}


void BlinkyBlocksScheduler::SemWaitOrReadDebugMessage() {
	if (BlinkyBlocksVM::isInDebuggingMode()) {
		while(!sem_schedulerStart->try_wait()) {
			//waitForOneVMCommand();
			checkForReceivedVMCommands();
			usleep(10000);
		}
	} else {
		sem_schedulerStart->wait();
	}
}

void *BlinkyBlocksScheduler::startPaused(/*void *param*/) {

	uint64_t systemCurrentTime, systemCurrentTimeMax, pausedTime;
	pausedTime = 0;
	int seed = 500;
	srand (seed);
	bool hasProcessed = false;
	
	// 1) world ready
	// 2) user start order
	int nbSemWait = 2; 
	
	//usleep(1000000);
	OUTPUT << "\033[1;33mScheduler Mode :" << schedulerMode << "\033[0m" << endl;
#ifndef TEST_DETER
	if (BlinkyBlocksVM::isInDebuggingMode()) {
		// 3) Debugger "run"
		nbSemWait = 3;
	}	
	for (int i = 0; i < nbSemWait; i++) {
		SemWaitOrReadDebugMessage();
	}
#else
	sem_schedulerStart->wait();
	schedulerMode = SCHEDULER_MODE_FASTEST;
#endif
	state = RUNNING;
	checkForReceivedVMCommands();
	uint64_t systemStartTime, systemStopTime;
	multimap<uint64_t, EventPtr>::iterator first, tmp;
	EventPtr pev;
	systemStartTime = (glutGet(GLUT_ELAPSED_TIME))*1000;
	OUTPUT << "\033[1;33m" << "Scheduler : start order received " << systemStartTime << "\033[0m" << endl;
	switch (schedulerMode) {
		case SCHEDULER_MODE_FASTEST:
			OUTPUT << "fastest mode scheduler\n" << endl;
         BlinkyBlocksDebugger::print("Simulation starts in deterministic mode");
			while (state != ENDED) {
            do {
            while (!eventsMap.empty()) {
               hasProcessed = true;
               do {
                  lock();
                  first = eventsMap.begin();		
                  pev = (*first).second;
                  if (pev->date == now()) {
                     break;
                  }
                  if (getWorld()->dateHasBeenReachedByAll(pev->date)) {
                     break;
                  }
                  unlock();
                  waitForOneVMCommand();
               } while (true);
               currentDate = pev->date;
               pev->consume();
               eventsMap.erase(first);
               eventsMapSize--;
               unlock();
               if (state == PAUSED) {
                  if (BlinkyBlocksVM::isInDebuggingMode()) {
                     getDebugger()->handleBreakAtTimeReached(currentDate);
                  } else {
                     sem_schedulerStart->wait();
                  }
                  setState(RUNNING);
               }
               checkForReceivedVMCommands();
            }
            checkForReceivedVMCommands();
         } while (!getWorld()->equilibrium() || !eventsMap.empty());
         
         if(hasProcessed) {
            hasProcessed = false;
            ostringstream s;            
            s << "Equilibrium reached at "<< now()/1000 << ":" << (now()%1000)/10 << "ms ...";
            BlinkyBlocksDebugger::print(s.str(), false);
            if (BlinkyBlocks::getSimulator()->testMode) {
               BlinkyBlocks::getWorld()->dump();
               stop(0);
               return 0;
            }
            if (BlinkyBlocksVM::isInDebuggingMode()) {
               BlinkyBlocks::getDebugger()->handlePauseRequest();
            }
         }
         checkForReceivedVMCommands();
         usleep(5000);
      }
#ifdef TEST_DETER
		getWorld()->killAllVMs();
		exit(0);
#endif
		break;
		case SCHEDULER_MODE_REALTIME:
			OUTPUT << "Realtime mode scheduler\n" << endl;
			BlinkyBlocksDebugger::print("Simulation starts in real time mode");
			while (state != ENDED) {
				systemCurrentTime = ((uint64_t)glutGet(GLUT_ELAPSED_TIME))*1000 - pausedTime;
				systemCurrentTimeMax = systemCurrentTime - systemStartTime;
				currentDate = systemCurrentTimeMax;
				checkForReceivedVMCommands();
				while (true) {
						// Lock from here, to be sure that the element
						// is not destroyed in antother thread
						// (previously the graphic interface was doing
						// it).
						lock();
						if (eventsMap.empty()) {
							unlock();
							break;
						}
						first=eventsMap.begin();
						pev = (*first).second;
						if(pev->date > systemCurrentTimeMax) { 
							unlock();
							break;
						}
						currentDate = pev->date;
						pev->consume();
						eventsMap.erase(first);
						eventsMapSize--;
						unlock();
						checkForReceivedVMCommands();
				}
				if (state == PAUSED) {
               cout << "paused" << endl;
            	int pauseBeginning = ((uint64_t)glutGet(GLUT_ELAPSED_TIME))*1000;
					SemWaitOrReadDebugMessage();
					setState(RUNNING);
					pausedTime += ((uint64_t)glutGet(GLUT_ELAPSED_TIME))*1000 - pauseBeginning;
				}
#ifdef WIN32
				Sleep(5);
#else
				usleep(5000);
#endif
			}
			break;
		default:
			OUTPUT << "ERROR : Scheduler mode not recognized !!" << endl;
	}
	systemStopTime = ((uint64_t)glutGet(GLUT_ELAPSED_TIME))*1000;
	OUTPUT << "\033[1;33m" << "Scheduler end : " << systemStopTime << "\033[0m" << endl;
	pev.reset();
	OUTPUT << "end time : " << currentDate << endl;
	OUTPUT << "real time elapsed : " << ((double)(systemStopTime-systemStartTime))/1000000 << endl;
	//	OUTPUT << "Nombre d'événements restants en mémoire : " << Evenement::nbEvenements << endl;
	//	OUTPUT << "Nombre de messages restants en mémoire : " << Message::nbMessages << endl;
	OUTPUT << "Maximum sized reached by the events list : " << largestEventsMapSize << endl;
	OUTPUT << "Size of the events list at the end : " << eventsMap.size() << endl;
	OUTPUT << "Number of events processed : " << Event::getNextId() << endl;
	OUTPUT << "Events(s) left in memory before destroying Scheduler : " << Event::getNbLivingEvents() << endl;
	OUTPUT << "Message(s) left in memory before destroying Scheduler : " << Message::getNbMessages() << endl;
	return(NULL);
}

void BlinkyBlocksScheduler::start(int mode) {
	static bool done = false;
	if ((state == NOTSTARTED) && !done) {
		done = true;
		BlinkyBlocksScheduler* sbs = (BlinkyBlocksScheduler*)scheduler;
		sbs->schedulerMode = mode;
		sbs->sem_schedulerStart->post();
	}
}

void BlinkyBlocksScheduler::pause(uint64_t date) {
	getScheduler()->scheduleLock(new VMDebugPauseSimEvent(date));
}

void BlinkyBlocksScheduler::unPause() {
   BlinkyBlocksScheduler* sbs = (BlinkyBlocksScheduler*)scheduler;
	if (state != RUNNING) {
		sbs->sem_schedulerStart->post();
	}
	OUTPUT << "unpause sim" << endl;
}

void BlinkyBlocksScheduler::stop(uint64_t date){
	getWorld()->killAllVMs();
	schedulerThread->detach();
	setState(ENDED);
}

bool BlinkyBlocksScheduler::scheduleLock(Event *ev) {
	bool ret;
	lock();
	ret = schedule(ev);
	unlock();
	return ret;
}

bool BlinkyBlocksScheduler::schedule(Event *ev) {
	assert(ev != NULL);
	stringstream info;

	EventPtr pev(ev);

	OUTPUT << "BlinkyBlocksScheduler: Schedule a " << pev->getEventName() << " (" << ev->id << ")" << endl;

	// (pev->date > maximumDate) condition was removed because
	// Blinky Block system never ends.
	
	// (pev->date < Scheduler::currentDate) was removed because sometimes
	// it causes bugs: when the graphical interface thread starts to
	// schedule an event, and in this same interval, the date of the 
	// scheduler changes...
	
	switch (schedulerMode) {
	case SCHEDULER_MODE_REALTIME:
		eventsMap.insert(pair<uint64_t, EventPtr>(pev->date,pev));
		break;
	case SCHEDULER_MODE_FASTEST:
		if (eventsMap.count(pev->date) > 0) {
			std::pair<multimap<uint64_t, EventPtr>::iterator,multimap<uint64_t, EventPtr>::iterator> range = eventsMap.equal_range(pev->date);
			multimap<uint64_t, EventPtr>::iterator it = range.first;
			while (it != range.second) {
				if (it->second->randomNumber == 0) {
					it++;
					continue;
				}
				if (it->second->randomNumber > pev->randomNumber) {
					break;
				}
				if (it->second->randomNumber == pev->randomNumber) {
					while (it->second->randomNumber == pev->randomNumber) {
						if (it->second->getConcernedBlock()->blockId > pev->getConcernedBlock()->blockId) {
							break;
						}
						it++;
					}
					break;
				}
				it++;
			}
			eventsMap.insert(it, pair<uint64_t, EventPtr>(pev->date,pev));
		} else {
			eventsMap.insert(pair<uint64_t, EventPtr>(pev->date,pev));	
		}
		break;
	default:
		ERRPUT << "unknown scheduler mode" << endl;
		return false;
		break;
	}
	eventsMapSize++;

	if (largestEventsMapSize < eventsMapSize) largestEventsMapSize = eventsMapSize;

	return(true);
}

} // BlinkyBlocks namespace
