/*
 * blinkyBlocksClock.cpp
 *
 *  Created on: 20 fevrier 2014
 *      Author: Andre
 */

#include <iostream>
#include "blinkyBlocksBlock.h"
#include "blinkyBlocksEvents.h"
#include "blinkyBlocksClock.h"
#include "blinkyBlocksNetwork.h"

#define MIN_CLOCK_DERIVATION_FACTOR 0.994
#define MAX_CLOCK_DERIVATION_FACTOR 1.006

using namespace std;

namespace BlinkyBlocks {

//===========================================================================================================
//
//          BlinkyBlocksClock  (class)
//
//===========================================================================================================

#define TRANSMISSION_DELAY 6

BlinkyBlocksClock::BlinkyBlocksClock(BlinkyBlocksBlock *host) {
	hostBlock = host;
	startTime = BaseSimulator::getScheduler()->now(); // we can add a random number here
	syncOffset = 0; // not computed yet
	clockDerivationFactor = (rand()/(double)RAND_MAX )* 
		(MAX_CLOCK_DERIVATION_FACTOR-MIN_CLOCK_DERIVATION_FACTOR) + MIN_CLOCK_DERIVATION_FACTOR;
	lastWaveId = 0;
}

uint64_t BlinkyBlocksClock::getClockUS() { // us
	return ((double)BaseSimulator::getScheduler()->now()*clockDerivationFactor) - syncOffset - startTime;
}

uint64_t BlinkyBlocksClock::getClockMS() { // ms
	return getClockUS()/1000;
}

void BlinkyBlocksClock::handleSyncMsg(MessagePtr m) {
	uint64_t processingTime;
	
	BlinkyBlocksClockSyncMsg_ptr message = boost::static_pointer_cast<BlinkyBlocksClockSyncMsg>(m);
	
	if (lastWaveId < message->waveId) {
		// Estimate the global time
		adjustClock(message->clock);
		lastWaveId = message->waveId;
		// schedule neighbors synchronization (random variable  between 0 - 2000 to simulate processing time)
		processingTime = (rand()/(double)RAND_MAX) * (2000-0) + 0;
		cout << hostBlock->blockId << " processing time: " << processingTime << endl;
		BaseSimulator::getScheduler()->schedule(new SynchronizeNeighborClocksEvent(BaseSimulator::getScheduler()->now() + processingTime, hostBlock, message->waveId));
	}
}

void BlinkyBlocksClock::adjustClock(uint64_t recv_clock) {
	syncOffset = 0;
	syncOffset = (getClockMS() - (recv_clock + TRANSMISSION_DELAY))*1000;
}

uint64_t BlinkyBlocksClock::getSchedulerTimeForLocalClockUS(uint64_t clock) {
	return (uint64_t (((double) (clock + syncOffset + startTime)) / clockDerivationFactor));
}

uint64_t BlinkyBlocksClock::getSchedulerTimeForLocalClockMS(uint64_t clock) {
	return getSchedulerTimeForLocalClockUS(clock * 1000);
}

}
