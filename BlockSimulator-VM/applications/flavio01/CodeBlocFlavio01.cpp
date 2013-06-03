/*
 * CodeBlocFlavio01.cpp
 *
 *  Created on: 10 July 2012
 *      Author: dom
 */

#include <iostream>
#include <sstream>
#include <set>

#include "CodeBlocFlavio01.h"

#define VM_MESSAGE_TYPE_START_SIMULATION	1
#define VM_MESSAGE_TYPE_END_SIMULATION		2
#define VM_MESSAGE_TYPE_CREATE_LINK			3
#define VM_MESSAGE_TYPE_COMPUTATION_LOCK	4
#define VM_MESSAGE_TYPE_COMPUTATION_UNLOCK	5
#define VM_MESSAGE_TYPE_SEND_MESSAGE		6
#define VM_MESSAGE_TYPE_RECEIVE_MESSAGE		7

#define ID_COMPUTATION_LOCK 2000

// not portable under windows, use stat variable threadCreated
//pthread_t CodeBlocFlavio01::VMInterfaceThread = NULL;
pthread_t CodeBlocFlavio01::VMInterfaceThread;
bool CodeBlocFlavio01::VMThreadCreated=false;
int CodeBlocFlavio01::socketID;
sockaddr_in CodeBlocFlavio01::serverAddress, CodeBlocFlavio01::clientAddress;
sockaddr &CodeBlocFlavio01::serverAddressCast = (sockaddr &)serverAddress;
sockaddr &CodeBlocFlavio01::clientAddressCast = (sockaddr &)clientAddress;
sem_t *CodeBlocFlavio01::sem_VMResponse;

typedef struct VMMessage_tt {
	uint64_t messageType;
	uint64_t param1;
	uint64_t param2;
	uint64_t param3;
} VMMessage_t;

using namespace std;

/*****************************************************
 *  Constructor for this Application
 *
 *  A thread dedicated to communications with the VM is initialized
 */
CodeBlocFlavio01::CodeBlocFlavio01() : CodeBloc() {
#if DEBUG_LEVEL > 3
	cout << "- Constructeur CodeBlocFlavio01\n";
#endif

	int ret;
	// Benoit : not portable under windows
	//if (VMInterfaceThread == NULL) {
	if (!VMThreadCreated) {
		ret = pthread_create( &VMInterfaceThread, NULL, waitForMessageFromVM, NULL);
		VMThreadCreated=(ret==0);
	}
}


void *CodeBlocFlavio01::waitForMessageFromVM(void *param) {
	stringstream info;
	Bloc *blc=NULL;

	socketID = socket(AF_INET, SOCK_DGRAM, 0);

	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(5100);
	serverAddress.sin_addr.s_addr = INADDR_ANY;

	VMMessage_t vmmessage;
	int bufferSize = sizeof(vmmessage);

	bind(socketID, &serverAddressCast, sizeof(serverAddressCast));

	socklen_t size=sizeof(clientAddress);

	Bloc *b1, *b2;
	uint64_t duration;
	uint64_t messageSize;

	static int nbMessagesReceived=0;

	InterfaceReseau *outgoingInterface;

	bool simulatorIsRunning = false;
	set<uint64_t> notYetInitializedThreads;
	uint64_t threadID;
	int value=0;

	sem_VMResponse = sem_open("/bensemaphore", O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO , 0);
	if (sem_VMResponse==SEM_FAILED) {
		cerr << "erreur semaphore" <<endl;
	}
	sem_close(sem_VMResponse);
	sem_unlink("/bensemaphore");

	bool endOfSimulation = false;
	info << "- Waiting for message from VM";
	Scheduler::trace(info.str());
	info.str("");
	ssize_t sz;

	while (!endOfSimulation) {
		nbMessagesReceived++;
		sz = recvfrom(socketID,(void*)&vmmessage, bufferSize, 0, &clientAddressCast, &size);
		info << "- Message received from VM (" << nbMessagesReceived << ") size=" << sz << endl;
		Scheduler::trace(info.str());
		info.str("");

		if (sz>0)  {
		switch (vmmessage.messageType) {
		case VM_MESSAGE_TYPE_START_SIMULATION:
			info << "- VM told us there should be " << vmmessage.param1 << " blocks";
			Scheduler::trace(info.str());
			info.str("");
			for (threadID = 0; threadID < vmmessage.param1; threadID++) {
				notYetInitializedThreads.insert(threadID);
				cout << "- activer block #" << threadID << endl;

				blc = GrilleSupport::getBlockFromId(threadID);
				if (blc) {
					blc->setColor(255,0,0);
				}
 			}
			break;
		case VM_MESSAGE_TYPE_CREATE_LINK:
			info << "- VM ordered us to add a link between blocks " << vmmessage.param1 << " and " << vmmessage.param2 << endl;
			Scheduler::trace(info.str());
			info.str("");
			b1 = Bloc::getBlocByID(vmmessage.param1);
			b2 = Bloc::getBlocByID(vmmessage.param2);
			cout << "- b1 ID : " << b1->blocID << endl;
			cout << "- b2 ID : " << b2->blocID << endl;
			b1->addP2PNetworkInterface(b2);
			break;
		case VM_MESSAGE_TYPE_COMPUTATION_LOCK:
			b1 = Bloc::getBlocByID(vmmessage.param1);
			if (b1 == NULL) { cout << "- ERROR, NULL block !" << endl; }
			duration = vmmessage.param2;
			info << "- VM asked me to lock block " << vmmessage.param1 << " for " << duration << endl;
			Scheduler::trace(info.str());
			info.str("");

			// color and highlight concerned block
			blc = GrilleSupport::getBlockFromId(vmmessage.param1);
			if (blc) {
				blc->toggleHighlight();
				blc->setColor(0,255,0);
			}

			//cout << "notYetInitializedThreads set size : " << notYetInitializedThreads.size() << endl;
			Scheduler::schedule(new EvenementBlocTimer(Scheduler::getHeureActuelle()+duration,b1,ID_COMPUTATION_LOCK));

//			int value;
			if (simulatorIsRunning) {
//				sem_getvalue(sem_VMResponse,&value);
//				cout << "getting another message from VM (sem value : " << value << ")" <<endl;
cout << "- is running !!!" << endl;
				sem_post(sem_VMResponse);

//				sem_getvalue(sem_VMResponse,&value);
//				cout << "getting another message from VM (new sem value : " << value << ")" <<endl;
			}
			if (!simulatorIsRunning) {
				cout << "- erase (" << vmmessage.param1 << ")"<< endl;
				cout << "- res=" << notYetInitializedThreads.erase(vmmessage.param1) << "," << notYetInitializedThreads.size() << endl;
				if (notYetInitializedThreads.empty()) {
cout << "- empty !" << endl;
					sem_VMResponse = sem_open("/bensemaphore", O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO , 0);
					if (sem_VMResponse == SEM_FAILED) {
						cout << "- ERREUR SEMAPHORE" << endl;
					}
//					sem_init(&sem_VMResponse, 0, 0);
//					sem_getvalue(sem_VMResponse,&value);
//					cout << " sem value just after init: " << value  << endl;
//					simulatorIsRunning = true;
					Scheduler::run(MODE_SCHEDULER_TEMPS_REEL);
//					Scheduler::waitingForStartOrder = false;
				}
			}

			break;
		case VM_MESSAGE_TYPE_SEND_MESSAGE:
			info << "- VM asked to send a message from " << vmmessage.param1 << " to " << vmmessage.param2 << " (size " << vmmessage.param3 << ")" << endl;
			Scheduler::trace(info.str());
			info.str("");

			b1 = Bloc::getBlocByID(vmmessage.param1);
			b2 = Bloc::getBlocByID(vmmessage.param2);
			messageSize = vmmessage.param3;
			outgoingInterface = b1->getP2PNetworkInterface(b2);
			if (outgoingInterface == NULL) {
				cout << "- ALERT !! Interface is NULL !!" << endl;
			}
			Scheduler::schedule(new EvenementBlocEnvoieMessage(Scheduler::getHeureActuelle(),new Message(),outgoingInterface));
			break;
		default:
			info << "- ERROR : Unknow or unsupported message received from Flavio's VM : " << vmmessage.messageType << endl;
			Scheduler::trace(info.str());
		}
		} else {
			switch (sz) {
			case MSG_EOR : cerr << "ERROR : end of record" << endl; break;
			case MSG_TRUNC : cerr << "ERROR : the trailing portion of a datagram was discarded because the datagram was larger than the buffer supplied." << endl; break;
			case MSG_CTRUNC : cerr << "ERROR : some control data were discarded due to lack of space in the buffer for ancillary data" << endl; break;
			case MSG_ERRQUEUE : cerr << "ERROR : no data was received but an extended error from the socket error queue." << endl; break;
			}

		}
	}

	//sendto(socketID, (void*)&vmmessage, bufferSize, 0, &clientAddressCast, size);

	return(NULL);
}

void CodeBlocFlavio01::demarre() {


	stringstream info;
	info << "- Demarrage du CodeBlocFlavio01 dans le bloc " << bloc->blocID;
	Scheduler::trace(info.str());

}

void CodeBlocFlavio01::traiteEvenementLocal(Evenement *ev) {
//	static int etat = 0;

	stringstream info;

	if (ev->typeEvenement == TYPE_EV_BLOC_TIMER) {
		info << "- block " << bloc->blocID << " just finished some computation, I have to tell the VM";
		Scheduler::trace(info.str());
		info.str("");

		VMMessage_t vmmessage;
		vmmessage.messageType = VM_MESSAGE_TYPE_COMPUTATION_UNLOCK;
		vmmessage.param1 = bloc->blocID;
		int bufferSize = sizeof(vmmessage);
		socklen_t size=sizeof(clientAddress);

		sendto(socketID, (void*)&vmmessage, bufferSize, 0, &clientAddressCast, size);

		int value;
		sem_getvalue(sem_VMResponse,&value);
		info << "- waiting for another message from VM (sem value : " << value << ")";
		Scheduler::trace(info.str());
		info.str("");
		sem_wait(sem_VMResponse);

	}

	if (ev->typeEvenement == TYPE_EV_BLOC_RECOIT_MESSAGE) {
		info << "- I got a message, I have to tell the VM";
		Scheduler::trace(info.str());
		info.str("");
		VMMessage_t vmmessage;
		vmmessage.messageType = VM_MESSAGE_TYPE_RECEIVE_MESSAGE;
		vmmessage.param1 = bloc->blocID;
		vmmessage.param2 = Scheduler::getHeureActuelle();
		int bufferSize = sizeof(vmmessage);
		socklen_t size=sizeof(clientAddress);

		sendto(socketID, (void*)&vmmessage, bufferSize, 0, &clientAddressCast, size);
	}
}
