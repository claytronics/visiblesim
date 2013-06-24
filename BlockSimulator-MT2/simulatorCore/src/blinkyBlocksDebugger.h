/*
 * blinkyBlocksDebugger.h
 *
 *  Created on: 21 juin 2013
 *      Author: Andre
 */

#ifndef BLINKYBLOCKSDEBUGGER_H_
#define BLINKYBLOCKSDEBUGGER_H_

#include <iostream>
#include <boost/shared_ptr.hpp>
 #include <inttypes.h>

using namespace std;

namespace BlinkyBlocks {

extern void (*debuggerMessageHandler)(uint64_t*);

/* -------------------------------------------------------------------*/
/* To be removed later, just for testing purpose */
inline void handleMessage (uint64_t *message) {
		cout << "message handler debugger" << endl;
};

inline void (*initDebugger(void (*send)(int, int, uint64_t*), 
					void (*pause)(int, int, uint64_t* ), 
					void (*unPause)(int, uint64_t*) ))(uint64_t*) {
	cout << "init debugger" << endl;
	return handleMessage;
	
};

/* -------------------------------------------------------------------*/

class VMDebugMessage {
public:
	int size; // in bytes
	uint64_t *message;
	
	VMDebugMessage(int s, uint64_t *m);
	virtual ~VMDebugMessage();
};

typedef boost::shared_ptr<VMDebugMessage> VMDebugMessagePtr;

void init();

void sendMessage(int id, int size, uint64_t *message);

void pauseSimulation(int timestamp, int size, uint64_t *message);

void unPauseSimulation(int size, uint64_t *message);

/*
class BlinkyBlocksDebugger {
	
public:
	
	BlinkyBlocksDebugger();
	~BlinkyBlocksDebugger();
	
protected:

}; */

}

#endif /* BLINKYBLOCKSDEBUGGER_H_ */
