/*
 * blinkyBlocksDebugger.h
 *
 *  Created on: 21 juin 2013
 *      Author: Andre
 */

#ifndef BLINKYBLOCKSDEBUGGER_H_
#define BLINKYBLOCKSDEBUGGER_H_

#include <iostream>

using namespace std;

namespace BlinkyBlocks {

extern void (*debuggerMessageHandler)(void*);

/* -------------------------------------------------------------------*/
/* To be removed later, just for testing purpose */
inline void handleMessage (void *message) {
		cout << "message handler debugger" << endl;
};

inline void (*initDebugger(void (*send)(int, void*), 
					void (*pause)(int), 
					void (*unPause)(void) ))(void*) {
	cout << "init debugger" << endl;
	return handleMessage;
	
};

/* -------------------------------------------------------------------*/

void init();

void sendMessage(int id, void *message);

void pauseSimulation(int timestamp);

void unPauseSimulation();

/*
class BlinkyBlocksDebugger {
	
public:
	
	BlinkyBlocksDebugger();
	~BlinkyBlocksDebugger();
	
protected:

}; */

}

#endif /* BLINKYBLOCKSDEBUGGER_H_ */
