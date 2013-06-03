/*
 * CodeBlocFlavio01.h
 *
 *  Created on: 19 July 2012
 *      Author: dom
 */

#ifndef CODEBLOCFLAVIO01_H_
#define CODEBLOCFLAVIO01_H_

#ifdef WIN32
#include <WinSock2.h>
typedef size_t	socklen_t;
#else
#include <sys/socket.h>
#include <netinet/in.h>
#endif
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <pthread.h>

#include "CodeBloc.h"
#include "Blocs.h"
#include "Scheduler.h"


#include <semaphore.h>

class CodeBlocFlavio01 : public CodeBloc {
private:
	static int socketID;
	static sockaddr_in serverAddress, clientAddress;
	static sockaddr &serverAddressCast;
	static sockaddr &clientAddressCast;
	static sem_t *sem_VMResponse;
	static pthread_mutex_t mutexVMResponse;
public:
	CodeBlocFlavio01();
	void demarre();
	void traiteEvenementLocal(Evenement *ev);

	static void *waitForMessageFromVM(void *param);

	//add by benoit
	static bool VMThreadCreated;
	static pthread_t VMInterfaceThread;
};


#endif /* CODEBLOCFLAVIO01_H_ */
