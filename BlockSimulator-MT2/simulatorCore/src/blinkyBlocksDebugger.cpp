#include "blinkyBlocksDebugger.h"
#include "blinkyBlocksSimulator.h"
#include "blinkyBlocksScheduler.h"
#include "blinkyBlocksEvents.h"
#include "blinkyBlocksWorld.h"
#include "blinkyBlocksBlock.h"
#include "blinkyBlocksVM.h"
#include "Debugger/debug_Simprompt.hpp"
#include <stdio.h>


namespace BlinkyBlocks {

VMDebugMessage::VMDebugMessage(int s, uint64_t *m) {
	size = s;
	message = new uint64_t[s / sizeof(uint64_t)];
	memcpy(message, m, size);
}

VMDebugMessage::~VMDebugMessage() {
	delete[] message;
}

//void (*debuggerMessageHandler)(uint64_t*) = NULL; // extern

BlinkyBlocksDebugger *BlinkyBlocksDebugger::debugger=NULL;

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
	
	/*BaseSimulator::getScheduler()->scheduleLock(new NetworkInterfaceEnqueueOutgoingEvent(BaseSimulator::getScheduler()->now(),
					new VMDataMessage(hostBlock->blockId, size, message), interface));*/
	//cout << "ID: " << id << endl;
	//uint64_t m[VM_MESSAGE_MAXLENGHT];
	//m[0] = message[0] + 4*sizeof(uint64_t);
	//m[1] = 16; // DEBUG FLAG
	//m[2] = BaseSimulator::getScheduler()->now();
	//m[3] = id; // not relevant
	//memcpy(m+4, message, message[0]);
	//cout << m[0] << "," << m[1] << "," << m[2] << "," << m[3] << "," << m[4] << endl;
	if (id > 0) {
		BlinkyBlocksBlock *bb = (BlinkyBlocksBlock*) getWorld()->getBlockById(id);
		if (bb != NULL) {
			//getScheduler()->schedule(new VMDebugMessageEvent(getScheduler()->now(), bb, new VMDebugMessage(size, message)));
			bb->blockCode->processLocalEvent(EventPtr (new VMSetIdEvent(BaseSimulator::getScheduler()->now(), bb)));
			bb->vm->sendMessage(size, message);
			//cout << "debug sent: size" << size << "content size" << message[0] << endl;
			return 1;
		} else {
			uint64_t fakeMessage[7];
			fakeMessage[0] = 4*sizeof(uint64_t);
			fakeMessage[1] = 16; // VM_DEBUG_MESSAGE
			fakeMessage[2] = 5; // PRINTCONTENT
			sprintf((char*)&fakeMessage[3], "Node %d does not exist.\n", id);
			debuggerMessageHandler(fakeMessage);
			return 0;
		}
	} else if (id == -1) {
		// send to all vm
		return getWorld()->broadcastVMMessage(size, message);
	} else {
		return 0;
	}
}

void BlinkyBlocksDebugger::pauseSim() {
	cout << "Simulator paused" << endl;
	getScheduler()->schedule(new VMDebugPauseSimEvent(BlinkyBlocks::getScheduler()->now()));
}

void BlinkyBlocksDebugger::unPauseSim() {
	// pas de schedule lock ici, sinon inter-blocage
	cout << "Simulator unpaused" << endl;
	getScheduler()->unPause();
}

}
