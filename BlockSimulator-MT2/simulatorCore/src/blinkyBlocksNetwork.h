/*
 * blinkyBlocksNetwork.h
 *
 *  Created on: 19 fevrier 2014
 *      Author: andre
 */

#ifndef BLINKYBLOCKSNETWORK_H_
#define BLINKYBLOCKSNETWORK_H_

#include <network.h>

namespace BlinkyBlocks {

//using namespace std;

class BlinkyBlocksMessage;
class SerialNetworkInterface;


//===========================================================================================================
//
//          BlinkyBlocksMessage  (class)
//
//===========================================================================================================

#define BB_MELD_VM_MESSAGE 36000
#define BB_CLOCK_SYNC_MESSAGE 36001


class BlinkyBlocksMessage: public Message {
protected:
public:
	unsigned int msize;

	BlinkyBlocksMessage(unsigned int s);
	BlinkyBlocksMessage() {};
	virtual ~BlinkyBlocksMessage() {};

	virtual unsigned int size();
	static unsigned int nbChunks(unsigned int s);
};

//===========================================================================================================
//
//          BlinkyBlocksClockSyncMsg  (class)
//
//===========================================================================================================

class BlinkyBlocksClockSyncMsg;
typedef boost::shared_ptr<BlinkyBlocksClockSyncMsg> BlinkyBlocksClockSyncMsg_ptr;

class BlinkyBlocksClockSyncMsg: public BlinkyBlocksMessage {
	static uint8_t waveIdCnt;
public:
	uint32_t clock;
	uint8_t waveId;
	
	BlinkyBlocksClockSyncMsg(uint32_t c);
	BlinkyBlocksClockSyncMsg(uint32_t c, uint8_t wId);
	~BlinkyBlocksClockSyncMsg() {};
};

//===========================================================================================================
//
//          SerialNetworkInterface  (class)
//
//===========================================================================================================

class SerialNetworkInterface : public P2PNetworkInterface {

public:
	SerialNetworkInterface(BaseSimulator::BuildingBlock *b);
	~SerialNetworkInterface();
	unsigned int computeTransmissionDuration(unsigned int size);
};

}

#endif /* BLINKYBLOCKSNETWORK_H_ */
