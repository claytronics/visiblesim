/*
 * flavio01BlockCode.h
 *
 *  Created on: 15 févr. 2013
 *      Author: dom
 */

#ifndef FLAVIO01BLOCKCODE_H_
#define FLAVIO01BLOCKCODE_H_

#include "multiCoresBlockCode.h"
#include "multiCoresSimulator.h"

class Flavio01BlockCode : MultiCoresBlockCode {
public:
	bool computing;
	bool waitingForVM;

	Flavio01BlockCode(MultiCoresBlock *host);
	~Flavio01BlockCode();

	void startup();
	void processLocalEvent(EventPtr pev);

	static Flavio01BlockCode *buildNewBlockCode(MultiCoresBlock *host);
};

#endif /* FLAVIO01BLOCKCODE_H_ */
