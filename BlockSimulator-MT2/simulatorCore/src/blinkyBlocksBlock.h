/*
 * blinkyBlocksBlock.h
 *
 *  Created on: 23 mars 2013
 *      Author: dom
 */

#ifndef BLINKYBLOCKSBLOCK_H_
#define BLINKYBLOCKSBLOCK_H_

#include "buildingBlock.h"
#include "blinkyBlocksBlockCode.h"
#include "blinkyBlocksGlBlock.h"

namespace BlinkyBlocks {
enum NeighborDirection { Front=0, Back, Left, Right, Top, Bottom };

class BlinkyBlocksBlockCode;

class BlinkyBlocksBlock : public BaseSimulator::BuildingBlock {
	P2PNetworkInterface *tabInterfaces[6];

public:
	BlinkyBlocksGlBlock *ptrGlBlock;
	Vecteur color; // color of the block
	Vecteur position; // position of the block;

	BlinkyBlocksBlockCode *(*buildNewBlockCode)(BlinkyBlocksBlock*);

	BlinkyBlocksBlock(int bId, BlinkyBlocksBlockCode *(*blinkyBlocksBlockCodeBuildingFunction)(BlinkyBlocksBlock *));
	~BlinkyBlocksBlock();

	inline BlinkyBlocksGlBlock* getGlBlock() { return ptrGlBlock; };
	inline void setGlBlock(BlinkyBlocksGlBlock*ptr) { ptrGlBlock=ptr;};
	void setColor(const Vecteur &c);
	void setColor(int num);
	void setPosition(const Vecteur &p);
	inline P2PNetworkInterface *getInterface(NeighborDirection d) { return tabInterfaces[d]; }
	NeighborDirection getDirection( P2PNetworkInterface*);
};

}

#endif /* BLINKYBLOCKSBLOCK_H_ */
