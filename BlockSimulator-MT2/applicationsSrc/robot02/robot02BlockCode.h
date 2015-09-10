/*
 * robot02BlockCode.h
 *
 *  Created on: 12 janvier 2014
 *      Author: Benoît
 */

#ifndef ROBOT02BLOCKCODE_H_
#define ROBOT02BLOCKCODE_H_

#include "robotBlocksBlockCode.h"
#include "robotBlocksSimulator.h"
#include "robotBlocksScheduler.h"
#include "robotBlocksBlock.h"

#define COLOR_MESSAGE	1001
#define SEARCH_MASTER_MESSAGE	1002
#define RETURN_MASTER_MESSAGE	1003

const int COM_DELAY=100000;

class SearchMasterMessage : public Message {
public :
	int blockId;
	Color blockColor;
	SearchMasterMessage(int i,Color c):blockId(i),blockColor(c) { type=SEARCH_MASTER_MESSAGE; };
	~SearchMasterMessage() {};
};

class ReturnMasterMessage : public Message {
public :
	int blockId;
	Color blockColor;
	ReturnMasterMessage(int i,Color c):blockId(i),blockColor(c) { type=RETURN_MASTER_MESSAGE; };
	~ReturnMasterMessage() {};
};

class ColorMessage : public Message {
public :
	Color color;
	ColorMessage(Color c):color(c) { type=COLOR_MESSAGE; };
	~ColorMessage() {};
};

typedef boost::shared_ptr<ColorMessage> ColorMessage_ptr;
typedef boost::shared_ptr<SearchMasterMessage> SearchMasterMessage_ptr;
typedef boost::shared_ptr<ReturnMasterMessage> ReturnMasterMessage_ptr;

using namespace std;
using namespace RobotBlocks;

class Robot02BlockCode : public RobotBlocks::RobotBlocksBlockCode {
	int masterId;
	Color masterColor;
	bool searchDone;
	int nbreOfWaitedAnswers;
	P2PNetworkInterface *block2answer;
	bool colored;
public:
	RobotBlocks::RobotBlocksScheduler *scheduler;
	RobotBlocks::RobotBlocksBlock *block;

	Robot02BlockCode (RobotBlocks::RobotBlocksBlock *host);
	~Robot02BlockCode ();

	void startup();
	void processLocalEvent(EventPtr pev);

	static RobotBlocks::RobotBlocksBlockCode *buildNewBlockCode( RobotBlocks::RobotBlocksBlock *host);
    void sendMasterMessageToAllNeighbors(P2PNetworkInterface *except);
    void sendReturnMessageTo(P2PNetworkInterface *p2p);
    void sendColorMessageToAllNeighbors(P2PNetworkInterface *except);
};

#endif /* ROBOT2BLOCKCODE_H_ */
