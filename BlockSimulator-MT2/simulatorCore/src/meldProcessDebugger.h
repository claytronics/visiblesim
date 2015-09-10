/*
 * meldProcessDebugger.h
 *
 *  Created on: 21 juin 2013
 *      Author: Andre
 */

#ifndef MELDPROCESSDEBUGGER_H_
#define MELDPROCESSDEBUGGER_H_

#include <iostream>
#include <boost/shared_ptr.hpp>
#include <inttypes.h>
#include "simulator.h"
#include "meldProcessScheduler.h"
#include "meldProcessVMCommands.h"

using namespace std;

namespace MeldProcess {

class MeldProcessDebugger {

protected:
	void (*debuggerCommandHandler)(uint64_t*);	
	static MeldProcessDebugger *debugger;

public:
	
	MeldProcessDebugger();
	~MeldProcessDebugger();
	
	int sendCmd(int id, DebbuggerVMCommand &c);
	void pauseSim(int t);
	void unPauseSim();
	
	static MeldProcessDebugger* getDebugger() {
		assert(debugger != NULL);
		return(debugger);
	}
	
	static void createDebugger() {
			debugger = new MeldProcessDebugger();
	}
	
	static void deleteDebugger() {
		delete debugger;
		debugger = NULL;
	}
	
	void handleDebugCommand(DebbuggerVMCommand *c);
	
	void timeOut(int num);
	
	void waitForDebuggerEnd();
	
	void detachDebuggerThread();
	
	void sendTerminateCmd(int id);
	
	void handlePauseRequest();
	
	void handleBreakAtTimeReached(uint64_t t);
   
   static void print(string s, bool arrow = true);
	
};

inline void createDebugger() { MeldProcessDebugger::createDebugger(); }

inline void deleteDebugger() { MeldProcessDebugger::deleteDebugger(); }

inline MeldProcessDebugger* getDebugger() { return(MeldProcessDebugger::getDebugger()); }
	
inline int sendCommand(int id, uint64_t *data, int size) { DebbuggerVMCommand c(data); return getDebugger()->sendCmd(id, c); }

inline void pauseSimulation(int t) { getDebugger()->pauseSim(t); }

inline void unPauseSimulation() { getDebugger()->unPauseSim(); }

inline void handleDebugCommand(DebbuggerVMCommand *c) { getDebugger()->handleDebugCommand(c); }

inline void quit() {
	// DOES NOT WORK WITH NVIDIA OPTIMUS CARD...
	if(getScheduler()->getState() != Scheduler::ENDED) {
		glutLeaveMainLoop();
	}
}
	
}

#endif /* MELDPROCESSDEBUGGER_H_ */
