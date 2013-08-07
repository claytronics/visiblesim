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
#include "blinkyBlocksSimulator.h"
#include "blinkyBlocksScheduler.h"

using namespace std;

namespace BlinkyBlocks {

class VMDebugMessage {
public:
	int size; // in bytes
	uint64_t *message;
	
	VMDebugMessage(int s, uint64_t *m);
	virtual ~VMDebugMessage();
};

typedef boost::shared_ptr<VMDebugMessage> VMDebugMessagePtr;

class BlinkyBlocksDebugger {

protected:
	void (*debuggerMessageHandler)(uint64_t*);	
	static BlinkyBlocksDebugger *debugger;

public:
	static bool threadHasFinished;
	
	BlinkyBlocksDebugger();
	~BlinkyBlocksDebugger();
	
	int sendMsg(int id, int size, uint64_t *message);
	void pauseSim();
	void unPauseSim();
	
	static BlinkyBlocksDebugger* getDebugger() {
		assert(debugger != NULL);
		return(debugger);
	}
	
	static void createDebugger() {
			debugger = new BlinkyBlocksDebugger();
	}
	
	static void deleteDebugger() {
		delete debugger;
		debugger = NULL;
	}
	
	void handleDebugMessage(uint64_t* m) {
		debuggerMessageHandler(m);
	}
	
	void timeOut(int num);
	
	void waitForDebuggerEnd();
	
	void sendTerminateMsg(int id);
	
	void handlePauseRequest();
	
};

inline void createDebugger() { BlinkyBlocksDebugger::createDebugger(); }

inline void deleteDebugger() { BlinkyBlocksDebugger::deleteDebugger(); }

inline BlinkyBlocksDebugger* getDebugger() { return(BlinkyBlocksDebugger::getDebugger()); }
	
inline int sendMessage(int id, uint64_t *message, int size) { return getDebugger()->sendMsg(id, size, message); }

inline void pauseSimulation(void) { getDebugger()->pauseSim(); }

inline void unPauseSimulation() { getDebugger()->unPauseSim(); }

inline void handleDebugMessage(uint64_t* m) { getDebugger()->handleDebugMessage(m); }

inline void quit() {
					BlinkyBlocksDebugger::threadHasFinished = true;
					if(getScheduler()->getState() != Scheduler::ENDED)
						glutLeaveMainLoop(); 
					}

}

#endif /* BLINKYBLOCKSDEBUGGER_H_ */
