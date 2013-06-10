/*
 * scheduler.cpp
 *
 *  Created on: 23 mars 2013
 *      Author: dom
 */

#include <iostream>
#include <stdlib.h>
#include "assert.h"
#include "scheduler.h"

using namespace std;

namespace BaseSimulator {

Scheduler *Scheduler::scheduler=NULL;

Scheduler::Scheduler() {
	cout << "Scheduler constructor" << endl;

	if (sizeof(uint64_t) != 8) {
		cerr << "\033[1;31m" << "ERROR : Scheduler requires 8bytes integer that are not available on this computer" << "\033[0m" << endl;
		exit(EXIT_FAILURE);
	}

	if (scheduler == NULL) {
		scheduler = this;
	} else {
		cerr << "\033[1;31m" << "Only one Scheduler instance can be created, aborting !" << "\033[0m" << endl;
		exit(EXIT_FAILURE);
	}

	currentDate = 0;
	maximumDate = 60000000;

	eventsMapSize = 0;
	largestEventsMapSize = 0;
}

Scheduler::~Scheduler() {
	cout << "Scheduler destructor" << endl;
}

bool Scheduler::schedule(Event *ev) {
	assert(ev != NULL);
	stringstream info;

	EventPtr pev(ev);

	info << "Schedule a " << pev->getEventName() << " (" << ev->id << ")";
	//MODIF NICO : cette ligne me spam trop l'affichage^^
	//~ trace(info.str());

//	lock();

	if (pev->date < Scheduler::currentDate) {
		cout << "ERROR : An event cannot be schedule in the past !\n";
	    cout << "current time : " << Scheduler::currentDate << endl;
	    cout << "ev->eventDate : " << pev->date << endl;
	    cout << "ev->getEventName() : " << pev->getEventName() << endl;
	    return(false);
	}

	if (pev->date > maximumDate) {
		cout << "WARNING : An event should not be schedule beyond the end of simulation date !\n";
		cout << "pev->date : " << pev->date << endl;
		cout << "maximumDate : " << maximumDate << endl;
	    return(false);
	}
	eventsMap.insert(pair<uint64_t, EventPtr>(pev->date,pev));

	eventsMapSize++;

	if (largestEventsMapSize < eventsMapSize) largestEventsMapSize = eventsMapSize;

//	unlock();

	return(true);
}

void Scheduler::removeEventsToBlock(BuildingBlock *bb) {
	multimap<uint64_t,EventPtr>::iterator im = eventsMap.begin();
	BuildingBlock *cb=NULL;
	while (im!=eventsMap.end()) {
		cb=(*im).second->getConcernedBlock();
		if (cb==bb) {
			multimap<uint64_t,EventPtr>::iterator im2 = im;
			im--;
			eventsMap.erase(im2);
		} else im++;
	}
}

uint64_t Scheduler::now() {
	return(currentDate);
}

void Scheduler::trace(string message) {
	mutex_trace.lock();
	cout.precision(6);
	cout << fixed << (double)(currentDate)/1000000 << " " << message << endl;
	mutex_trace.unlock();
}
/*
void Scheduler::lock() {
	mutex_schedule.lock();
}

void Scheduler::unlock() {
	mutex_schedule.unlock();
}
*/
} // BaseSimulator namespace
