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

namespace BlinkyBlocks {

class BlinkyBlocksBlockCode;

class BlinkyBlocksBlock : public BaseSimulator::BuildingBlock {
public:
	BlinkyBlocksBlockCode *(*buildNewBlockCode)(BlinkyBlocksBlock*);

	BlinkyBlocksBlock(int bId, BlinkyBlocksBlockCode *(*blinkyBlocksBlockCodeBuildingFunction)(BlinkyBlocksBlock *));
	~BlinkyBlocksBlock();
};

}

#endif /* BLINKYBLOCKSBLOCK_H_ */
