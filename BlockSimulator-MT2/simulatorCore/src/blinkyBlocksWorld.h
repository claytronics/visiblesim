/*
 * blinkyBlocksWorld.h
 *
 *  Created on: 23 mars 2013
 *      Author: dom
 */

#ifndef BLINKYBLOCKSWORLD_H_
#define BLINKYBLOCKSWORLD_H_

#include "openglViewer.h"
#include "world.h"
#include "vecteur.h"
#include "blinkyBlocksBlock.h"
#include "objLoader.h"
#include <boost/asio.hpp>

namespace BlinkyBlocks {

class BlinkyBlocksWorld : BaseSimulator::World {
protected:
	int gridSize[3];
	BlinkyBlocksBlock **gridPtrBlocks;
	GLfloat blockSize[3];
	Camera *camera;
	ObjLoader::ObjLoader *objBlock,*objBlockForPicking,*objRepere;
	GLuint idTextureWall;
	GLushort numSelectedFace;
	GLuint numSelectedBlock;
	GLint menuId;
	int port;
	boost::asio::io_service ios;
	tcp::acceptor *acceptor;
	string vmPath;
	string programPath;

	BlinkyBlocksWorld(int slx,int sly,int slz,int p, int argc, char *argv[]);
	virtual ~BlinkyBlocksWorld();
	inline BlinkyBlocksBlock* getGridPtr(int ix,int iy,int iz) { return gridPtrBlocks[ix+(iy+iz*gridSize[1])*gridSize[0]]; };
	inline void setGridPtr(int ix,int iy,int iz,BlinkyBlocksBlock *ptr) { gridPtrBlocks[ix+(iy+iz*gridSize[1])*gridSize[0]]=ptr; };
public:
	static void createWorld(int slx,int sly,int slz, int p, int argc, char *argv[]);
	static void deleteWorld();
	static BlinkyBlocksWorld* getWorld() {
		assert(world != NULL);
		return((BlinkyBlocksWorld*)world);
	}
	boost::asio::io_service& getIos() { return ios; }
	void printInfo() {
		cout << "I'm a BlinkyBlocksWorld" << endl;
	}

	virtual void addBlock(int blockId, BlinkyBlocksBlockCode *(*blinkyBlockCodeBuildingFunction)(BlinkyBlocksBlock*), const Vecteur &pos, const Vecteur &col);
	void deleteBlock(BlinkyBlocksBlock *bb);
	inline void setBlocksSize(float *siz) { blockSize[0] = siz[0]; blockSize[1] = siz[1]; blockSize[2] = siz[2]; };
	inline void setVmPath(string p) { vmPath = p; };
	inline void setProgramPath(string p) { programPath = p; };
	void linkBlocks();
	void loadTextures(const string &str);
	virtual void glDraw();
	virtual void glDrawId();
	virtual void glDrawIdByMaterial();
	virtual void updateGlData(BlinkyBlocksBlock*blc);
	virtual void createPopupMenu(int ix, int iy);
	inline virtual Camera *getCamera() { return camera; };
	virtual void setSelectedFace(int n);
	virtual void menuChoice(int n);
	/* Send a message to the VM associated to bId block depending on the handled event */
	void tapBlock(int bId);
	void accelBlock(int bId, int x, int y, int z);
	void shakeBlock(int bId, int f);
};

inline void createWorld(int slx,int sly,int slz, int p, int argc, char *argv[]) {
	BlinkyBlocksWorld::createWorld(slx,sly,slz, p, argc,argv);
}

inline void deleteWorld() {
	BlinkyBlocksWorld::deleteWorld();
}

inline BlinkyBlocksWorld* getWorld() { return(BlinkyBlocksWorld::getWorld()); }

} // BlinkyBlocks namespace

#endif /* BLINKYBLOCKSWORLD_H_ */
