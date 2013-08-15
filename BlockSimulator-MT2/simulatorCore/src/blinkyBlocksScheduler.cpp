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
	/* sleep for a while, to be sure that the schedulerThread will be
	* killed before destroying all the events.
	*/ 
	//usleep(10000);
}

void BlinkyBlocksScheduler::createScheduler() {
	scheduler = new BlinkyBlocksScheduler();
}

void BlinkyBlocksScheduler::deleteScheduler() {
	delete((BlinkyBlocksScheduler*)scheduler);
}

void *BlinkyBlocksScheduler::startPaused(/*void *param*/) {

	uint64_t systemCurrentTime, systemCurrentTimeMax, pausedTime;
	pausedTime = 0;
	int seed = 500;
	srand (seed);
	
	//usleep(1000000);
	OUTPUT << "\033[1;33mScheduler Mode :" << schedulerMode << "\033[0m" << endl;
#ifndef TEST_DETER
	sem_schedulerStart->wait(); // wait for 'r' or 'R'
	if (BlinkyBlocksVM::isInDebuggingMode()) {
		sem_schedulerStart->wait(); // wait for "run" in the debugger
	}
#else
	schedulerMode = SCHEDULER_MODE_FASTEST_1;
#endif
	
	while (state == NOTREADY) {usleep(5000);} // use the semaphore instead
	state = RUNNING;
	checkForReceivedVMCommands();
	//BaseSimulator::getScheduler()->schedule(new CodeEndSimulationEvent(BaseSimulator::getScheduler()->now()+100000));
	uint64_t systemStartTime, systemStopTime, reachedDate;
	multimap<uint64_t, EventPtr>::iterator first, tmp;
	EventPtr pev;
	systemStartTime = (glutGet(GLUT_ELAPSED_TIME))*1000;
	OUTPUT << "\033[1;33m" << "Scheduler : start order received " << systemStartTime << "\033[0m" << endl;
	switch (schedulerMode) {
		case SCHEDULER_MODE_FASTEST_1:
		cout << "start SCHEDULER_MODE_FASTEST_1" << endl;
		BaseSimulator::getScheduler()->schedule(new CodeEndSimulationEvent(BaseSimulator::getScheduler()->now()+100000));
		while (!eventsMap.empty()) {
			do {
				lock();
				first = eventsMap.begin();		
				pev = (*first).second;
				unlock();
				if (getWorld()->dateHasBeenReachedByAll(pev->date))
					break;
					
				waitForOneVMCommand();
				checkForReceivedVMCommands();
			} while (true);
#ifdef TEST_DETER
			//cout << "date " << now() << " has been reached" << endl;
#endif
			currentDate = pev->date;
#ifdef TEST_DETER
//			cout << " Event: date: "<< now() << " process event " << pev->getEventName() << "(" << pev->eventType << ")" << endl;
//			if (pev->getConcernedBlock() != NULL) { 
//				BlockEvent *pevbc = (BlockEvent*) pev.get();
//				cout << pevbc->getConcernedBlock()->blockId << " RANDOM NUMBER : " << pevbc->randomNumber << endl;			
//			}
#endif
			cout << " Event: date: "<< now() << " process event " << pev->getEventName() << "(" << pev->eventType << ")" << endl;
			pev->consume();
			lock();
			eventsMap.erase(first);
			eventsMapSize--;
			unlock();
			checkForReceivedVMCommands();
		}
		cout << "scheduler end at "<< now() << "..." << endl;
#ifdef TEST_DETER
		//glutLeaveMainLoop();
		getWorld()->killAllVMs();
		exit(0);
#endif
		break;
		case SCHEDULER_MODE_FASTEST_2:		
		cout << "start SCHEDULER_MODE_FASTEST_2" << endl;
		BaseSimulator::getScheduler()->schedule(new CodeEndSimulationEvent(BaseSimulator::getScheduler()->now()+100000));

			while (!eventsMap.empty()) {
				lock();						
				first = eventsMap.begin();		
				pev = (*first).second;			
				currentDate = pev->date;
				unlock();
				pev->consume();
				lock();
				eventsMap.erase(first);
				eventsMapSize--;
				unlock();
				checkForReceivedVMCommands();
				}
				cout << "scheduler end at "<< now() << "..." << endl;
#ifdef TEST_DETER
				getWorld()->killAllVMs();
				//glutLeaveMainLoop();		
				exit(0);
#endif
			break;

		case SCHEDULER_MODE_REALTIME:
			OUTPUT << "Realtime mode scheduler\n";
			cout << "start  SCHEDULER_MODE_REALTIME" << endl;
			while (state != ENDED) {
				systemCurrentTime = ((uint64_t)glutGet(GLUT_ELAPSED_TIME))*1000 - pausedTime;
				systemCurrentTimeMax = systemCurrentTime - systemStartTime;
				currentDate = systemCurrentTimeMax;
				checkForReceivedVMCommands();
				while (true) {
						lock();
						if (eventsMap.empty()) { unlock(); break;}
						first=eventsMap.begin();
						pev = (*first).second;
						if(pev->date > systemCurrentTimeMax) { unlock(); break;}
						currentDate = pev->date;
						unlock();
						// may call schedule(), which contains lock();
						pev->consume();
						lock();
						eventsMap.erase(first);
						eventsMapSize--;
						unlock();
						// may call schedule(), which contains lock();
						checkForReceivedVMCommands();
				}
				if (state == PAUSED) {
					int pauseBeginning = ((uint64_t)glutGet(GLUT_ELAPSED_TIME))*1000;
					sem_schedulerStart->wait();
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
	BlinkyBlocksScheduler* sbs = (BlinkyBlocksScheduler*)scheduler;
	sbs->schedulerMode = mode;
	sbs->sem_schedulerStart->post();
}

void BlinkyBlocksScheduler::pause(uint64_t date) {
	getScheduler()->schedule(new VMDebugPauseSimEvent(date));
}

void BlinkyBlocksScheduler::unPause() {
	BlinkyBlocksScheduler* sbs = (BlinkyBlocksScheduler*)scheduler;
	sbs->sem_schedulerStart->post();
	OUTPUT << "unpause sim" << endl;
}

void BlinkyBlocksScheduler::stop(uint64_t date){
	if (!BlinkyBlocksVM::isInDebuggingMode()) {
		// stop all the VMs
		getWorld()->stopBlock(date, -1);
	}
	getScheduler()->schedule(new CodeEndSimulationEvent(date));
	//if (state == NOTSTARTED || state == PAUSED) {
		sem_schedulerStart->post(); // resume the simulation if it is paused
		sem_schedulerStart->post();
	//}
}

bool BlinkyBlocksScheduler::schedule(Event *ev) {
	assert(ev != NULL);
	stringstream info;

	EventPtr pev(ev);

	OUTPUT << "BlinkyBlocksScheduler: Schedule a " << pev->getEventName() << " (" << ev->id << ") with " << ev->priority << endl;

	if (pev->date < Scheduler::currentDate) {
		cout << "ERROR : An event cannot be schedule in the past !\n" << endl;
		OUTPUT << "ERROR : An event cannot be schedule in the past !\n";
	    OUTPUT << "current time : " << Scheduler::currentDate << endl;
	    OUTPUT << "ev->eventDate : " << pev->date << endl;
	    OUTPUT << "ev->getEventName() : " << pev->getEventName() << endl;
	    return(false);
	}

	if (pev->date > maximumDate) {
		OUTPUT << "WARNING : An event should not be schedule beyond the end of simulation date !\n";
		OUTPUT << "pev->date : " << pev->date << endl;
		OUTPUT << "maximumDate : " << maximumDate << endl;
	    return(false);
	}
	
	lock();
	switch (schedulerMode) {
	case SCHEDULER_MODE_REALTIME:
		eventsMap.insert(pair<uint64_t, EventPtr>(pev->date,pev));
		break;
	case SCHEDULER_MODE_FASTEST_1:
	case SCHEDULER_MODE_FASTEST_2:
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
		break;
	}
	eventsMapSize++;

	if (largestEventsMapSize < eventsMapSize) largestEventsMapSize = eventsMapSize;

	unlock();
	return(true);
}

} // BlinkyBlocks namespace
