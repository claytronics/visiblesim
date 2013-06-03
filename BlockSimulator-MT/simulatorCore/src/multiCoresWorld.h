/*
 * multiCoresWorld.h
 *
 *  Created on: 16 févr. 2013
 *      Author: dom
 */

#ifndef MULTICORESWORLD_H_
#define MULTICORESWORLD_H_

#include "world.h"
#include "multiCoresBlock.h"

class MultiCoresWorld : public World {
public:
	MultiCoresWorld();
	~MultiCoresWorld();

	void addBlock(int blockId, MultiCoresBlockCode *(*multiCoreBlockCodeBuildingFunction)(MultiCoresBlock*), float posX, float posY, float posZ, float colorR, float colorG, float colorB);
};

#endif /* MULTICORESWORLD_H_ */
