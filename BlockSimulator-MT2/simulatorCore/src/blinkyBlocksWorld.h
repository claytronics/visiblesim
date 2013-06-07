/*
 * blinkyBlocksWorld.h
 *
 *  Created on: 23 mars 2013
 *      Author: dom
 */

#ifndef BLINKYBLOCKSWORLD_H_
#define BLINKYBLOCKSWORLD_H_

#include "world.h"
#include "vecteur.h"
#include "blinkyBlocksBlock.h"
#include <boost/asio.hpp> 

using boost::asio::ip::tcp;

namespace BlinkyBlocks {

class BlinkBlocksWorld : BaseSimulator::World {
protected:
	int gridWidth,gridHeight;	
	boost::asio::io_service ios;
	tcp::acceptor *acceptor;

	BlinkBlocksWorld();
	BlinkBlocksWorld(int gw,int gh);
	virtual ~BlinkBlocksWorld();

public:
	//static void createWorld();
	static void createWorld(int gw,int gh);
	static void deleteWorld();
	virtual void addBlock(int blockId, BlinkyBlocksBlockCode *(*blinkyBlocksBlockCodeBuildingFunction)(BlinkyBlocksBlock*),const Vecteur &pos,const Vecteur &col);
	static BlinkBlocksWorld* getWorld() {
		assert(world != NULL);
		return((BlinkBlocksWorld*)world);
	}
	
	map<int, BaseSimulator::BuildingBlock*>& getBuildingBlocksMap() {
		return buildingBlocksMap;
	}

	void printInfo() {
		cout << "I'm a BlinkBlocksWorld" << endl;
	}

	virtual void addBlock(int i) { }

};

inline void createWorld(int gw,int gh) {
	BlinkBlocksWorld::createWorld(gw, gh);
}

inline void deleteWorld() {
	BlinkBlocksWorld::deleteWorld();
}

inline BlinkBlocksWorld* getWorld() { return(BlinkBlocksWorld::getWorld()); }

} // BlinkyBlocks namespace


#endif /* BLINKYBLOCKSWORLD_H_ */
