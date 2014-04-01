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
	float clockDriftFactor; // clock derivation
	int64_t syncOffset; // computed by the time sync part
	
	uint64_t lastSendSync;
	uint64_t lastRecSync;
	double speedAvg;
	
	uint64_t clockMaxReach;
	
public:
	//uint8_t lastWaveId;
	uint64_t nbSync;
	
	BlinkyBlocksClock(BlinkyBlocksBlock *host);
	~BlinkyBlocksClock() {};
	
	//inline uint64_t setStartTime(uint64_t t) { startTime = t; }
	
	uint64_t getTimeUS();
	uint64_t getTimeMS();
	
	uint64_t getLocalClockUS(); // us
	uint64_t getLocalClockMS(); // ms
	
	uint64_t getEstimatedGlobalClockUS(); // us
	uint64_t getEstimatedGlobalClockMS(); // ms
	

	bool handleSyncMsg(MessagePtr m);
	void adjustClock(uint64_t estimation, uint8_t nbhops);
	
	// WARNING CAN MOVE WITH A NEW SYNCHRONIZATION PHASE
	uint64_t getSchedulerTimeForLocalClockUS(uint64_t clock);
	uint64_t getSchedulerTimeForLocalClockMS(uint64_t clock);
};


}

#endif /* BLINKYBLOCKSCLOCK_H_ */
