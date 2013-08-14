/*
 * blinkyBlocksBlockCode.h
 *
 *  Created on: 23 mars 2013
 *      Author: dom
 */

#ifndef BLINKYBLOCKSBLOCKCODE_H_
#define BLINKYBLOCKSBLOCKCODE_H_

#include "blockCode.h"
#include "blinkyBlocksBlock.h"
#include "blinkyBlocksVMCommands.h"

namespace BlinkyBlocks {

class BlinkyBlocksBlock;

class BlinkyBlocksBlockCode : public BaseSimulator::BlockCode {
public:
	uint64_t currentLocalDate; // deterministic mode 1
	bool hasWork; // deterministic mode 1 & 2
	
	BlinkyBlocksBlockCode(BlinkyBlocksBlock *host);
	virtual ~BlinkyBlocksBlockCode();

	virtual void handleCommand(VMCommand &command) {}
	virtual void handleDeterministicMode(VMCommand &command) {}
	virtual bool mustBeQueued(VMCommand &command) { return false; }
	inline uint64_t getCurrentLocalDate() { return currentLocalDate; }

	//static BlinkyBlocksBlockCode* buildNewBlockCode(BlinkyBlocksBlock *host);
	virtual void processLocalEvent(EventPtr pev) = 0;
};

}

#endif /* BLINKYBLOCKSBLOCKCODE_H_ */
