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

VMDebugMessage::VMDebugMessage(int s, uint64_t *m) {
	size = s;
	message = new uint64_t[s / sizeof(uint64_t)];
	memcpy(message, m, size);
}

VMDebugMessage::~VMDebugMessage() {
	delete[] message;
}

BlinkyBlocksDebugger *BlinkyBlocksDebugger::debugger=NULL;
bool BlinkyBlocksDebugger::threadHasFinished = false;
int cur_timeout = 0;

BlinkyBlocksDebugger::BlinkyBlocksDebugger() {
	if (debugger == NULL) {
		debugger = this;
		debuggerMessageHandler = debugger::initDebugger(&sendMessage, &pauseSimulation, &unPauseSimulation, &quit);
	} else {
		ERRPUT << "\033[1;31m" << "Only one Debugger instance can be created, aborting !" << "\033[0m" << endl;
		exit(EXIT_FAILURE);
	}
}


int BlinkyBlocksDebugger::sendMsg(int id, int size, uint64_t *message) {

	if (id > 0) {
		BlinkyBlocksBlock *bb = (BlinkyBlocksBlock*) getWorld()->getBlockById(id);
		if (bb != NULL && bb->state >= BlinkyBlocksBlock::ALIVE && bb->vm != NULL) {
			bb->vm->sendMessage(size, message);
			return 1;
		} else {
			return -1;
		}
	} else if (id == -1) {
		// send to all vm
		return getWorld()->broadcastDebugMessage(size, message);
	} else {
		return -1;
	}
}

void BlinkyBlocksDebugger::pauseSim(int t) {
	if (t == -1) {
		getScheduler()->pause(BlinkyBlocks::getScheduler()->now());
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

void BlinkyBlocksDebugger::sendTerminateMsg(int id) {
	debugger::sendMsg(id,debugger::TERMINATE,"");
}

void BlinkyBlocksDebugger::handlePauseRequest() {
	debugger::handlePauseCommand();
}

BlinkyBlocksDebugger::~BlinkyBlocksDebugger() {};

}
