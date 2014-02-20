/*
 * network.cpp
 *
 *  Created on: 24 mars 2013
 *      Author: dom
 */

#include <iostream>
#include <sstream>
#include "scheduler.h"
#include "blinkyBlocksNetwork.h"
#include "trace.h"

using namespace std;

namespace BlinkyBlocks {

//===========================================================================================================
//
//          BlinkyBlocksMessage  (class)
//
//===========================================================================================================

BlinkyBlocksMessage::BlinkyBlocksMessage(unsigned int s) : Message() {
	msize = s;	
}

unsigned int BlinkyBlocksMessage::size() {
	return msize;
}

unsigned int BlinkyBlocksMessage::nbChunks(unsigned int s){
	// Chunk on real Blinky Blocks:
	// data : 17 bytes
	// Frame delimiter (FD) : 1 byte
	// Pointer message handler: 2 bytes
	// Checkum : 1 byte
	unsigned int n = s/17;
	if ( (s % 17) != 0) {
		n++;
	}
	return n;
}

//===========================================================================================================
//
//          BlinkyBlocksClockSyncMsg  (class)
//
//===========================================================================================================

uint8_t BlinkyBlocksClockSyncMsg::waveIdCnt = 1;

BlinkyBlocksClockSyncMsg::BlinkyBlocksClockSyncMsg(uint32_t c) {
	clock = c;
	waveId = waveIdCnt;
	waveIdCnt++;
	msize = 1 + 1 + 4; // type (CLOCK_SYNC) + Wave Id + clock value
	type = BB_CLOCK_SYNC_MESSAGE;
}

BlinkyBlocksClockSyncMsg::BlinkyBlocksClockSyncMsg(uint32_t c, uint8_t wId) {
	clock = c;
	waveId = wId;
	msize = 1 + 1 + 4;
	type = BB_CLOCK_SYNC_MESSAGE;
}

//===========================================================================================================
//
//          P2PNetworkInterface  (class)
//
//===========================================================================================================

// 1 start + 8 data + 1 stop
#define BITS_PER_SYMBOL 10 
// ms
#define MIN_TIME 6.08
#define MAX_TIME 6.11
SerialNetworkInterface::SerialNetworkInterface(BaseSimulator::BuildingBlock *b) : P2PNetworkInterface(b) {
	dataRate = 38400;
}

SerialNetworkInterface::~SerialNetworkInterface() {}

unsigned int SerialNetworkInterface::computeTransmissionDuration(unsigned int size) {
	float t = 0.0;
	unsigned int nbChunks = BlinkyBlocksMessage::nbChunks(size);
	for (unsigned int i = 0; i < nbChunks; i++) {
		t += (rand()/(double)RAND_MAX ) * (MAX_TIME-MIN_TIME) + MIN_TIME;
	}
	//cout << "size: " << size << endl;
	//cout << "nbChunks: " << nbChunks << ", time: " << t << " ms" << endl;
	cout << hostBlock->blockId << " network trans. time: " << t * 1000 << endl;
	return (t * 1000); // ms to us (simulator unit)
}

}
