/*
 * message.cpp
 *
 *  Created on: 1 mars 2013
 *      Author: dom
 */

#include <iostream>
#include "message.h"

using namespace std;

unsigned int Message::nextId = 0;
unsigned int Message::nbMessages = 0;

Message::Message() {
	id = nextId;
	nextId++;
	nbMessages++;
	//cout << "Message constructor (" << id << ")" <<endl;
	MESSAGE_CONSTRUCTOR_INFO();
}

Message::~Message() {
	//cout << "Message destructor(" << id << ")" << endl;
	MESSAGE_DESTRUCTOR_INFO();
	nbMessages--;
}

unsigned int Message::getNbMessages() {
	return(nbMessages);
}

string Message::getMessageName() {
	return("generic message");
}
