/*
 * buildingBlock.h
 *
 *  Created on: 22 mars 2013
 *      Author: dom
 */

#ifndef BUILDINGBLOCK_H_
#define BUILDINGBLOCK_H_

//#include <tr1/unordered_set>
#include <boost/shared_ptr.hpp>
#include <boost/random.hpp>
#include <list>

#include "glBlock.h"
#include "blockCode.h"


class Event;
typedef boost::shared_ptr<Event> EventPtr;

using namespace std;

class P2PNetworkInterface;

namespace BaseSimulator {

class BlockCode;

//===========================================================================================================
//
//          BuildingBlock  (class)
//
//===========================================================================================================

// alive state must be associated to a number >= 2
enum State {Stopped = 0, Removed = 1, Alive = 2, Computing = 3};

class BuildingBlock {
protected:
	static int nextId;
	//static std::tr1::unordered_set<BuildingBlock*> buildingBlocksSet;

	int P2PNetworkInterfaceNextLocalId;
	list<P2PNetworkInterface*> P2PNetworkInterfaceList;

	list<EventPtr> localEventsList;
public:
	int blockId;
	BlockCode *blockCode;
	State state;
	boost::rand48 generator;

	BuildingBlock(int bId);
	virtual ~BuildingBlock();

	unsigned int getNextP2PInterfaceLocalId();

	P2PNetworkInterface *getP2PNetworkInterfaceByDestBlockId(int destBlockId);
	bool addP2PNetworkInterfaceAndConnectTo(BuildingBlock *destBlock);
	bool addP2PNetworkInterfaceAndConnectTo(int destBlockId);
	P2PNetworkInterface *getP2PNetworkInterfaceByBlockRef(BuildingBlock *destBlock);

	void scheduleLocalEvent(EventPtr pev);
	void processLocalEvent();

	virtual void updateGlData() {};
	
	virtual void addNeighbor(P2PNetworkInterface *ni, BuildingBlock* target) {};
	virtual void removeNeighbor(P2PNetworkInterface *ni) {};
	
	virtual void stop() {};
	int getState() { return state; };
	virtual void setState(int s) { state = State(s); };
	
	int getNextRandomNumber();
};

} // BaseSimulator namespace

#endif /* BUILDINGBLOCK_H_ */
