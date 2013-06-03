/*
 * multiCoresBlock.h
 *
 *  Created on: 15 févr. 2013
 *      Author: dom
 */

#include "buildingBlock.h"


#ifndef MULTICORESBLOCK_H_
#define MULTICORESBLOCK_H_

class MultiCoresBlockCode;
class MultiCoresSimulator;

class MultiCoresBlock : public BuildingBlock {
protected:
	bool undefinedState;
	static MultiCoresSimulator *simulator;
public:
	MultiCoresBlockCode *(*buildNewBlockCode)(MultiCoresBlock*);

	MultiCoresBlock(MultiCoresBlockCode *(*multiCoreBlockCodeBuildingFunction)(MultiCoresBlock *));
	MultiCoresBlock(unsigned int bId, MultiCoresBlockCode *(*multiCoreBlockCodeBuildingFunction)(MultiCoresBlock *));
	~MultiCoresBlock();

	bool getUndefinedState() { return(undefinedState); }
	void setUndefinedState(bool state) { undefinedState = state; }
};

#endif /* MULTICORESBLOCK_H_ */
