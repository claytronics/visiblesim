#include "blinkyBlocksDebugger.h"
#include "blinkyBlocksSimulator.h"
#include "blinkyBlocksScheduler.h"
#include "blinkyBlocksEvents.h"
#include "blinkyBlocksWorld.h"
#include "blinkyBlocksBlock.h"
#include "blinkyBlocksVM.h"
#include "Debugger/debug_Simprompt.hpp"
#include <stdio.h>
#include <boost/thread/thread.hpp>
#include <boost/bind.hpp>


namespace BlinkyBlocks {

BlinkyBlocksDebugger *BlinkyBlocksDebugger::debugger=NULL;
bool BlinkyBlocksDebugger::threadHasFinished = false;

BlinkyBlocksDebugger::BlinkyBlocksDebugger() {
	if (debugger == NULL) {
		debugger = this;
		debuggerCommandHandler = debugger::initDebugger(&sendCommand, &pauseSimulation, &unPauseSimulation, &quit);
	} else {
		ERRPUT << "\033[1;31m" << "Only one Debugger instance can be created, aborting !" << "\033[0m" << endl;
		exit(EXIT_FAILURE);
	}
}


int BlinkyBlocksDebugger::sendCmd(int id, DebbuggerVMCommand &c) {
	if (id > 0) {
		BlinkyBlocksBlock *bb = (BlinkyBlocksBlock*) getWorld()->getBlockById(id);
		if (bb != NULL && bb->state >= BlinkyBlocksBlock::ALIVE && bb->vm != NULL) {
			bb->vm->sendCommand(c);
			return 1;
		} else {
			return -1;
		}
	} else if (id == -1) {
		// send to all vm
		return getWorld()->broadcastDebugCommand(c);
	} else {
		return -1;
	}
}

 void BlinkyBlocksDebugger::handleDebugCommand(DebbuggerVMCommand *c) {
		debuggerCommandHandler(c->getData());
		delete c; // delete command object, not the data. The debugger will do it after having processed the command.
 }

void BlinkyBlocksDebugger::pauseSim(int t) {
	if (t == -1) {
		if (getScheduler()->getMode() == SCHEDULER_MODE_REALTIME) {
			getScheduler()->pause(BlinkyBlocks::getScheduler()->now());
		}
	} else {
		getScheduler()->pause(t);
	}
}

void BlinkyBlocksDebugger::unPauseSim() {
	getScheduler()->unPause();
}

void BlinkyBlocksDebugger::waitForDebuggerEnd() {
	debugger::joinThread();
}

void BlinkyBlocksDebugger::sendTerminateCmd(int id) {
	//debugger::sendCmd(id,debugger::TERMINATE,"");
}

void BlinkyBlocksDebugger::handlePauseRequest() {
	debugger::handlePauseCommand();
}

BlinkyBlocksDebugger::~BlinkyBlocksDebugger() {};

}
