/*
 * blinkyBlocksBlock.h
 *
 *  Created on: 23 mars 2013
 *      Author: dom
 */

#ifndef BLINKYBLOCKSBLOCK_H_
#define BLINKYBLOCKSBLOCK_H_

#include "buildingBlock.h"
#include "blinkyBlocksVM.h"
#include "blinkyBlocksBlockCode.h"
#include "blinkyBlocksGlBlock.h"
#include "blinkyBlocksClock.h"
#include <boost/asio.hpp> 
#include <stdexcept>
#include <list>

namespace BlinkyBlocks {

class NeighborDirection {
public:
	enum Direction { Bottom = 0, Back = 1, Right, Front, Left, Top };
	static int getOpposite(int d);
	static string getString(int d);
};

class BlinkyBlocksBlockCode;
class meldVM;

class BlinkyBlocksBlock : public BaseSimulator::BuildingBlock {
	P2PNetworkInterface *tabInterfaces[6];

protected:
	boost::interprocess::interprocess_mutex mutex_vm;
	
public:
	BlinkyBlocksGlBlock *ptrGlBlock;
	Vecteur color; // color of the block
	Vecteur position; // position of the block;
	BlinkyBlocksVM *vm;
	
	BlinkyBlocksClock localClock;

	BlinkyBlocksBlockCode *(*buildNewBlockCode)(BlinkyBlocksBlock*);
	BlinkyBlocksBlock(int bId, BlinkyBlocksBlockCode *(*blinkyBlocksBlockCodeBuildingFunction)(BlinkyBlocksBlock*));
	~BlinkyBlocksBlock();

	inline BlinkyBlocksGlBlock* getGlBlock() { return ptrGlBlock; };
	inline void setGlBlock(BlinkyBlocksGlBlock*ptr) { ptrGlBlock=ptr;};
	void setColor(const Vecteur &c);
	void setColor(int num);
	void setPosition(const Vecteur &p);
	inline P2PNetworkInterface *getInterface(NeighborDirection::Direction d) { return tabInterfaces[d]; }
	inline P2PNetworkInterface *getInterfaceDestId(int id) {
		for (int i=0; i<6; i++) {
			if (tabInterfaces[i]->connectedInterface != NULL) {
					if (tabInterfaces[i]->connectedInterface->hostBlock->blockId == id) {
							return tabInterfaces[i];
					}
			}
		}
		return NULL;
	}
	unsigned int getNbNeighbors();
	
	NeighborDirection::Direction getDirection(P2PNetworkInterface*);

	void killVM();

	/* schedule the appropriate event for this action */
	void tap(uint64_t date);
	void accel(uint64_t date, int x, int y, int z);	
	void shake(uint64_t date, int f);	
	void addNeighbor(P2PNetworkInterface *ni, BuildingBlock* target);
	void removeNeighbor(P2PNetworkInterface *ni);	
	void stop(uint64_t date, State s);
	
	/* Lock activated only in debugging mode */
	void lockVM();
	void unlockVM();
	
	int sendCommand(VMCommand &c);
	
	// DOES NOT WORK, TO FIX
	//unsigned int broadcastMessage(Message *msg, P2PNetworkInterface* excluded); 
	//unsigned int spreadMessage(Message *msg, list<P2PNetworkInterface*> l);
	
};

std::ostream& operator<<(std::ostream &stream, BlinkyBlocksBlock const& bb);

}

#endif /* BLINKYBLOCKSBLOCK_H_ */
