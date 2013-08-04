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
			debuggerMessageHandler(debugger::pack(debugger::PRINTCONTENT, "node does not exist\n"));
			return 0;
		}
	} else if (id == -1) {
		// send to all vm
		int x = getWorld()->broadcastDebugMessage(size, message);
		// Bricolage en attendant un reel algorithme
		cur_timeout++;
		//boost::thread(boost::bind(&BlinkyBlocksDebugger::timeOut, this, cur_timeout));
		return x;
	} else {
		return 0;
	}
}

void BlinkyBlocksDebugger::pauseSim() {
	cout << "Simulator paused" << endl;
	getScheduler()->pause(BlinkyBlocks::getScheduler()->now());
}

void BlinkyBlocksDebugger::unPauseSim() {
	cout << "Simulator unpaused" << endl;
	getScheduler()->unPause();
}

void BlinkyBlocksDebugger::timeOut(int num) {
	//debuggerMessageHandler(debugger::pack(debugger::TIMEOUT, "System appears to be in equilibrium\n"));
	while(getScheduler()->getState() == Scheduler::NOTSTARTED) {
		usleep(5000);
	}	
	sleep(3);
	if(num == cur_timeout)
		debuggerMessageHandler(debugger::pack(debugger::TIMEOUT, "TimeOut to avoid deadlock when system is in equilibrium\n"));
}

void BlinkyBlocksDebugger::waitForDebuggerEnd() {
	debugger::joinThread();
}

void BlinkyBlocksDebugger::sendTerminateMsg(int id) {
	debugger::sendMsg(id,debugger::TERMINATE,"");
}

BlinkyBlocksDebugger::~BlinkyBlocksDebugger() {};

}
