/*
 * blockCode.h
 *
 *  Created on: 15 févr. 2013
 *      Author: dom
 */

#ifndef BLOCKCODE_H_
#define BLOCKCODE_H_

#include "buildingBlock.h"

class BlockCode {
public:
	BuildingBlock *hostBlock;
	uint64_t availabilityDate;

	BlockCode(BuildingBlock *host);
	~BlockCode();

	virtual void startup() = 0;
	virtual void processLocalEvent(EventPtr pev) = 0;

	//static BlockCode *buildNewBlockCode();
};

#endif /* BLOCKCODE_H_ */
