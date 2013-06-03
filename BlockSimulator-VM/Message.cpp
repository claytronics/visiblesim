/*
 * Message.cpp
 *
 *  Created on: 1 févr. 2012
 *      Author: dom
 */

#include "Message.h"

unsigned int Message::nbMessages = 0;

Message::Message() {
	//cout << "création message" << endl;
	typeMessage = TYPE_MSG_GENERIQUE;
	blocSource = NULL;
	blocDestination = NULL;
	interfaceSource = NULL;
	interfaceDestination = NULL;
	nbMessages++;
	echec = false;
	nbRef = 0;
}

Message::~Message() {
	//cout << "destruction message" << endl;
	nbMessages--;
}
