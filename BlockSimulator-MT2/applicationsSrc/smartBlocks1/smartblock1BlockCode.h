/*
 * smartblock1BlockCode.h
 *
 *  Created on: 12 avril 2013
 *      Author: ben
 */

#ifndef SMARTBLOCK1BLOCKCODE_H_
#define SMARTBLOCK1BLOCKCODE_H_

#define DIST_MSG_ID	9001
#define ACK_MSG_ID	9002

#define NEIGHBORS_NUMBER 4
#define MAX_DIST 1000

#include "smartBlocksBlockCode.h"
#include "smartBlocksSimulator.h"
#include "smartBlocksScheduler.h"
#include "smartBlocksBlock.h"


class Dist_message;
class Ack_message;


typedef boost::shared_ptr<Dist_message> Dist_message_ptr;
typedef boost::shared_ptr<Ack_message> Ack_message_ptr;


class SmartBlock1BlockCode : public SmartBlocks::SmartBlocksBlockCode {
	unsigned int my_distance;
	//~ unsigned int neighbors_dist[NEIGHBORS_NUMBER];
	bool isAck[ NEIGHBORS_NUMBER];
	P2PNetworkInterface * distance_dealer;

public:

	SmartBlocks::SmartBlocksScheduler *scheduler;
	SmartBlocks::SmartBlocksBlock *smartBlock;

	SmartBlock1BlockCode (SmartBlocks::SmartBlocksBlock *host);
	~SmartBlock1BlockCode ();

	void startup();
	void processLocalEvent(EventPtr pev);

	static SmartBlocks::SmartBlocksBlockCode *buildNewBlockCode( SmartBlocks::SmartBlocksBlock *host);

	void send_dist( unsigned int distance,  P2PNetworkInterface * by_interface, uint64_t time_offset);
	void send_ack( unsigned int distance,  P2PNetworkInterface * by_interface, uint64_t time_offset);

	bool i_can_ack();
};

class Dist_message : public Message {
	unsigned int distance;
public :
	Dist_message( unsigned int);
	~Dist_message();

	unsigned int getDistance() { return distance; };
	//~ virtual unsigned int size() { cout << "appel a size"<<endl; return(4); }
};

class Ack_message : public Message {
	unsigned int distance;
public :
	unsigned int getDistance() { return distance; };
	Ack_message( unsigned int);
	~Ack_message();
};

#endif /* BLINKY01BLOCKCODE_H_ */
