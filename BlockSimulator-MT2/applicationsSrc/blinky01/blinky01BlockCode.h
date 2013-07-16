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
#include "blinkyBlocksEvents.h"

class VMDataMessage : public Message {	
public:
	uint64_t *message;
	
	VMDataMessage(uint64_t src, uint64_t* m);	
	~VMDataMessage();	
	virtual unsigned int size();
};

//class VMTimeOutWaitForCommandEvent;

class Blinky01BlockCode : public BlinkyBlocks::BlinkyBlocksBlockCode {
private:
	bool computing;
	BlinkyBlocks::VMTimeOutWaitForCommandEvent* timeOutEvent;
	
public:

	Blinky01BlockCode(BlinkyBlocks::BlinkyBlocksBlock *host);
	~Blinky01BlockCode();

	void startup();
	void processLocalEvent(EventPtr pev);
	void handleNewMessage();
	
	static BlinkyBlocks::BlinkyBlocksBlockCode *buildNewBlockCode(BlinkyBlocks::BlinkyBlocksBlock *host);
};

#endif /* BLINKY01BLOCKCODE_H_ */
