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
using boost::asio::ip::tcp;
//using namespace BaseSimulator;

namespace BlinkyBlocks {

BlinkyBlocksScheduler::BlinkyBlocksScheduler() {
	cout << "BlinkyBlocksScheduler constructor" << endl;
	map<int, BaseSimulator::BuildingBlock*>::iterator it;
	for(it = getWorld()->getBuildingBlocksMap().begin(); it != getWorld()->getBuildingBlocksMap().end(); it++) {
			cout << it->first << endl;
	}
}

void *BlinkyBlocksScheduler::startPaused() {
	int systemStartTime, systemStopTime;
    multimap<uint64_t, EventPtr>::iterator first;
    EventPtr pev;
   	//int s = getWorld()->getBuildingBlocksMap().size();
	map<int, BaseSimulator::BuildingBlock*>::iterator it;
	for(it = getWorld()->getBuildingBlocksMap().begin(); it != getWorld()->getBuildingBlocksMap().end(); it++) {
			cout << it->first << endl;
	}
    // sem.wait(); // attend le 1er message
	trace("\033[1;33mScheduler : created and waiting for VM connection : \033[0m");
	
	trace("\033[1;33mScheduler : VM connected and waiting for start order : \033[0m");
	return(NULL);
}

BlinkyBlocksScheduler::~BlinkyBlocksScheduler() {
	cout << "BlinkyBlocksScheduler destructor" << endl;
}


void BlinkyBlocksScheduler::createScheduler() {
	scheduler = new BlinkyBlocksScheduler();
}

void BlinkyBlocksScheduler::deleteScheduler() {
	delete((BlinkyBlocksScheduler*)scheduler);
}
/*
void startPaused() {
    
    while (!eventsMap.empty() || !undefinedBlocksSetIsEmpty()) && currentDate < maximumDate) {
        while (!undefinedBlocksSetIsEmpty()) {
               sem.wait(); // attend un message
            }
        if (!eventsMap.empty()) {
                mutex.lock();
                first=eventsMap.begin();
                pev = (*first).second;
                currentDate = pev->date;
                pev->consume();
                eventsMap.erase(first);
                eventsMapSize--;
                mutex.unlock();
            }
    }
}

void callback (message, socket) {
    mutex.lock();
    switch message( ) {
        ...
        ajout de l'évènement à la file d'évènements
        ...
    }
    mutex.unlock();
    async(socket, ..., callback, ...); // relance une écoute sur cette socket
    sem.post();
} 
*/
} // BlinkyBlocks namespace
