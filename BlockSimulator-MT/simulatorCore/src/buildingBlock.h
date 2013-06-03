/*
 * buildingBlock.h
 *
 *  Created on: 15 févr. 2013
 *      Author: dom
 */

#ifndef BUILDINGBLOCK_H_
#define BUILDINGBLOCK_H_

#include <deque>
#include <vector>
#include <list>
#include <inttypes.h>
#include "message.h"

class BlockCode;
class BuildingBlock;
class Event;
typedef boost::shared_ptr<Event> EventPtr;

using namespace std;

//===========================================================================================================
//
//          P2PNetworkInterface  (class)
//
//===========================================================================================================

class P2PNetworkInterface {
protected:
	static unsigned int nextId;
	static unsigned int defaultDataRate;
	unsigned int dataRate;
public:
	unsigned int id;
	unsigned int localId;
	deque<MessagePtr> outgoingQueue;
	deque<MessagePtr> incomingQueue;
	P2PNetworkInterface *connectedInterface;
	BuildingBlock *block;
	uint64_t availabilityDate;
	MessagePtr messageBeingTransmitted;

	P2PNetworkInterface(BuildingBlock *b);
	~P2PNetworkInterface();
	bool addToOutgoingBuffer(MessagePtr msg);
	void send();
	void connect(P2PNetworkInterface *ni);
	void disconnect();
	static void setDefaultDataRate(unsigned int rate) { defaultDataRate = rate; }
	void setDataRate(unsigned int rate) { dataRate = rate; }
};

//===========================================================================================================
//
//          BuildingBlock  (class)
//
//===========================================================================================================


class BuildingBlock {
protected:
	static unsigned int nextID;
	static vector<BuildingBlock*> buildingBlocksVector;
	list<P2PNetworkInterface*> P2PNetworkInterfaceList;
	unsigned int P2PNetworkInterfaceNextLocalId;
public:

	unsigned int blockID;
	BlockCode *blockCode;


	list<EventPtr> localEventsList;

	BuildingBlock();
	BuildingBlock(unsigned int bid);
	~BuildingBlock();

	void setBlockCode(BlockCode *bc);

	unsigned int getNextP2PInterfaceLocalId();
	static BuildingBlock *getBlocByID(unsigned int bId);
	bool addP2PNetworkInterface(BuildingBlock *destBlock);
	bool addP2PNetworkInterface(unsigned int destBlockID);
	bool removeP2PNetworkInterface(BuildingBlock *destBlock);
	P2PNetworkInterface *getP2PNetworkInterfaceByBlockRef(BuildingBlock *destBlock);
	P2PNetworkInterface *getP2PNetworkInterfaceByDestBlockId(unsigned int destBlockID);
	P2PNetworkInterface *getP2PNetworkInterfaceById(unsigned int id);
	P2PNetworkInterface *getP2PNetworkInterfaceByLocalId(unsigned int id);

	void scheduleLocalEvent(EventPtr pev);
	void processLocalEvent();


};


#endif /* BUILDINGBLOCK_H_ */
