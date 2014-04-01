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

#define BB_MELD_VM_MESSAGE 					36000
#define BB_CS_LE_MIN_ID_ELECTION_MESSAGE 	36001
#define BB_CS_LE_MIN_ID_BACK_MESSAGE 		36002
#define BB_CS_LE_BAR_BLOCK_DISTANCE_MESSAGE 36003
#define BB_CS_LE_BAR_SUM_REQUEST_MESSAGE 	36004
#define BB_CS_LE_BAR_SUM_INFO_MESSAGE 		36005
#define BB_CS_ST_GO_MESSAGE					36006
#define BB_CS_ST_BACK_MESSAGE     			36007
#define BB_CS_CLOCK_REQUEST_MESSAGE 		36010
#define BB_CS_CLOCK_INFO_MESSAGE 			36011

class BlinkyBlocksMessage: public Message {
protected:
public:
	unsigned int msize;

	BlinkyBlocksMessage(unsigned int s);
	BlinkyBlocksMessage() : Message() {};
	BlinkyBlocksMessage(BlinkyBlocksMessage *msg);
	
	virtual ~BlinkyBlocksMessage() {};

	virtual unsigned int size();
	static unsigned int nbChunks(unsigned int s);
};

//===========================================================================================================
//
//          BlinkyBlocksLEMinIdElectionMsg  (class)
//
//===========================================================================================================

class BlinkyBlocksLeaderElectionMinIdElectionMsg;
typedef boost::shared_ptr<BlinkyBlocksLeaderElectionMinIdElectionMsg> BlinkyBlocksLeaderElectionMinIdElectionMsg_ptr;

class BlinkyBlocksLeaderElectionMinIdElectionMsg: public BlinkyBlocksMessage {
public:
	unsigned int id;
	
	BlinkyBlocksLeaderElectionMinIdElectionMsg(unsigned int i);
	BlinkyBlocksLeaderElectionMinIdElectionMsg(BlinkyBlocksLeaderElectionMinIdElectionMsg *msg);
	~BlinkyBlocksLeaderElectionMinIdElectionMsg() {};
};

//===========================================================================================================
//
//          BlinkyBlocksLEMinIdBackMsg  (class)
//
//===========================================================================================================

class BlinkyBlocksLeaderElectionMinIdBackMsg;
typedef boost::shared_ptr<BlinkyBlocksLeaderElectionMinIdBackMsg> BlinkyBlocksLeaderElectionMinIdBackMsg_ptr;

class BlinkyBlocksLeaderElectionMinIdBackMsg: public BlinkyBlocksMessage {
public:
	unsigned int id;
	bool answer;
	
	BlinkyBlocksLeaderElectionMinIdBackMsg(unsigned int i, bool a);
	BlinkyBlocksLeaderElectionMinIdBackMsg(BlinkyBlocksLeaderElectionMinIdBackMsg *msg);
	~BlinkyBlocksLeaderElectionMinIdBackMsg() {};
};

//===========================================================================================================
//
//          BlinkyBlocksLEBarBlockDistanceMsg  (class)
//
//===========================================================================================================

class BlinkyBlocksLEBarBlockDistanceMsg;
typedef boost::shared_ptr<BlinkyBlocksLEBarBlockDistanceMsg> BlinkyBlocksLEBarBlockDistanceMsg_ptr;

class BlinkyBlocksLEBarBlockDistanceMsg: public BlinkyBlocksMessage {
public:
	unsigned int id;
	unsigned int distance;
	
	BlinkyBlocksLEBarBlockDistanceMsg(unsigned int i, unsigned int d);
	~BlinkyBlocksLEBarBlockDistanceMsg() {};
};

//===========================================================================================================
//
//          BlinkyBlocksLEBarSumRequestMsg  (class)
//
//===========================================================================================================

class BlinkyBlocksLEBarSumRequestMsg;
typedef boost::shared_ptr<BlinkyBlocksLEBarSumRequestMsg> BlinkyBlocksLEBarSumRequestMsg_ptr;

class BlinkyBlocksLEBarSumRequestMsg: public BlinkyBlocksMessage {
public:

	BlinkyBlocksLEBarSumRequestMsg();
	~BlinkyBlocksLEBarSumRequestMsg() {};
};

//===========================================================================================================
//
//          BlinkyBlocksLEBarSumInfoMsg  (class)
//
//===========================================================================================================

class BlinkyBlocksLEBarSumInfoMsg;
typedef boost::shared_ptr<BlinkyBlocksLEBarSumInfoMsg> BlinkyBlocksLEBarSumInfoMsg_ptr;

class BlinkyBlocksLEBarSumInfoMsg: public BlinkyBlocksMessage {
public:
	unsigned int sum;
	
	BlinkyBlocksLEBarSumInfoMsg(unsigned int s);
	~BlinkyBlocksLEBarSumInfoMsg() {};
};

//===========================================================================================================
//
//          ST_go_message  (class)
//
//===========================================================================================================

class ST_go_message;
typedef boost::shared_ptr<ST_go_message>  ST_go_message_ptr;

class ST_go_message : public BlinkyBlocksMessage {
public :	
	unsigned int level;
	
	ST_go_message(unsigned int l);
	~ST_go_message() {};
};

//===========================================================================================================
//
//          ST_back_message  (class)
//
//===========================================================================================================

class ST_back_message;
typedef boost::shared_ptr<ST_back_message> ST_back_message_ptr;

class ST_back_message : public BlinkyBlocksMessage {
public :	
	bool answer;
	unsigned int level;
	
	ST_back_message(bool b, unsigned int l);
	~ST_back_message() {};
};

//===========================================================================================================
//
//          BlinkyBlocksCSClockInfoMsg  (class)
//
//===========================================================================================================
/*
class BlinkyBlocksClockSyncInfoMsg;
typedef boost::shared_ptr<BlinkyBlocksClockSyncInfoMsg> BlinkyBlocksClockSyncInfoMsg_ptr;

class BlinkyBlocksClockSyncInfoMsg: public BlinkyBlocksMessage {
	static uint8_t waveIdCnt;
public:
	uint32_t clock;
	uint8_t waveId;
	uint8_t nbhops;
	
	BlinkyBlocksClockSyncInfoMsg(uint32_t c);
	BlinkyBlocksClockSyncInfoMsg(uint32_t c, uint8_t wId, uint8_t n);
	~BlinkyBlocksClockSyncInfoMsg() {};
};*/

class BlinkyBlocksClockSyncInfoMsg;
typedef boost::shared_ptr<BlinkyBlocksClockSyncInfoMsg> BlinkyBlocksClockSyncInfoMsg_ptr;

class BlinkyBlocksClockSyncInfoMsg: public BlinkyBlocksMessage {
public:
	uint32_t clock;
	uint8_t nbhops;
	
	BlinkyBlocksClockSyncInfoMsg(uint32_t c);
	BlinkyBlocksClockSyncInfoMsg(uint32_t c, uint8_t n);
	~BlinkyBlocksClockSyncInfoMsg() {};
};

//===========================================================================================================
//
//          BlinkyBlocksClockSyncRequestMsg  (class)
//
//===========================================================================================================

class BlinkyBlocksClockSyncRequestMsg;
typedef boost::shared_ptr<BlinkyBlocksClockSyncRequestMsg> BlinkyBlocksClockSyncRequestMsg_ptr;

class BlinkyBlocksClockSyncRequestMsg: public BlinkyBlocksMessage {
public:	
	BlinkyBlocksClockSyncRequestMsg();
	~BlinkyBlocksClockSyncRequestMsg() {};
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

