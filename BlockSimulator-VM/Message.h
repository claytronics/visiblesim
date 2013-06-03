/*
 * Message.h
 *
 *  Created on: 1 févr. 2012
 *      Author: dom
 */

#ifndef MESSAGE_H_
#define MESSAGE_H_

#include "Blocs.h"
#include "map"

#define TYPE_MSG_GENERIQUE			1


class Message {
public:

	static unsigned int nbMessages;

	int nbRef;
	Bloc *blocSource;
	Bloc *blocDestination;
	InterfaceReseau *interfaceSource;
	InterfaceReseau *interfaceDestination;
	bool echec;
	unsigned int typeMessage;

	Message();
	~Message();
};


#endif /* MESSAGE_H_ */
