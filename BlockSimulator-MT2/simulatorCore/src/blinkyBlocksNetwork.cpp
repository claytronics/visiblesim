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

BlinkyBlocksMessage::BlinkyBlocksMessage(BlinkyBlocksMessage *msg) : Message() {
	msize = msg->msize;
	type = msg->type;
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
//          BlinkyBlocksLeaderElectionMinIdElectionMsg  (class)
//
//===========================================================================================================

BlinkyBlocksLeaderElectionMinIdElectionMsg::BlinkyBlocksLeaderElectionMinIdElectionMsg(unsigned int i): BlinkyBlocksMessage() {
	type = BB_CS_LE_MIN_ID_ELECTION_MESSAGE;
	id = i;
	msize = 1 + 1 + 2; // type: CLOCK_SYNC, LEADER ELECTION, MIN ID
}

BlinkyBlocksLeaderElectionMinIdElectionMsg::BlinkyBlocksLeaderElectionMinIdElectionMsg(BlinkyBlocksLeaderElectionMinIdElectionMsg *msg): BlinkyBlocksMessage(msg) {
	id = msg->id;
}

//===========================================================================================================
//
//          BlinkyBlocksLeaderElectionMinIdBackMsg  (class)
//
//===========================================================================================================

BlinkyBlocksLeaderElectionMinIdBackMsg::BlinkyBlocksLeaderElectionMinIdBackMsg(unsigned int i, bool a) : BlinkyBlocksMessage(){
	type = BB_CS_LE_MIN_ID_BACK_MESSAGE;
	msize = 1 + 1 + 2; // type: CLOCK_SYNC, LEADER ELECTION BACK, MIN ID
	id = i;
	answer = a;
}

BlinkyBlocksLeaderElectionMinIdBackMsg::BlinkyBlocksLeaderElectionMinIdBackMsg(BlinkyBlocksLeaderElectionMinIdBackMsg *msg): BlinkyBlocksMessage(msg) {
	id = msg->id;
	answer = msg->answer;
}

//===========================================================================================================
//
//          BlinkyBlocksLEBarBlockDistanceMsg  (class)
//
//===========================================================================================================

BlinkyBlocksLEBarBlockDistanceMsg::BlinkyBlocksLEBarBlockDistanceMsg(unsigned int i, unsigned int d) {	
	type = BB_CS_LE_BAR_BLOCK_DISTANCE_MESSAGE;
	msize = 2 + 1;
	id = i;
	distance = d;
}

//===========================================================================================================
//
//          BlinkyBlocksLEBarSumRequestMsg  (class)
//
//===========================================================================================================

BlinkyBlocksLEBarSumRequestMsg::BlinkyBlocksLEBarSumRequestMsg() {
	type = BB_CS_LE_BAR_SUM_REQUEST_MESSAGE;
	msize = 2; // ?
}

//===========================================================================================================
//
//          BlinkyBlocksLEBarSumInfoMsg  (class)
//
//===========================================================================================================
	
BlinkyBlocksLEBarSumInfoMsg::BlinkyBlocksLEBarSumInfoMsg(unsigned int s) {
	type = BB_CS_LE_BAR_SUM_INFO_MESSAGE;
	msize = 2; // ?
	sum = s;
}

//===========================================================================================================
//
//          ST_go_message  (class)
//
//===========================================================================================================

ST_go_message::ST_go_message(unsigned int l) {
	type = BB_CS_ST_GO_MESSAGE;
	msize = 1;
	level = l; 
}

//===========================================================================================================
//
//          ST_back_message  (class)
//
//===========================================================================================================

ST_back_message::ST_back_message(bool b, unsigned int l) {
	type = BB_CS_ST_BACK_MESSAGE;
	msize = 1+1;
	answer = b;
	level = l;
}

//===========================================================================================================
//
//          BlinkyBlocksClockSyncClockInfoMsg  (class)
//
//===========================================================================================================
/*
uint8_t BlinkyBlocksClockSyncInfoMsg::waveIdCnt = 1;

BlinkyBlocksClockSyncInfoMsg::BlinkyBlocksClockSyncInfoMsg(uint32_t c) : BlinkyBlocksMessage() {
	clock = c;
	waveId = waveIdCnt;
	waveIdCnt++;
	msize = 1 +  1 + 1 + 4; // type (CLOCK_SYNC) + CLOCK INFO + Wave Id + clock value
	nbhops = 0;
	type = BB_CS_CLOCK_INFO_MESSAGE;
}

BlinkyBlocksClockSyncInfoMsg::BlinkyBlocksClockSyncInfoMsg(uint32_t c, uint8_t wId, uint8_t n) : BlinkyBlocksMessage() {
	clock = c;
	waveId = wId;
	nbhops = n;
	msize = 1 + 1 + 4;
	type = BB_CS_CLOCK_INFO_MESSAGE;
}*/

BlinkyBlocksClockSyncInfoMsg::BlinkyBlocksClockSyncInfoMsg(uint32_t c) : BlinkyBlocksMessage() {
	clock = c;
	msize = 1 +  1 + 1 + 4; // type (CLOCK_SYNC) + CLOCK INFO + Wave Id + clock value
	nbhops = 0;
	type = BB_CS_CLOCK_INFO_MESSAGE;
}

BlinkyBlocksClockSyncInfoMsg::BlinkyBlocksClockSyncInfoMsg(uint32_t c, uint8_t n) : BlinkyBlocksMessage() {
	clock = c;
	nbhops = n;
	msize = 1 + 1 + 4;
	type = BB_CS_CLOCK_INFO_MESSAGE;
}

//===========================================================================================================
//
//          BlinkyBlocksClockSyncRequestMsg  (class)
//
//===========================================================================================================

BlinkyBlocksClockSyncRequestMsg::BlinkyBlocksClockSyncRequestMsg() : BlinkyBlocksMessage() {
	type = BB_CS_CLOCK_REQUEST_MESSAGE;
	msize = 1 + 1; // type: CLOCK_SYNC, CLOCK REQUEST
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
	//cout << hostBlock->blockId << " network trans. time: " << t * 1000 << endl;
	return (t * 1000); // ms to us (simulator unit)
}

}
