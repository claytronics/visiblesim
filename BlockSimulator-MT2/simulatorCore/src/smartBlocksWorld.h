/*
 * smartBlocksWorld.h
 *
 *  Created on: 12 avril 2013
 *      Author: ben
 */

#ifndef SMARTBLOCKSWORLD_H_
#define SMARTBLOCKSWORLD_H_

#include "openglViewer.h"
#include "world.h"
#include "vecteur.h"
#include "smartBlocksBlock.h"
#include "objLoader.h"

namespace SmartBlocks {

class SmartBlocksWorld : BaseSimulator::World {
protected:
	int gridWidth,gridHeight;
	SmartBlocksBlock **tabPtrBlocks;
	GLfloat blockSize[3];
	GLuint idTextureFloor;
	Camera *camera;
	ObjLoader::ObjLoader *objBlock,*objRepere;

	SmartBlocksWorld(int gw,int gh,int argc, char *argv[]);
	virtual ~SmartBlocksWorld();
	inline SmartBlocksBlock* getGridPtr(int x,int y) { return tabPtrBlocks[x+y*gridWidth]; };
	inline void setGridPtr(int x,int y,SmartBlocksBlock *ptr) { tabPtrBlocks[x+y*gridWidth]=ptr; };
public:
	static void createWorld(int gw,int gh,int argc, char *argv[]);
	static void deleteWorld();
	static SmartBlocksWorld* getWorld() {
		assert(world != NULL);
		return((SmartBlocksWorld*)world);
	}

	void printInfo() {
		cout << "I'm a SmartBlocksWorld" << endl;
	}

	virtual void addBlock(int blockId, SmartBlocksBlockCode *(*smartBlockCodeBuildingFunction)(SmartBlocksBlock*),const Vecteur &pos,const Vecteur &col);
	inline void setBlockSize(float *siz) { blockSize[0]=siz[0];blockSize[1]=siz[1];blockSize[2]=siz[2];};
	void linkBlocks();
	void loadTextures(const string &str);
	virtual void glDraw();
	virtual void glDrawId();
	virtual void updateGlData(SmartBlocksBlock*blc);
	inline virtual Camera *getCamera() { return camera; };

};

inline void createWorld(int gw,int gh,int argc, char *argv[]) {
	SmartBlocksWorld::createWorld(gw,gh,argc,argv);
}

inline void deleteWorld() {
	SmartBlocksWorld::deleteWorld();
}

inline SmartBlocksWorld* getWorld() { return(SmartBlocksWorld::getWorld()); }

} // SmartBlocks namespace


#endif /* SMARTBLOCKSWORLD_H_ */
