/*
 * blinkyBlocksWorld.h
 *
 *  Created on: 23 mars 2013
 *      Author: dom
 */

#ifndef BLINKYBLOCKSWORLD_H_
#define BLINKYBLOCKSWORLD_H_

#include "world.h"

namespace BlinkyBlocks {

class BlinkBlocksWorld : BaseSimulator::World {
protected:
	BlinkBlocksWorld();
	virtual ~BlinkBlocksWorld();

public:
	static void createWorld();
	static void deleteWorld();
	static BlinkBlocksWorld* getWorld() {
		assert(world != NULL);
		return((BlinkBlocksWorld*)world);
	}

	void printInfo() {
		cout << "I'm a BlinkBlocksWorld" << endl;
	}

	virtual void addBlock(int i) { }

};

inline void createWorld() {
	BlinkBlocksWorld::createWorld();
}

inline void deleteWorld() {
	BlinkBlocksWorld::deleteWorld();
}

inline BlinkBlocksWorld* getWorld() { return(BlinkBlocksWorld::getWorld()); }

} // BlinkyBlocks namespace


#endif /* BLINKYBLOCKSWORLD_H_ */
