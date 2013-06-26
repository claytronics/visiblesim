#include "blinkyBlocksDebugger.h"
#include "blinkyBlocksSimulator.h"
#include "blinkyBlocksScheduler.h"
#include "blinkyBlocksEvents.h"
#include "blinkyBlocksWorld.h"
//#include "world.h"
#include "blinkyBlocksBlock.h"

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
		debuggerMessageHandler = initDebugger(&sendMessage, &pauseSimulation, &unPauseSimulation);
	} else {
		ERRPUT << "\033[1;31m" << "Only one Scheduler instance can be created, aborting !" << "\033[0m" << endl;
		exit(EXIT_FAILURE);
	}
}

void BlinkyBlocksDebugger::sendMes(int id, int size, uint64_t *message) {
	
	/*BaseSimulator::getScheduler()->scheduleLock(new NetworkInterfaceEnqueueOutgoingEvent(BaseSimulator::getScheduler()->now(),
					new VMDataMessage(hostBlock->blockId, size, message), interface));*/
	if (id > 0) {
		BlinkyBlocksBlock *bb = (BlinkyBlocksBlock*) getWorld()->getBlockById(id);
		if (bb != NULL) {
			//getScheduler()->scheduleLock(new VMDebugMessageEvent(getScheduler()->now(), bb, new VMDebugMessage(size, message)));
			getScheduler()->schedule(new VMDebugMessageEvent(getScheduler()->now(), bb, new VMDebugMessage(size, message)));
		}
	} else if (id == -1) {
		// send to all vm
		
	} 
}

void BlinkyBlocksDebugger::pauseSim(int timestamp) {
	//sendMessage(-1, int size, void *message); // attention au timestamp
	//getScheduler()->scheduleLock(new VMDebugPauseSimEvent(timestamp));
	getScheduler()->schedule(new VMDebugPauseSimEvent(timestamp));
}

void BlinkyBlocksDebugger::unPauseSim() {
	//sendMessage(-1, int size, void *message);
	// pas de schedule lock ici, sinon inter-blocage
	getScheduler()->unPauseSimulation();
}

}
