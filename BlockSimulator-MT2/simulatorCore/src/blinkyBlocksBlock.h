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
#include <boost/asio.hpp> 
#include <stdexcept>

namespace BlinkyBlocks {

enum NeighborDirection { Front=0, Back, Left, Right, Top, Bottom };

class BlinkyBlocksBlockCode;
class BlinkyBlocksVM;

class BlinkyBlocksBlock : public BaseSimulator::BuildingBlock {
	P2PNetworkInterface *tabInterfaces[6];

public:
	BlinkyBlocksGlBlock *ptrGlBlock;
	Vecteur color; // color of the block
	Vecteur position; // position of the block;
	BlinkyBlocksVM *vm;

	BlinkyBlocksBlockCode *(*buildNewBlockCode)(BlinkyBlocksBlock*);
	BlinkyBlocksBlock(int bId, BlinkyBlocksBlockCode *(*blinkyBlocksBlockCodeBuildingFunction)(BlinkyBlocksBlock*));
	~BlinkyBlocksBlock();

	inline BlinkyBlocksGlBlock* getGlBlock() { return ptrGlBlock; };
	inline void setGlBlock(BlinkyBlocksGlBlock*ptr) { ptrGlBlock=ptr;};
	void setColor(const Vecteur &c);
	void setColor(int num);
	void setPosition(const Vecteur &p);
	inline P2PNetworkInterface *getInterface(NeighborDirection d) { return tabInterfaces[d]; }
	NeighborDirection getDirection(P2PNetworkInterface*);

	void killVM();

	/* schedule the appropriate event for this action */
	void tap();
	void accel(int x, int y, int z);	
	void shake(int f);	
	void addNeighbor(P2PNetworkInterface *ni, BuildingBlock* target);
	void removeNeighbor(P2PNetworkInterface *ni);	
	void stop();

};

}

#endif /* BLINKYBLOCKSBLOCK_H_ */
