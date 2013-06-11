/*
 * blinkyBlocksScheduler.cpp
 *
 *  Created on: 23 mars 2013
 *      Author: dom
 */

#include <iostream>
#include <stdlib.h>
#include "blinkyBlocksScheduler.h"
#include "blinkyBlocksSimulator.h"
#include "blinkyBlocksWorld.h"
#include "buildingBlock.h"
#include "blockCode.h"

using namespace std;
using namespace boost;
using boost::asio::ip::tcp;
namespace BlinkyBlocks {

BlinkyBlocksScheduler::BlinkyBlocksScheduler() {
	cout << "BlinkyBlocksScheduler constructor" << endl;
	sem_schedulerStart = new boost::interprocess::interprocess_semaphore(0);
	schedulerMode = SCHEDULER_MODE_FASTEST;
	schedulerThread = new thread(bind(&BlinkyBlocksScheduler::startPaused, this));
}

BlinkyBlocksScheduler::~BlinkyBlocksScheduler() {
	cout << "\033[1;31mBlinkyBlocksScheduler destructor\33[0m" << endl;
	delete sem_schedulerStart;
	delete schedulerThread;
}

void handler(const boost::system::error_code& error, std::size_t bytes_transferred, BlinkyBlocksBlock* bb){

cout << bb->blockId << " sends: " << bb->getBufferPtr()->param1 << endl;
		/*boost::asio::async_read(bb->getSocket(), 
				boost::asio::buffer((void*)bb->getBufferPtr(), sizeof(VMMessage_t)),
				boost::bind(handler, boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred, bb));*/
// getWorld()->getIos().run(); // synchronization...
}

void BlinkyBlocksScheduler::createScheduler() {
	scheduler = new BlinkyBlocksScheduler();
}

void BlinkyBlocksScheduler::deleteScheduler() {
	delete((BlinkyBlocksScheduler*)scheduler);
}

void *BlinkyBlocksScheduler::startPaused(/*void *param*/) {
	//MultiCoresScheduler *scheduler = (MultiCoresScheduler*)param;
	bool mustStop;
	uint64_t systemCurrentTime, systemCurrentTimeMax;

	usleep(1000000);
	cout << "\033[1;33mScheduler Mode :" << schedulerMode << endl;

	sem_schedulerStart->wait();

	int systemStartTime, systemStopTime;
	multimap<uint64_t, EventPtr>::iterator first;
	EventPtr pev;

	systemStartTime = (glutGet(GLUT_ELAPSED_TIME))*1000;
	cout << "\033[1;33m" << "Scheduler : start order received " << systemStartTime << "\033[0m" << endl;

	switch (schedulerMode) {
	case SCHEDULER_MODE_FASTEST:
		while ( (!eventsMap.empty() ) && currentDate < maximumDate) {
	    	first=eventsMap.begin();
	    	pev = (*first).second;
				pev->consume();
				eventsMap.erase(first);
				eventsMapSize--;
	    }
		break;
	case SCHEDULER_MODE_REALTIME:

		cout << "Realtime mode scheduler\n";
		mustStop = false;
	    while(!mustStop && !eventsMap.empty()) {
	      //gettimeofday(&heureGlobaleActuelle,NULL);
	    	systemCurrentTime = ((uint64_t)glutGet(GLUT_ELAPSED_TIME))*1000;
	    	systemCurrentTimeMax = systemCurrentTime - systemStartTime;
	      //ev = *(listeEvenements.begin());
	    	first=eventsMap.begin();
	    	pev = (*first).second;
	    	while (!eventsMap.empty() && pev->date <= systemCurrentTimeMax) {
	    		first=eventsMap.begin();
	    		pev = (*first).second;

			  /* traitement du mouvement des objets physiques*/
			  //Physics::update(ev->heureEvenement);
	    		currentDate = pev->date;
	    		//lock();
	    		pev->consume();
	    		//unlock();
	    		//pev->nbRef--;

	    		//listeEvenements.pop_front();
	    		eventsMap.erase(first);
	    		eventsMapSize--;
	    		//	    	  ev = *(listeEvenements.begin());
	    		//first=eventsMap.begin();
	    		//pev = (*first).second;
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

		break;
	default:
		cout << "ERROR : Scheduler mode not recognized !!" << endl;
	}

	systemStopTime = ((uint64_t)glutGet(GLUT_ELAPSED_TIME))*1000;

	cout << "\033[1;33m" << "Scheduler end : " << systemStopTime << "\033[0m" << endl;

	pev.reset();

	cout << "end time : " << currentDate << endl;
	cout << "real time elapsed : " << ((double)(systemStopTime-systemStartTime))/1000000 << endl;
//	cout << "Nombre d'événements restants en mémoire : " << Evenement::nbEvenements << endl;
//	cout << "Nombre de messages restants en mémoire : " << Message::nbMessages << endl;
	cout << "Maximum sized reached by the events list : " << largestEventsMapSize << endl;
	cout << "Size of the events list at the end : " << eventsMap.size() << endl;
	cout << "Number of events processed : " << Event::getNextId() << endl;
	cout << "Events(s) left in memory before destroying Scheduler : " << Event::getNbLivingEvents() << endl;
	cout << "Message(s) left in memory before destroying Scheduler : " << Message::getNbMessages() << endl;

	return(NULL);
}

void BlinkyBlocksScheduler::start(int mode) {
	BlinkyBlocksScheduler* sbs = (BlinkyBlocksScheduler*)scheduler;
	sbs->schedulerMode = mode;
	sbs->sem_schedulerStart->post();
}

} // BlinkyBlocks namespace
