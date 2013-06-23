#include "blinkyBlocksDebugger.h"
#include "blinkyBlocksSimulator.h"
#include "blinkyBlocksEvents.h"

namespace BlinkyBlocks {

void (*debuggerMessageHandler)(void*) = NULL;

void init() {
	debuggerMessageHandler = initDebugger(&sendMessage, &pauseSimulation, &unPauseSimulation);
}

void sendMessage(int id, void *message) {
	//getScheduler()->scheduleLock(new VMDebugMessageEvent(getScheduler()->now(), ));
}

void pauseSimulation(int timestamp) {
	
}

void unPauseSimulation() {

}
}
