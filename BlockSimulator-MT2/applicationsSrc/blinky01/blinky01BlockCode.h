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

class VMDataMessage : public Message {	
public:
	uint64_t *message;
	
	VMDataMessage(uint64_t src, uint64_t* m);
	VMDataMessage(VMDataMessage *m);
	~VMDataMessage();	
	virtual unsigned int size();
};

class Blinky01BlockCode : public BlinkyBlocks::BlinkyBlocksBlockCode {
private:
	bool computing; // deterministic mode 2
	uint64_t endComputingTime; // deterministic mode 2
	
public:

	Blinky01BlockCode(BlinkyBlocks::BlinkyBlocksBlock *host);
	~Blinky01BlockCode();

	void startup();
	void processLocalEvent(EventPtr pev);
	void handleNewMessage(uint64_t *message);
	bool mustBeQueued();
	void handleDeterministicMode();
	static BlinkyBlocks::BlinkyBlocksBlockCode *buildNewBlockCode(BlinkyBlocks::BlinkyBlocksBlock *host);
};

#endif /* BLINKY01BLOCKCODE_H_ */
