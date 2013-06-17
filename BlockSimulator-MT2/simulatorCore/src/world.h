/*
 * world.h
 *
 *  Created on: 23 mars 2013
 *      Author: dom
 */

#ifndef WORLD_H_
#define WORLD_H_

#include <iostream>
#include <map>
#include <vector>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include "assert.h"
#include "buildingBlock.h"
#include "glBlock.h"

using namespace std;

namespace BaseSimulator {

class World {
	boost::interprocess::interprocess_mutex mutex_gl;

protected:
	static World *world;
	static map<int, BuildingBlock*>buildingBlocksMap;
	static vector<GlBlock*>tabGlBlocks;
	GlBlock *selectedBlock;

	World();
	virtual ~World();
public:
	static World* getWorld() {
		assert(world != NULL);
		return(world);
	}
	static void deleteWorld() {
		delete(world);
		world=NULL;
	}

	void printInfo() {
		cout << "I'm a World" << endl;
	}
	virtual BuildingBlock* getBlockById(int bId);

	inline GlBlock* getSelectedBlock() { return selectedBlock; };
	inline GlBlock* setSelectedBlock(int n) { return (selectedBlock=tabGlBlocks[n]); };
	virtual void setSelectedFace(int n) {};
	inline GlBlock* getBlockByNum(int n) { return tabGlBlocks[n]; };

	void lock();
	void unlock();
	virtual void glDraw() {};
	virtual void glDrawId() {};
	virtual void glDrawIdByMaterial() {};
	virtual void createPopupMenu(int ix,int iy) {};
	virtual Camera *getCamera() { return NULL; };
	virtual void menuChoice(int) {};
	virtual void tapBlock(int bId) {};
	virtual void accelBlock(int bId, int x, int y, int z) {};
	virtual void shakeBlock(int bId, int f) {};
};

inline void deleteWorld() {
	World::deleteWorld();
}

inline World* getWorld() { return(World::getWorld()); }

} // BaseSimulator namespace

#endif /* WORLD_H_ */
