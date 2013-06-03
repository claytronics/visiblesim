#include <stdio.h>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

int main(int argv, char **argc) {

	int socketID;
	sockaddr_in serverAddress, clientAddress;
	sockaddr &serverAddressCast;
	sockaddr &clientAddressCast;


	socketID = socket(AF_INET, SOCK_DGRAM, 0);

	serverAddress.sin_family = AF_INET;
	serverAddress.sin_port = htons(5000);
	serverAddress.sin_addr.s_addr = INADDR_ANY;

	VMMessage_t vmmessage;
	int bufferSize = sizeof(vmmessage);

	bind(socketID, &serverAddressCast, sizeof(serverAddressCast));

	socklen_t size=sizeof(clientAddress);

}
