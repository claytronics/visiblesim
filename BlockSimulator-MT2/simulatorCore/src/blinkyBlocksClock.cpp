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

// theoretically +-1%
//#define MIN_CLOCK_DRIFT_FACTOR 0.99
//#define MAX_CLOCK_DRIFT_FACTOR 1.01

// in practice
#define MIN_CLOCK_DRIFT_FACTOR 0.994
#define MAX_CLOCK_DRIFT_FACTOR 1.006

#define LINEAR_CORRECTION

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
	clockDriftFactor = (rand()/(double)RAND_MAX )* 
		(MAX_CLOCK_DRIFT_FACTOR-MIN_CLOCK_DRIFT_FACTOR) + MIN_CLOCK_DRIFT_FACTOR;
	//lastWaveId = 0;
	
	lastSendSync = 0;
	lastRecSync = 0;
	speedAvg = 1;
	clockMaxReach = 0;
	nbSync = 0;
}

uint64_t BlinkyBlocksClock::getTimeUS() {
	return ((double)BaseSimulator::getScheduler()->now()*clockDriftFactor) - startTime;
}

uint64_t BlinkyBlocksClock::getTimeMS() {
	return getTimeUS()/1000;
}

uint64_t BlinkyBlocksClock::getEstimatedGlobalClockUS() { // us
	//cout << (double)getTimeUS() << "*" << speedAvg << " + " << syncOffset*1000 << endl;
	#ifdef LINEAR_CORRECTION
		return ((double)getTimeUS()*speedAvg) + syncOffset;
	#else
		return getTimeUS() + syncOffset;
	#endif
}

uint64_t BlinkyBlocksClock::getEstimatedGlobalClockMS() { // ms
	return getEstimatedGlobalClockUS()/1000;
}

uint64_t BlinkyBlocksClock::getLocalClockUS() { // us
	//return ((double)BaseSimulator::getScheduler()->now()*clockDriftFactor) - syncOffset - startTime;
	uint64_t estimation = getEstimatedGlobalClockUS();
	return max(clockMaxReach, estimation);
}

uint64_t BlinkyBlocksClock::getLocalClockMS() { // ms
	return getLocalClockUS()/1000;
}

/*
bool BlinkyBlocksClock::handleSyncMsg(MessagePtr m) {
	uint64_t processingTime;
	
	BlinkyBlocksClockSyncInfoMsg_ptr message = boost::static_pointer_cast<BlinkyBlocksClockSyncInfoMsg>(m);
	
	if (lastWaveId < message->waveId) {
		// Estimate the global time
		adjustClock(message->clock, message->nbhops);
		lastWaveId = message->waveId;
		// schedule neighbors synchronization (random variable  between 0 - 2000 to simulate processing time)
		processingTime = (rand()/(double)RAND_MAX) * (2000-0) + 0;
		//cout << hostBlock->blockId << " processing time: " << processingTime << endl;
		BaseSimulator::getScheduler()->schedule(new SynchronizeNeighborClocksEvent(BaseSimulator::getScheduler()->now() + processingTime, hostBlock, message->waveId, message->nbhops+1));
		return true;
	}
	return false;
}*/


bool BlinkyBlocksClock::handleSyncMsg(MessagePtr m) {
	uint64_t processingTime;
	
	BlinkyBlocksClockSyncInfoMsg_ptr message = boost::static_pointer_cast<BlinkyBlocksClockSyncInfoMsg>(m);
	
	// Estimate the global time
	cout << "@" <<  hostBlock->blockId <<  " sent time: " << message->clock << endl;
	adjustClock(message->clock, message->nbhops);
	// schedule neighbors synchronization (random variable  between 0 - 2000 to simulate processing time)
	processingTime = (rand()/(double)RAND_MAX) * (2000-0) + 0;
	//cout << hostBlock->blockId << " processing time: " << processingTime << endl;
	getScheduler()->schedule(new SynchronizeNeighborClocksEvent(getScheduler()->now() + processingTime, hostBlock, message->nbhops+1));

	return true;
}


void BlinkyBlocksClock::adjustClock(uint64_t recv_clock, uint8_t nbhops) {
	uint64_t estimatedGlobalTime = recv_clock + TRANSMISSION_DELAY;
	uint64_t now = getTimeMS();
	nbSync++;
#ifdef LINEAR_CORRECTION	
	double observedSpeed = ((double) (estimatedGlobalTime - lastSendSync))/ ((double) (now - lastRecSync));
	double n = (double) nbSync;
	lastSendSync = estimatedGlobalTime;
	lastRecSync = now;
#endif

	clockMaxReach = max(getLocalClockUS(), clockMaxReach);
	syncOffset = 0;
#ifdef LINEAR_CORRECTION
	speedAvg = (speedAvg*(n-1) + observedSpeed) / n;
	syncOffset = round(estimatedGlobalTime - (speedAvg*((double)getTimeMS())))*1000;
	cout << "@" <<  hostBlock->blockId << " estimated global time: " << estimatedGlobalTime << ", now: " << now << endl;
	cout << "@" <<  hostBlock->blockId << " speed: " << observedSpeed << ", speedAvg: " << speedAvg << ", offset: " << syncOffset << ", time: " << getTimeMS() << ", estimated global clock: " << getEstimatedGlobalClockMS() << ", local clock: " << getLocalClockMS()  << endl;
#else
	syncOffset = ((estimatedGlobalTime + 0*(float)(nbhops+1)) - now)*1000;
	cout << "@" <<  hostBlock->blockId << " adjusted clock: " << getLocalClockUS() << " at " << getScheduler()->now() << "(diff: " << (int64_t) getLocalClockUS() - (int64_t)getScheduler()->now()  << ")" << endl;
#endif
}

uint64_t BlinkyBlocksClock::getSchedulerTimeForLocalClockUS(uint64_t clock) {
	//return (uint64_t (((double) (clock + syncOffset + startTime)) / clockDriftFactor));
	if (clock < getLocalClockUS()) {
		cerr << "@" <<  hostBlock->blockId << ": scheduler time already reach for local clock " << clock << endl; 
		return getScheduler()->now();
	}
	return (uint64_t(((double) ( ((double)(clock - syncOffset)/speedAvg) + startTime)) / clockDriftFactor));
}

uint64_t BlinkyBlocksClock::getSchedulerTimeForLocalClockMS(uint64_t clock) {
	return getSchedulerTimeForLocalClockUS(clock * 1000);
}

}
