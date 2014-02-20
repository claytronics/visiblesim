/*
 * blinkyBlocksClock.h
 *
 *  Created on: 20 fevrier 2014
 *      Author: Andre
 */

#ifndef BLINKYBLOCKSCLOCK_H_
#define BLINKYBLOCKSCLOCK_H_

#include "buildingBlock.h"
#include "blinkyBlocksVM.h"
#include "blinkyBlocksBlockCode.h"
#include "blinkyBlocksGlBlock.h"
#include <boost/asio.hpp> 
#include <stdexcept>

namespace BlinkyBlocks {

class BlinkyBlocksClock {

protected:
	BlinkyBlocksBlock *hostBlock;
	// Software clock and synchronization
	uint64_t startTime; // block's clock starts to count only when the 
	float clockDerivationFactor; // clock derivation
	uint64_t syncOffset; // computed by the time sync part
	
public:
	uint8_t lastWaveId;
		
	BlinkyBlocksClock(BlinkyBlocksBlock *host);
	~BlinkyBlocksClock() {};
	
	//inline uint64_t setStartTime(uint64_t t) { startTime = t; }
	
	uint64_t getClockUS(); // us
	uint64_t getClockMS(); // ms

	void handleSyncMsg(MessagePtr m);
	void adjustClock(uint64_t estimation);
	
	// WARNING CAN MOVE WITH A NEW SYNCHRONIZATION PHASE
	uint64_t getSchedulerTimeForLocalClockUS(uint64_t clock);
	uint64_t getSchedulerTimeForLocalClockMS(uint64_t clock);
};


}

#endif /* BLINKYBLOCKSCLOCK_H_ */
