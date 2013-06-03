/*
 * multiCoresSimulator.h
 *
 *  Created on: 14 févr. 2013
 *      Author: dom
 */

#ifndef MULTICORESSIMULATOR_H_
#define MULTICORESSIMULATOR_H_

#include "simulator.h"
#include "multiCoresBlock.h"

#define VM_MESSAGE_TYPE_START_SIMULATION        1
#define VM_MESSAGE_TYPE_END_SIMULATION          2
#define VM_MESSAGE_TYPE_CREATE_LINK             3
#define VM_MESSAGE_TYPE_COMPUTATION_LOCK        4
#define VM_MESSAGE_TYPE_COMPUTATION_UNLOCK      5
#define VM_MESSAGE_TYPE_SEND_MESSAGE            6
#define VM_MESSAGE_TYPE_RECEIVE_MESSAGE         7
#define VM_MESSAGE_TYPE_WAIT_FOR_MESSAGE		8



class MultiCoresSimulator : public Simulator {
public:
	MultiCoresBlock *b1;

	static MultiCoresBlockCode *(*buildNewBlockCode)(MultiCoresBlock*);

	MultiCoresSimulator(int argc, char *argv[], MultiCoresBlockCode *(*multiCoreBlockCodeBuildingFunction)(MultiCoresBlock*));
	~MultiCoresSimulator();

	void initWorld();
};

#endif /* MULTICORESSIMULATOR_H_ */
