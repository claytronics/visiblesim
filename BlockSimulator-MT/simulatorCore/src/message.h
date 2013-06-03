/*
 * message.h
 *
 *  Created on: 1 mars 2013
 *      Author: dom
 */

#ifndef MESSAGE_H_
#define MESSAGE_H_

#include <string>
#include <boost/shared_ptr.hpp>

using namespace std;

class Message;
class P2PNetworkInterface;

typedef boost::shared_ptr<Message> MessagePtr;

#ifdef DEBUG_MESSAGES
#define MESSAGE_CONSTRUCTOR_INFO()			(cout << getMessageName() << " constructor (" << id << ")" << endl)
#define MESSAGE_DESTRUCTOR_INFO()			(cout << getMessageName() << " destructor (" << id << ")" << endl)
#else
#define MESSAGE_CONSTRUCTOR_INFO()
#define MESSAGE_DESTRUCTOR_INFO()
#endif

class Message {
protected:
	static unsigned int nextId;
	static unsigned int nbMessages;
public:
	unsigned int id;
	P2PNetworkInterface *sourceInterface, *destinationInterface;

	Message();
	virtual ~Message();

	static unsigned int getNbMessages();
	virtual string getMessageName();

	virtual unsigned int size() { return(4); }
};


#endif /* MESSAGE_H_ */
