/*
 * blinky01BlockCode.h
 *
 *  Created on: 26 mars 2013
 *      Author: dom
 */

#ifndef BLINKY01BLOCKCODE_H_
#define BLINKY01BLOCKCODE_H_

#include "blinkyBlocksBlockCode.h"
#include "blinkyBlocksSimulator.h"
#include "blinkyBlocksVMCommands.h"
#include <boost/random.hpp>

class Blinky01BlockCode : public BlinkyBlocks::BlinkyBlocksBlockCode {
private:
	bool computing; // fastest mode 2
	uint64_t endComputingTime; // fastest mode 2
	bool willHaveWork; // fastest mode 2
	boost::rand48 generator; // fastest mode 2
	
public:

	Blinky01BlockCode(BlinkyBlocks::BlinkyBlocksBlock *host);
	~Blinky01BlockCode();

	void startup();
	void processLocalEvent(EventPtr pev);
	void handleCommand(BlinkyBlocks::VMCommand &command);
	void handleDeterministicMode(BlinkyBlocks::VMCommand &command);
	bool mustBeQueued(BlinkyBlocks::VMCommand &command);
	void handleDeterministicMode();
	int getRandom();
	static BlinkyBlocks::BlinkyBlocksBlockCode *buildNewBlockCode(BlinkyBlocks::BlinkyBlocksBlock *host);
};

#endif /* BLINKY01BLOCKCODE_H_ */
