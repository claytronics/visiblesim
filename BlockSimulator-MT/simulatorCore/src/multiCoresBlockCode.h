/*
 * multiCoresBlockCode.h
 *
 *  Created on: 15 févr. 2013
 *      Author: dom
 */

#ifndef MULTICORESBLOCKCODE_H_
#define MULTICORESBLOCKCODE_H_

#include "blockCode.h"
#include "multiCoresBlock.h"

class MultiCoresSimulator;

class MultiCoresBlockCode : public BlockCode {
protected:
	static MultiCoresSimulator *simulator;
public:
	MultiCoresBlockCode(MultiCoresBlock *host);
	~MultiCoresBlockCode();
};
#endif /* MULTICORESBLOCKCODE_H_ */
