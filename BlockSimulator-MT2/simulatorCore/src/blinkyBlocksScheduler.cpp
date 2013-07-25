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
	sem_schedulerStart = new boost::interprocess::interprocess_semaphore(0);
	//schedulerMode = SCHEDULER_MODE_FASTEST;
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

/*
static bool sortEvents(multimap<uint64_t, EventPtr>::iterator f, multimap<uint64_t, EventPtr>::iterator s) {
	BaseSimulator::BuildingBlock *b1 = f->second->getConcernedBlock();
	BaseSimulator::BuildingBlock *b2 = s->second->getConcernedBlock();
	if(!b1 || !b2) {
		return true;
	}
	return (b1->blockId*rand() > b2->blockId*rand());
} */

void *BlinkyBlocksScheduler::startPaused(/*void *param*/) {
	
	bool mustStop;
	uint64_t systemCurrentTime, systemCurrentTimeMax;
	int seed = 500;
	srand (seed);
	
	usleep(1000000);
	OUTPUT << "\033[1;33mScheduler Mode :" << schedulerMode << "\033[0m" << endl;
	
	sem_schedulerStart->wait();
	checkForReceivedVMMessages();
	//BaseSimulator::getScheduler()->schedule(new CodeEndSimulationEvent(BaseSimulator::getScheduler()->now()+100000));
	int systemStartTime, systemStopTime;
	multimap<uint64_t, EventPtr>::iterator first, tmp;
	EventPtr pev;
	systemStartTime = (glutGet(GLUT_ELAPSED_TIME))*1000;
	OUTPUT << "\033[1;33m" << "Scheduler : start order received " << systemStartTime << "\033[0m" << endl;

	switch (schedulerMode) {
		case SCHEDULER_MODE_FASTEST:
			//cout << "START SCHEDULER FASTEST MODE" << endl;
			while (!eventsMap.empty()) {
				lock();						
				first = eventsMap.begin();		
				pev = (*first).second;			
				currentDate = pev->date;
				unlock();
				//cout << "consume " << pev->date << endl;
				pev->consume();
				lock();
				eventsMap.erase(first);
				eventsMapSize--;
				unlock();
				checkForReceivedVMMessages();
				}
			//cout << "eventsMap empty!" << endl;
			break;
		case SCHEDULER_MODE_REALTIME:
			OUTPUT << "Realtime mode scheduler\n";
			mustStop = false;
			while (!mustStop) {
				systemCurrentTime = ((uint64_t)glutGet(GLUT_ELAPSED_TIME))*1000;
				systemCurrentTimeMax = systemCurrentTime - systemStartTime;
				currentDate = systemCurrentTimeMax;
				checkForReceivedVMMessages();
				while (true) {
						lock();
						if (eventsMap.empty()) { unlock(); break;}
						first=eventsMap.begin();
						pev = (*first).second;
						if(pev->date > systemCurrentTimeMax) { unlock(); break;}
						if (pev->eventType == EVENT_END_SIMULATION) {
							OUTPUT << "end simulation" << endl;
							mustStop = true;
						}
						currentDate = pev->date;
						unlock();
						// may call schedule(), which contains lock();
						pev->consume();
						lock();
						eventsMap.erase(first);
						eventsMapSize--;
						unlock();
						// may call schedule(), which contains lock();
						checkForReceivedVMMessages();
				}
#ifdef WIN32
				Sleep(5);
#else
				usleep(5000);
#endif
			}
			/*
			while(!mustStop || !eventsMap.empty()) {
				systemCurrentTime = ((uint64_t)glutGet(GLUT_ELAPSED_TIME))*1000;
				systemCurrentTimeMax = systemCurrentTime - systemStartTime;
				//lock();
				//readIncomingMessages();
				if(!eventsMap.empty())	{
					first=eventsMap.begin();
					pev = (*first).second;
					while (!eventsMap.empty() && pev->date <= systemCurrentTimeMax) {
						first=eventsMap.begin();
						pev = (*first).second;
						if (pev->eventType == EVENT_END_SIMULATION) {
							mustStop = true;
							OUTPUT << "END EVENT" << endl;
						}
						// traitement du mouvement des objets physiques
						//Physics::update(ev->heureEvenement);
						currentDate = pev->date;
						//lock();
						pev->consume();
						//unlock();
						//pev->nbRef--;
						//listeEvenements.pop_front();
						eventsMap.erase(first);
						eventsMapSize--;
						//ev = *(listeEvenements.begin());
						//first=eventsMap.begin();
						//pev = (*first).second;
						readIncomingMessages();
					}
					systemCurrentTime = systemCurrentTimeMax;
					if (!eventsMap.empty()) {
						//ev = *(listeEvenements.begin());
						first=eventsMap.begin();
						pev = (*first).second;
						#ifdef WIN32
						Sleep(5);
						#else
						usleep(5000);
						#endif
					}
				}
			} */
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
	//sem_schedulerStart->post();
}

void BlinkyBlocksScheduler::pause(int timestamp) {
	getScheduler()->schedule(new VMDebugPauseSimEvent(timestamp));
}

void BlinkyBlocksScheduler::unPause() {
	BlinkyBlocksScheduler* sbs = (BlinkyBlocksScheduler*)scheduler;
	sbs->sem_schedulerStart->post();
	OUTPUT << "unpause sim" << endl;
}

void BlinkyBlocksScheduler::stop(){
	// stop all the VMs
	getWorld()->stopBlock(-1);
	getScheduler()->schedule(new CodeEndSimulationEvent(BaseSimulator::getScheduler()->now()));
	sem_schedulerStart->post(); // resume the simulation if it is paused
}

bool BlinkyBlocksScheduler::schedule(Event *ev) {
	assert(ev != NULL);
	stringstream info;

	EventPtr pev(ev);

	OUTPUT << "BlinkyBlocksScheduler: Schedule a " << pev->getEventName() << " (" << ev->id << ") with " << ev->priority << endl;
	//cout << "BlinkyBlocksScheduler: Schedule a " << pev->getEventName() << " (" << ev->id << ") with " << ev->priority << "for " << pev->date << endl;
	//MODIF NICO : cette ligne me spam trop l'affichage^^
	//~ trace(info.str());

	if (pev->date < Scheduler::currentDate) {
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
	case SCHEDULER_MODE_FASTEST:
		if (eventsMap.count(pev->date) > 1 && pev->getConcernedBlock() != NULL) {
			//multimap<uint64_t, EventPtr>::iterator it = eventsMap.find(pev->date);
			std::pair<multimap<uint64_t, EventPtr>::iterator,multimap<uint64_t, EventPtr>::iterator> range = eventsMap.equal_range(pev->date);
			multimap<uint64_t, EventPtr>::iterator it = range.first;
			while (it != range.second) {
				if (it->second->getConcernedBlock() == NULL) {
					it++;
					continue;
				}
				BlockEvent *bev1 = (BlockEvent*) it->second.get();
				BlockEvent *bev2 = (BlockEvent*) it->second.get();					
				if (bev1->randomNumber >= bev2->randomNumber) {
					break;
				}
				it++;
			}
			if (it == range.second) {
				it--;
			}
			//advance (it, rand() % eventsMap.count(pev->date));
			eventsMap.insert(it, pair<uint64_t, EventPtr>(pev->date,pev));
		} else {
			if (pev->getConcernedBlock() == NULL) {
				//cout << "not a block event: " << pev->getEventName() << endl;
				//cout << "date " << pev->date << endl;
			}
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
	//cout << "schedule end" << endl;
	return(true);
}

} // BlinkyBlocks namespace
