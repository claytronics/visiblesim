/*
 * VMDrivingTest.cpp
 *
 *  Created on: 19 juil. 2012
 *      Author: dom
 */

#include <iostream>
#include <sstream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <netdb.h>

using namespace std;

#define VM_MESSAGE_TYPE_START_SIMULATION	1
#define VM_MESSAGE_TYPE_END_SIMULATION		2
#define VM_MESSAGE_TYPE_CREATE_LINK			3
#define VM_MESSAGE_TYPE_COMPUTATION_LOCK	4
#define VM_MESSAGE_TYPE_COMPUTATION_UNLOCK	5
#define VM_MESSAGE_TYPE_SEND_MESSAGE		6
#define VM_MESSAGE_TYPE_RECEIVE_MESSAGE		7

typedef struct VMMessage_tt {
	uint64_t messageType;
	uint64_t param1;
	uint64_t param2;
	uint64_t param3;
} VMMessage_t;

int socketID = socket(AF_INET, SOCK_DGRAM, 0);
sockaddr_in serverAddress, clientAddress;
sockaddr &serverAddressCast = (sockaddr &)serverAddress;
sockaddr &clientAddressCast = (sockaddr &)clientAddress;

VMMessage_t vmmessage;
int bufferSize;
socklen_t size;


int etape=0;

void reception() {
	cout << "attente du simulateur" << endl;
	recvfrom(socketID,(void*)&vmmessage, bufferSize, 0, &serverAddressCast, &size);
	cout << "received message from simulator : " <<endl;
	switch (vmmessage.messageType) {
	case VM_MESSAGE_TYPE_COMPUTATION_UNLOCK:
		cout << "Unlock thread " << vmmessage.param1 << endl;
		if (etape == 0) {
			vmmessage.messageType = VM_MESSAGE_TYPE_SEND_MESSAGE;
			vmmessage.param1 = 0;
			vmmessage.param2 = 1;
			vmmessage.param3 = 1000;
			sendto(socketID, (void*)&vmmessage, bufferSize, 0, &serverAddressCast, size);
			etape++;
		}

		break;
	case VM_MESSAGE_TYPE_RECEIVE_MESSAGE:
		cout << "Thread " << vmmessage.param1 << " received a message at time " << vmmessage.param2 << endl;
		break;
	default:
		cout << "Unknown message type" <<endl;
	}

}

int main(int argc, char **argv) {

	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(5100);
	struct hostent *hp = gethostbyname("localhost");
	memcpy((char*)&serverAddress.sin_addr, (char*)hp->h_addr, hp->h_length);

	clientAddress.sin_family = AF_INET;
	clientAddress.sin_port = htons(5101);
	clientAddress.sin_addr.s_addr = INADDR_ANY;

	bind(socketID, &clientAddressCast, sizeof(clientAddressCast));

	bufferSize = sizeof(vmmessage);
	size=sizeof(clientAddress);

	vmmessage.messageType = VM_MESSAGE_TYPE_START_SIMULATION;
	vmmessage.param1 = 3; // nombre de blocks #0 à #2
	sendto(socketID, (void*)&vmmessage, bufferSize, 0, &serverAddressCast, size);

	vmmessage.messageType = VM_MESSAGE_TYPE_CREATE_LINK;
	vmmessage.param1 = 0;
	vmmessage.param2 = 1;
	sendto(socketID, (void*)&vmmessage, bufferSize, 0, &serverAddressCast, size);


	vmmessage.messageType = VM_MESSAGE_TYPE_COMPUTATION_LOCK;
	vmmessage.param1 = 0;
	vmmessage.param2 = 2000000;
	sendto(socketID, (void*)&vmmessage, bufferSize, 0, &serverAddressCast, size);

	vmmessage.messageType = VM_MESSAGE_TYPE_COMPUTATION_LOCK;
	vmmessage.param1 = 1;
	vmmessage.param2 = 3000000;
	sendto(socketID, (void*)&vmmessage, bufferSize, 0, &serverAddressCast, size);

/*	vmmessage.messageType = VM_MESSAGE_TYPE_COMPUTATION_LOCK;
	vmmessage.param1 = 2;
	vmmessage.param2 = 3000000;
	sendto(socketID, (void*)&vmmessage, bufferSize, 0, &serverAddressCast, size);

*/
	reception();
	reception();


	/*
	vmmessage.messageType = VM_MESSAGE_TYPE_COMPUTATION_LOCK;
	vmmessage.param1 = 1;
	vmmessage.param2 = 1000000;
	sendto(socketID, (void*)&vmmessage, bufferSize, 0, &serverAddressCast, size);
*/

	/*
	vmmessage.messageType = VM_MESSAGE_TYPE_SEND_MESSAGE;
	vmmessage.param1 = 5;
	vmmessage.param2 = 6;
	vmmessage.param3 = 1000;
	sendto(socketID, (void*)&vmmessage, bufferSize, 0, &serverAddressCast, size);
*/

}


