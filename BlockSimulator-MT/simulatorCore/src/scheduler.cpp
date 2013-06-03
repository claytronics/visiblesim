/*
 * scheduler.cpp
 *
 *  Created on: 19 févr. 2013
 *      Author: dom
 */

#include <iostream>
#include <assert.h>
#include "scheduler.h"
#include <GL/freeglut.h>
#include "multiCoresWorld.h"
#include "multiCoresSimulator.h"

uint64_t Scheduler::currentDate = 0;
uint64_t Scheduler::maximumDate = 600000000;
int Scheduler::listSize = 0;
int Scheduler::largestListSize = 0;
multimap<uint64_t,EventPtr> Scheduler::eventsList;
boost::interprocess::interprocess_semaphore Scheduler::sem_schedulerStart(0);
thread *Scheduler::schedulerThread;
int Scheduler::schedulerMode;

//boost::mutex Scheduler::mutScheduler;
boost::interprocess::interprocess_mutex Scheduler::mutex_schedule;
set<unsigned int> Scheduler::undefinedBlocksSet;
boost::interprocess::interprocess_mutex Scheduler::mutex_undefinedBlocksSet;

boost::asio::io_service *Scheduler::ios=NULL;
udp::socket *Scheduler::socket=NULL;
udp::endpoint Scheduler::VMEndpoint;


using namespace std;

Scheduler::Scheduler() {
	cout << "Scheduler constructor" << endl;

	if (sizeof(uint64_t) != 8) {
		puts("ERROR : Scheduler requires 8bytes integer that are not available on this computer");
		exit(EXIT_FAILURE);
	}

	ios = new boost::asio::io_service();
	socket = new udp::socket(*ios,udp::endpoint(udp::v4(), 7800));
	schedulerThread = new thread(Scheduler::startPaused);
}

Scheduler::~Scheduler() {
	cout << "Scheduler destructor" << endl;
	//schedulerThread->join();
}

bool Scheduler::schedule(Event *ev) {
	assert(ev != NULL);
	stringstream info;

	EventPtr pev(ev);

//	info << "Schedule a " << pev->getEventName() << " (" << ev->id << ")";
//	Scheduler::trace(info.str());

	lock();
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
		cout << "sizeof(int) : " << sizeof(int) << endl;
		cout << "sizeof(uint64_t) : " << sizeof(uint64_t) << endl;
	    return(false);
	}

	eventsList.insert(pair<uint64_t, EventPtr>(pev->date,pev));

	listSize++;

	if (largestListSize < listSize) largestListSize = listSize;

	unlock();
	return(true);
}

uint64_t Scheduler::now() {
	return(currentDate);
}

void Scheduler::trace(string message) {
	cout.precision(6);
	cout << fixed << (double)(currentDate)/1000000 << " " << message << endl;
}

void *Scheduler::startPaused() {
	int systemStartTime, systemStopTime;
	multimap<uint64_t, EventPtr>::iterator first;
	EventPtr pev;

	cout << "\033[1;33m" << "Scheduler : created and waiting for start order : " << "\033[0m" << endl;

//	sem_schedulerStart.wait();
	waitForVMMessage();
	schedulerMode = SCHEDULER_MODE_FASTEST;

	systemStartTime = (glutGet(GLUT_ELAPSED_TIME))*1000;
	cout << "\033[1;33m" << "Scheduler : start order received " << systemStartTime << "\033[0m" << endl;

	switch (schedulerMode) {
	case SCHEDULER_MODE_FASTEST:
	    while ( (!eventsList.empty() || !undefinedBlocksSetIsEmpty()) && currentDate < maximumDate) {

	    	while (!undefinedBlocksSetIsEmpty()) {
	    		waitForVMMessage();
	    	}

	    	if (!eventsList.empty()) {
	    		first=eventsList.begin();
	    		pev = (*first).second;
	    		currentDate = pev->date;
// 		   		lock();
				pev->consume();
//    			unlock();
				eventsList.erase(first);
				listSize--;
	    	}
	    }
		break;
	case SCHEDULER_MODE_REALTIME:
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
	cout << "Maximum sized reached by the events list : " << Scheduler::largestListSize << endl;
	cout << "Size of the events list at the end : " << eventsList.size() << endl;
	cout << "Number of events processed : " << Event::getNextId() << endl;
	cout << "Events(s) left in memory before destroying Scheduler : " << Event::getNbEvents() << endl;
	cout << "Message(s) left in memory before destroying Scheduler : " << Message::getNbMessages() << endl;

	return(NULL);
}

void Scheduler::start(int mode) {
	schedulerMode = mode;
	sem_schedulerStart.post();
}

void Scheduler::waitForSchedulerEnd() {
	schedulerThread->join();
}

void Scheduler::lock() {
	mutex_schedule.lock();
}

void Scheduler::unlock() {
	mutex_schedule.unlock();
}

void Scheduler::waitForVMMessage() {
	MultiCoresWorld *world = (MultiCoresWorld*)Simulator::world;
	stringstream info;
	VMMessage mes;

	float blockX = 0.0;
	float blockY = 0.0;
	float blockZ = 0.0;
	float blockRed = 1.0;
	float blockGreen = 1.0;
	float blockBlue = 1.0;

	size_t length = socket->receive_from(boost::asio::buffer((void*)&mes, sizeof(VMMessage)), VMEndpoint);

	VM_TRACE_MESSAGE(mes);

	if (length == sizeof(VMMessage)) {
		switch (mes.messageType) {
		case VM_MESSAGE_TYPE_START_SIMULATION:
			info.str("");
			info << " The simulator needs " << mes.param1 << " blocks, creating them";
			Scheduler::trace(info.str());
			for (unsigned int i=0; i<mes.param1; i++) {
				Scheduler::addUndefinedBlock(i);
				((MultiCoresWorld*)world)->addBlock(i,
						MultiCoresSimulator::buildNewBlockCode,blockX, blockY, blockZ, blockRed, blockGreen, blockBlue);
			}
			break;
		case VM_MESSAGE_TYPE_CREATE_LINK:
			info.str("");
			info << " Linking block " << mes.param1 << " with " << mes.param2;
			Scheduler::trace(info.str());
			BuildingBlock *b0, *b1;
			b0 = BuildingBlock::getBlocByID(mes.param1);
			b1 = BuildingBlock::getBlocByID(mes.param2);
			b0->addP2PNetworkInterface(b1);
			break;
		case VM_MESSAGE_TYPE_COMPUTATION_LOCK:
			if (!Scheduler::isBlockUndefined(mes.param1)) {
				info.str("");
				info << "Block " << mes.param1 << " received a COMPUTATION_LOCK whereas it was not in undefined state !";
				Scheduler::trace(info.str());
			}
			MultiCoresBlock *mcb0;
			mcb0 = (MultiCoresBlock*)BuildingBlock::getBlocByID(mes.param1);
			Scheduler::schedule(new VMStartComputationEvent(Scheduler::now(), mcb0, mes.param2));

			mcb0->setUndefinedState(false);
			Scheduler::removeUndefinedBlock(mes.param1);
			break;
		case VM_MESSAGE_TYPE_END_SIMULATION:
			if (!Scheduler::undefinedBlocksSetIsEmpty()) {
				info.str("");
				info << "A SIMULATION_END was received whereas there is at least one more block in undefined state !";
				Scheduler::trace(info.str());
			}
//			endSimulation = true;
			break;
		case VM_MESSAGE_TYPE_SEND_MESSAGE:
			if (!Scheduler::isBlockUndefined(mes.param1)) {
				info.str("");
				info << "Block " << mes.param1 << " received a SEND_MESSAGE whereas it was not in undefined state !";
				Scheduler::trace(info.str());
			}
			{
				MultiCoresBlock *mcb0;
				mcb0 = (MultiCoresBlock*)BuildingBlock::getBlocByID(mes.param1);
				Scheduler::schedule(new VMStartTransmissionEvent(Scheduler::now(), mcb0, mes.param2, mes.param3));
			}

			break;
		case VM_MESSAGE_TYPE_WAIT_FOR_MESSAGE:
			if (!Scheduler::isBlockUndefined(mes.param1)) {
				info.str("");
				info << "Block " << mes.param1 << " received a WAIT_FOR_MESSAGE whereas it was not in undefined state !";
				Scheduler::trace(info.str());
			}
			Scheduler::removeUndefinedBlock(mes.param1);
			//Scheduler::printUndefinedBlocksSet();
			break;
		default:
			cout << "*** ERROR *** : unsupported message received from VM" << endl;
			break;
		}
	}
}

void Scheduler::sendMessageToVM(VMMessage message) {
	socket->send_to(boost::asio::buffer((void*)&message,sizeof(message)),VMEndpoint);
}

