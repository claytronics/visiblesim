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
	// Deterministic mode (fastest mode):
	uint64_t currentLocalDate; 	// Time unit: us 
								//represents the elapsed CPU time 
								// (cycles/cpu_frequency)
	bool hasWork; // fastest mode
	bool polling; // fastest mode
		
	BlinkyBlocksBlockCode(BlinkyBlocksBlock *host);
	virtual ~BlinkyBlocksBlockCode();

	virtual void handleCommand(VMCommand &command) = 0;
	virtual void handleDeterministicMode(VMCommand &command) = 0;
	virtual void init() = 0;
	
	inline uint64_t getCurrentLocalDate() { return currentLocalDate; }

	//static BlinkyBlocksBlockCode* buildNewBlockCode(BlinkyBlocksBlock *host);
	virtual void processLocalEvent(EventPtr pev) = 0;
};

}

#endif /* BLINKYBLOCKSBLOCKCODE_H_ */
