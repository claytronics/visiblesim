/*
 * blinkyBlockWorld.cpp
 *
 *  Created on: 23 mars 2013
 *      Author: dom
 */

#include <iostream>
#include <string> 
#include <stdlib.h>
#include "blinkyBlocksWorld.h"
#include "blinkyBlocksBlock.h"
#include "blinkyBlocksEvents.h"
#include "trace.h"
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>

using namespace std;

namespace BlinkyBlocks {

BlinkyBlocksWorld::BlinkyBlocksWorld(int slx,int sly,int slz, int argc, char *argv[]):World() {
	OUTPUT << "\033[1;31mBlinkyBlocksWorld constructor\033[0m" << endl;
	gridSize[0]=slx;
	gridSize[1]=sly;
	gridSize[2]=slz;
	gridPtrBlocks = new BlinkyBlocksBlock*[slx*sly*slz];

	// initialise grid of blocks
	int i=slx*sly*slz;
	BlinkyBlocksBlock **ptr = gridPtrBlocks;
	while (i--) {
		*ptr=NULL;
		ptr++;
	}

	GlutContext::init(argc,argv);
	idTextureWall=0;
	blockSize[0]=39.0;
	blockSize[1]=39.0;
	blockSize[2]=40.0;
	objBlock = new ObjLoader::ObjLoader("../../simulatorCore/blinkyBlocksTextures","blinkyBlockSimple.obj");
	objBlockForPicking = new ObjLoader::ObjLoader("../../simulatorCore/blinkyBlocksTextures","blinkyBlockPicking.obj");
	objRepere = new ObjLoader::ObjLoader("../../simulatorCore/smartBlocksTextures","repere25.obj");
	camera = new Camera(-M_PI/2.0,M_PI/3.0,750.0);
	camera->setLightParameters(Vecteur(0,0,0),45.0,80.0,800.0,45.0,10.0,1500.0);
	camera->setTarget(Vecteur(0,0,1.0));

	menuId=0;
	numSelectedFace=0;
	numSelectedBlock=0;
}

BlinkyBlocksWorld::~BlinkyBlocksWorld() {
	OUTPUT << "BlinkyBlocksWorld destructor" << endl;
	/*	block linked are deleted by world::~world() */
	delete [] gridPtrBlocks;
	delete objBlock;
	delete objBlockForPicking;
	delete objRepere;
	delete camera;
	/* free Scenario Events */
	vector<ScenarioEvent*>::const_iterator it=tabEvents.begin();
	while (it!=tabEvents.end()) {
		delete (*it);
		it++;
	}
	tabEvents.clear();
}


void BlinkyBlocksWorld::createWorld(int slx,int sly,int slz, int argc, char *argv[]) {
	world = new BlinkyBlocksWorld(slx,sly,slz,argc,argv);
}

void BlinkyBlocksWorld::deleteWorld() {
	delete((BlinkyBlocksWorld*)world);
}

void BlinkyBlocksWorld::addBlock(int blockId, BlinkyBlocksBlockCode *(*blinkyBlockCodeBuildingFunction)(BlinkyBlocksBlock*),const Vecteur &pos,const Vecteur &col) {

	if (blockId == -1) {		
		map<int, BaseSimulator::BuildingBlock*>::iterator it;
			for(it = buildingBlocksMap.begin(); 
					it != buildingBlocksMap.end(); it++) {
				BlinkyBlocksBlock* bb = (BlinkyBlocksBlock*) it->second;
				if (it->second->blockId > blockId) blockId = bb->blockId;
			}
		blockId++;
	}
	BlinkyBlocksBlock *blinkyBlock = new BlinkyBlocksBlock(blockId, blinkyBlockCodeBuildingFunction);
	buildingBlocksMap.insert(std::pair<int,BaseSimulator::BuildingBlock*>(blinkyBlock->blockId, (BaseSimulator::BuildingBlock*)blinkyBlock));

	getScheduler()->schedule(new CodeStartEvent(getScheduler()->now(), blinkyBlock));

	BlinkyBlocksGlBlock *glBlock = new BlinkyBlocksGlBlock(blockId);
	tabGlBlocks.push_back(glBlock);
	blinkyBlock->setGlBlock(glBlock);
	blinkyBlock->setPosition(pos);
	blinkyBlock->setColor(col);

	int ix,iy,iz;
	ix = int(blinkyBlock->position.pt[0]);
	iy = int(blinkyBlock->position.pt[1]);
	iz = int(blinkyBlock->position.pt[2]);
	if (ix>=0 && ix<gridSize[0] && 
		iy>=0 && iy<gridSize[1] && 
		iz>=0 && iz<gridSize[2]) {
		setGridPtr(ix,iy,iz,blinkyBlock);
	} else {
		ERRPUT << "ERROR : BLOCK #" << blockId << " out of the grid !!!!!" << endl;
		exit(1);
	}
}

void BlinkyBlocksWorld::linkBlocks() {
	int ix,iy,iz;
	BlinkyBlocksBlock *ptrBlock;
	for (iz=0; iz<gridSize[2]; iz++) {
		for (iy=0; iy<gridSize[1]; iy++) {
			for(ix=0; ix<gridSize[0]; ix++) {
				ptrBlock = getGridPtr(ix,iy,iz);
				if (ptrBlock) {
					if (iz<gridSize[2]-1 && getGridPtr(ix,iy,iz+1)) {
						(ptrBlock)->getInterface(NeighborDirection::Top)->connect(getGridPtr(ix,iy,iz+1)->getInterface(NeighborDirection::Bottom));
						OUTPUT << "connection #" << (ptrBlock)->blockId << " to #" << getGridPtr(ix,iy,iz+1)->blockId << endl;
					} else {
						(ptrBlock)->getInterface(NeighborDirection::Top)->connect(NULL);
					}
					if (iy<gridSize[1]-1 && getGridPtr(ix,iy+1,iz)) {
						(ptrBlock)->getInterface(NeighborDirection::Right)->connect(getGridPtr(ix,iy+1,iz)->getInterface(NeighborDirection::Left));
						OUTPUT << "connection #" << (ptrBlock)->blockId << " to #" << getGridPtr(ix,iy+1,iz)->blockId << endl;
					} else {
						(ptrBlock)->getInterface(NeighborDirection::Right)->connect(NULL);
					}
					if (ix<gridSize[0]-1 && getGridPtr(ix+1,iy,iz)) {
						(ptrBlock)->getInterface(NeighborDirection::Front)->connect(getGridPtr(ix+1,iy,iz)->getInterface(NeighborDirection::Back));
						OUTPUT << "connection #" << (ptrBlock)->blockId << " to #" << getGridPtr(ix+1,iy,iz)->blockId << endl;
					} else {
						(ptrBlock)->getInterface(NeighborDirection::Front)->connect(NULL);
					}
					if (iy>0 && getGridPtr(ix,iy-1,iz)) {
						(ptrBlock)->getInterface(NeighborDirection::Left)->connect(getGridPtr(ix,iy-1,iz)->getInterface(NeighborDirection::Right));
						OUTPUT << "connection #" << (ptrBlock)->blockId << " to #" << getGridPtr(ix,iy-1,iz)->blockId << endl;
					} else {
						(ptrBlock)->getInterface(NeighborDirection::Left)->connect(NULL);
					}
					if (iz>0 && getGridPtr(ix,iy,iz-1)) {
						(ptrBlock)->getInterface(NeighborDirection::Bottom)->connect(getGridPtr(ix,iy,iz-1)->getInterface(NeighborDirection::Top));
						OUTPUT << "connection #" << (ptrBlock)->blockId << " to #" << getGridPtr(ix,iy,iz-1)->blockId << endl;
					} else {
						(ptrBlock)->getInterface(NeighborDirection::Bottom)->connect(NULL);
					}
					if (ix>0 && getGridPtr(ix-1,iy,iz)) {
						(ptrBlock)->getInterface(NeighborDirection::Back)->connect(getGridPtr(ix-1,iy,iz)->getInterface(NeighborDirection::Front));
						OUTPUT << "connection #" << (ptrBlock)->blockId << " to #" << getGridPtr(ix-1,iy,iz)->blockId << endl;
					} else {
						(ptrBlock)->getInterface(NeighborDirection::Back)->connect(NULL);
					}
				}
			}

		}
	}
}

void BlinkyBlocksWorld::deleteBlock(BlinkyBlocksBlock *bb) {
	if (bb->getState() >= BlinkyBlocksBlock::ALIVE ) {
		// cut links between bb and others
		for(int i=0; i<6; i++) {
			P2PNetworkInterface *bbi = bb->getInterface(NeighborDirection::Direction(i));
			if (bbi->connectedInterface) {
				//bb->removeNeighbor(bbi); //Useless
				bbi->connectedInterface->hostBlock->removeNeighbor(bbi->connectedInterface);
				bbi->connectedInterface->connectedInterface=NULL;
				bbi->connectedInterface=NULL;
			}
		}
		// free grid cell
		int ix,iy,iz;
		ix = int(bb->position.pt[0]);
		iy = int(bb->position.pt[1]);
		iz = int(bb->position.pt[2]);
		setGridPtr(ix,iy,iz,NULL);
		
		// remove the block from the lists
		//buildingBlocksMap.erase(bb->blockId);
		// remove event from the list
		//getScheduler()->removeEventsToBlock(bb);
		
		bb->stop(getScheduler()->now(), BlinkyBlocksBlock::REMOVED); // schedule stop event, set REMOVED state
		linkBlocks();
	}   
	if (selectedBlock == bb->ptrGlBlock) {
      selectedBlock = NULL;
      GlutContext::mainWindow->select(NULL);
   }
	// remove the associated glBlock
	std::vector<GlBlock*>::iterator cit=tabGlBlocks.begin();
	if (*cit==bb->ptrGlBlock) tabGlBlocks.erase(cit);
	else {
		while (cit!=tabGlBlocks.end() && (*cit)!=bb->ptrGlBlock) {
			cit++;
		}
		if (*cit==bb->ptrGlBlock) tabGlBlocks.erase(cit);
	}
	delete bb->ptrGlBlock;
}


void BlinkyBlocksWorld::glDraw() {
	static const GLfloat white[]={1.0,1.0,1.0,1.0},
			gray[]={0.6,0.6,0.6,1.0};


	glPushMatrix();
	glTranslatef(0.5*blockSize[0],0.5*blockSize[1],0);
	glDisable(GL_TEXTURE_2D);
	vector <GlBlock*>::iterator ic=tabGlBlocks.begin();
	lock();
	while (ic!=tabGlBlocks.end()) {
		((BlinkyBlocksGlBlock*)(*ic))->glDraw(objBlock);
	   	ic++;
	}
	unlock();

	glPopMatrix();
	glMaterialfv(GL_FRONT,GL_AMBIENT,gray);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,white);
	glMaterialfv(GL_FRONT,GL_SPECULAR,gray);
	glMaterialf(GL_FRONT,GL_SHININESS,40.0);
	glPushMatrix();
		enableTexture(true);
		glBindTexture(GL_TEXTURE_2D,idTextureWall);
		glScalef(gridSize[0]*blockSize[0],gridSize[1]*blockSize[1],gridSize[2]*blockSize[2]);
		glBegin(GL_QUADS);
		// bottom
			glNormal3f(0,0,1.0f);
			glTexCoord2f(0,0);
			glVertex3f(0.0f,0.0f,0.0f);
			glTexCoord2f(gridSize[0],0);
			glVertex3f(1.0f,0.0f,0.0f);
			glTexCoord2f(gridSize[0],gridSize[1]);
			glVertex3f(1.0,1.0,0.0f);
			glTexCoord2f(0,gridSize[1]);
			glVertex3f(0.0,1.0,0.0f);
		// top
			glNormal3f(0,0,-1.0f);
			glTexCoord2f(0,0);
			glVertex3f(0.0f,0.0f,1.0f);
			glTexCoord2f(0,gridSize[1]);
			glVertex3f(0.0,1.0,1.0f);
			glTexCoord2f(gridSize[0],gridSize[1]);
			glVertex3f(1.0,1.0,1.0f);
			glTexCoord2f(gridSize[0],0);
			glVertex3f(1.0f,0.0f,1.0f);
		// left
			glNormal3f(1.0,0,0);
			glTexCoord2f(0,0);
			glVertex3f(0.0f,0.0f,0.0f);
			glTexCoord2f(gridSize[1],0);
			glVertex3f(0.0f,1.0f,0.0f);
			glTexCoord2f(gridSize[1],gridSize[2]);
			glVertex3f(0.0,1.0,1.0f);
			glTexCoord2f(0,gridSize[2]);
			glVertex3f(0.0,0.0,1.0f);
		// right
			glNormal3f(-1.0,0,0);
			glTexCoord2f(0,0);
			glVertex3f(1.0f,0.0f,0.0f);
			glTexCoord2f(0,gridSize[2]);
			glVertex3f(1.0,0.0,1.0f);
			glTexCoord2f(gridSize[1],gridSize[2]);
			glVertex3f(1.0,1.0,1.0f);
			glTexCoord2f(gridSize[1],0);
			glVertex3f(1.0f,1.0f,0.0f);
		// back
			glNormal3f(0,-1.0,0);
			glTexCoord2f(0,0);
			glVertex3f(0.0f,1.0f,0.0f);
			glTexCoord2f(gridSize[0],0);
			glVertex3f(1.0f,1.0f,0.0f);
			glTexCoord2f(gridSize[0],gridSize[2]);
			glVertex3f(1.0f,1.0,1.0f);
			glTexCoord2f(0,gridSize[2]);
			glVertex3f(0.0,1.0,1.0f);
		// front
			glNormal3f(0,1.0,0);
			glTexCoord2f(0,0);
			glVertex3f(0.0f,0.0f,0.0f);
			glTexCoord2f(0,gridSize[2]);
			glVertex3f(0.0,0.0,1.0f);
			glTexCoord2f(gridSize[0],gridSize[2]);
			glVertex3f(1.0f,0.0,1.0f);
			glTexCoord2f(gridSize[0],0);
			glVertex3f(1.0f,0.0f,0.0f);
		glEnd();
	glPopMatrix();
	// draw the axes
	objRepere->glDraw();
}

void BlinkyBlocksWorld::glDrawId() {
	glPushMatrix();
	glTranslatef(0.5*blockSize[0],0.5*blockSize[1],0);
	glDisable(GL_TEXTURE_2D);
	vector <GlBlock*>::iterator ic=tabGlBlocks.begin();
	int n=1;
	lock();
	while (ic!=tabGlBlocks.end()) {
		((BlinkyBlocksGlBlock*)(*ic))->glDrawId(objBlock,n);
	   	ic++;
	}
	unlock();
	glPopMatrix();
}

void BlinkyBlocksWorld::glDrawIdByMaterial() {
	glPushMatrix();
	glTranslatef(0.5*blockSize[0],0.5*blockSize[1],0);

	glDisable(GL_TEXTURE_2D);
	vector <GlBlock*>::iterator ic=tabGlBlocks.begin();
	int n=1;
	lock();
	while (ic!=tabGlBlocks.end()) {
		((BlinkyBlocksGlBlock*)(*ic))->glDrawIdByMaterial(objBlockForPicking,n);
	   	ic++;
	}
	unlock();
	glPopMatrix();
}


void BlinkyBlocksWorld::loadTextures(const string &str) {
	string path = str+"/texture_plane.tga";
	int lx,ly;
	idTextureWall = GlutWindow::loadTexture(path.c_str(),lx,ly);
}

void BlinkyBlocksWorld::updateGlData(BlinkyBlocksBlock*blc) {
	BlinkyBlocksGlBlock *glblc = blc->getGlBlock();
	if (glblc) {
		lock();
		Vecteur pos(blockSize[0]*blc->position[0],blockSize[1]*blc->position[1],blockSize[2]*blc->position[2]);
		glblc->setPosition(pos);
		glblc->setColor(blc->color);
		unlock();
	}
}

void BlinkyBlocksWorld::createPopupMenu(int ix, int iy) {
	if (!GlutContext::popupMenu) {
		GlutContext::popupMenu = new GlutPopupMenuWindow(NULL,0,0,200,180);
		GlutContext::popupMenu->addButton(1,"../../simulatorCore/blinkyBlocksTextures/menu_add.tga");
		GlutContext::popupMenu->addButton(2,"../../simulatorCore/blinkyBlocksTextures/menu_del.tga");
		GlutContext::popupMenu->addButton(3,"../../simulatorCore/blinkyBlocksTextures/menu_stop.tga");
		GlutContext::popupMenu->addButton(4,"../../simulatorCore/blinkyBlocksTextures/menu_save.tga");
		GlutContext::popupMenu->addButton(5,"../../simulatorCore/blinkyBlocksTextures/menu_cancel.tga");
	}
	if (iy<GlutContext::popupMenu->h) iy=GlutContext::popupMenu->h;
	// verify if add is possible for this face
	BlinkyBlocksBlock *bb = (BlinkyBlocksBlock *)getBlockById(tabGlBlocks[numSelectedBlock]->blockId);
	bool valid=true;
	switch (numSelectedFace) {
		case NeighborDirection::Left :
			valid=(bb->position[0]>0 && getGridPtr(int(bb->position[0])-1,int(bb->position[1]),int(bb->position[2]))==NULL);
			break;
		case NeighborDirection::Right :
			valid=(bb->position[0]<gridSize[0]-1 && getGridPtr(int(bb->position[0])+1,int(bb->position[1]),int(bb->position[2]))==NULL);
		break;
		case NeighborDirection::Front :
			valid=(bb->position[1]>0 && getGridPtr(int(bb->position[0]),int(bb->position[1])-1,int(bb->position[2]))==NULL);
			break;
		case NeighborDirection::Back :
			valid=(bb->position[1]<gridSize[1]-1 && getGridPtr(int(bb->position[0]),int(bb->position[1])+1,int(bb->position[2]))==NULL);
		break;
		case NeighborDirection::Bottom :
			valid=(bb->position[2]>0 && getGridPtr(int(bb->position[0]),int(bb->position[1]),int(bb->position[2])-1)==NULL);
			break;
		case NeighborDirection::Top :
			valid=(bb->position[2]<gridSize[2]-1 && getGridPtr(int(bb->position[0]),int(bb->position[1]),int(bb->position[2])+1)==NULL);
		break;
	}
	GlutContext::popupMenu->activate(1,valid);
	GlutContext::popupMenu->setCenterPosition(ix,GlutContext::screenHeight-iy);
	GlutContext::popupMenu->show(true);
}

void BlinkyBlocksWorld::menuChoice(int n) {
	switch (n) {
		case 1 : {
			BlinkyBlocksBlock *bb = (BlinkyBlocksBlock *)getBlockById(tabGlBlocks[numSelectedBlock]->blockId);
			OUTPUT << "ADD block link to : " << bb->blockId << "     num Face : " << numSelectedFace << endl;
			Vecteur pos=bb->position;
			switch (numSelectedFace) {
				case NeighborDirection::Left :
					pos.pt[0]--;
					break;
				case NeighborDirection::Right :
					pos.pt[0]++;
				break;
				case NeighborDirection::Front :
					pos.pt[1]--;
					break;
				case NeighborDirection::Back :
					pos.pt[1]++;
				break;
				case NeighborDirection::Bottom :
					pos.pt[2]--;
					break;
				case NeighborDirection::Top :
					pos.pt[2]++;
				break;
			}
			addBlock(-1, bb->buildNewBlockCode,pos,bb->color);
			linkBlocks();
		} break;
		case 2 : {
			OUTPUT << "DEL num block : " << tabGlBlocks[numSelectedBlock]->blockId << endl;
         BlinkyBlocksBlock *bb = (BlinkyBlocksBlock *)getBlockById(tabGlBlocks[numSelectedBlock]->blockId);
			deleteBlock(bb);
		} break;
		case 3 : {
			BlinkyBlocksBlock *bb = (BlinkyBlocksBlock *)getBlockById(tabGlBlocks[numSelectedBlock]->blockId);
			stopBlock(getScheduler()->now(), bb->blockId);
		} break;
	}
}

void BlinkyBlocksWorld::setSelectedFace(int n) {
	numSelectedBlock=n/6;
	string name = objBlockForPicking->getObjMtlName(n%6);
    if (name=="face_top") numSelectedFace=NeighborDirection::Top;
    else if (name=="face_bottom") numSelectedFace=NeighborDirection::Bottom;
    else if (name=="face_right") numSelectedFace=NeighborDirection::Right;
    else if (name=="face_left") numSelectedFace=NeighborDirection::Left;
    else if (name=="face_front") numSelectedFace=NeighborDirection::Front;
    else if (name=="face_back") numSelectedFace=NeighborDirection::Back;
}

	void BlinkyBlocksWorld::tapBlock(uint64_t date, int bId) {
		BlinkyBlocksBlock *bb = (BlinkyBlocksBlock*)getBlockById(bId);
		bb->tap(date);
	}
	
	void BlinkyBlocksWorld::accelBlock(uint64_t date, int bId, int x, int y, int z) {
		BlinkyBlocksBlock *bb = (BlinkyBlocksBlock*)getBlockById(bId);
		bb->accel(date, x,y,z);
	}
	
	void BlinkyBlocksWorld::shakeBlock(uint64_t date, int bId, int f) {
		BlinkyBlocksBlock *bb = (BlinkyBlocksBlock*)getBlockById(bId);
		bb->shake(date, f);	
	}
	
	int BlinkyBlocksWorld::broadcastDebugCommand(DebbuggerVMCommand &c) {
		map<int, BaseSimulator::BuildingBlock*>::iterator it;
		int aliveBlocks = 0;
		for(it = buildingBlocksMap.begin(); 
				it != buildingBlocksMap.end(); it++) {
			BlinkyBlocksBlock* bb = (BlinkyBlocksBlock*) it->second;
			BlinkyBlocksBlockCode* bbc = (BlinkyBlocksBlockCode*) bb->blockCode;
			/* Send id & set deterministic mode if necessary */
			bbc->init();
			aliveBlocks += bb->sendCommand(c);
		}
		return aliveBlocks;
	}
	
	int BlinkyBlocksWorld::sendCommand(int id, VMCommand &c) {
		BlinkyBlocksBlock *bb = (BlinkyBlocksBlock*)getBlockById(id);
      BlinkyBlocksBlockCode* bbc = (BlinkyBlocksBlockCode*) bb->blockCode;
		bbc->init();
      return bb->sendCommand(c);
	}

	
	void BlinkyBlocksWorld::stopBlock(uint64_t date, int bId) {
		if (bId < 0) {
			// Delete the block	without deleting the links
			map<int, BaseSimulator::BuildingBlock*>::iterator it;
			for(it = buildingBlocksMap.begin(); 
					it != buildingBlocksMap.end(); it++) {
				BlinkyBlocksBlock* bb = (BlinkyBlocksBlock*) it->second;
				if (bb->getState() >= BlinkyBlocksBlock::ALIVE ) 
					bb->stop(date, BlinkyBlocksBlock::STOPPED);
			}
		} else {
			// Delete all the links and then the block		
			BlinkyBlocksBlock *bb = (BlinkyBlocksBlock *)getBlockById(bId);
			if(bb->getState() >= BlinkyBlocksBlock::ALIVE) {
				// cut links between bb and others
				for(int i=0; i<6; i++) {
					P2PNetworkInterface *bbi = bb->getInterface(NeighborDirection::Direction(i));
					if (bbi->connectedInterface) {
						//bb->removeNeighbor(bbi); //Useless
						bbi->connectedInterface->hostBlock->removeNeighbor(bbi->connectedInterface);
						bbi->connectedInterface->connectedInterface=NULL;
						bbi->connectedInterface=NULL;
					}
				}
				// free grid cell
				int ix,iy,iz;
				ix = int(bb->position.pt[0]);
				iy = int(bb->position.pt[1]);
				iz = int(bb->position.pt[2]);
				setGridPtr(ix,iy,iz,NULL);
				bb->stop(date, BlinkyBlocksBlock::STOPPED); // schedule stop event, set STOPPED state
				linkBlocks();
			}
		}
	}
	
	void BlinkyBlocksWorld::createHelpWindow() {
		if (GlutContext::helpWindow) 
			delete GlutContext::helpWindow;
		GlutContext::helpWindow = new GlutHelpWindow(NULL,10,40,540,500,"../../simulatorCore/blinkyBlocksHelp.txt");
	}
	
	bool BlinkyBlocksWorld::dateHasBeenReachedByAll(uint64_t date) {
		static uint64_t minReallyReached = 0;
		uint64_t min, min2;
		int alive = 0, hasNoWork = 0;
		if (date < minReallyReached) {
			return true;
		}
		map<int, BaseSimulator::BuildingBlock*>::iterator it;
		for(it = buildingBlocksMap.begin(); 
				it != buildingBlocksMap.end(); it++) {
			BlinkyBlocksBlock* bb = (BlinkyBlocksBlock*) it->second;			
			BlinkyBlocksBlockCode *bc = (BlinkyBlocksBlockCode*) bb->blockCode;
			if (bb->getState() < BlinkyBlocksBlock::ALIVE) {
				continue;
			}
			alive++;
			if (!bc->hasWork || bc->polling) {
				hasNoWork++;
				if (alive == 1) {
					min2 = bc->currentLocalDate;
				} else if (bc->currentLocalDate < min2) {
					min2 = bc->currentLocalDate;
				}
			} else {
				if ((alive - 1) == hasNoWork) {
					min = bc->currentLocalDate;
				} else if (bc->currentLocalDate < min) {
					min = bc->currentLocalDate;
				}
				if (min < min2) {
					min2 = min;
				}
			}
		}
		if (alive==hasNoWork) {
			return true;
		}
		minReallyReached = min2;
		return (date < min);
	}
	
	void BlinkyBlocksWorld::killAllVMs() {
		map<int, BaseSimulator::BuildingBlock*>::iterator it;
		for(it = buildingBlocksMap.begin(); 
				it != buildingBlocksMap.end(); it++) {	
			BlinkyBlocksBlock* bb = (BlinkyBlocksBlock*) it->second;
			bb->killVM();
		}
	}
   
   void BlinkyBlocksWorld::closeAllSockets() {
		map<int, BaseSimulator::BuildingBlock*>::iterator it;
		for(it = buildingBlocksMap.begin(); 
				it != buildingBlocksMap.end(); it++) {	
			BlinkyBlocksBlock* bb = (BlinkyBlocksBlock*) it->second;
         if(bb->vm != NULL) {
            bb->vm->socket->close();
            bb->vm->socket.reset();
         }
		}
	}
   	
	bool BlinkyBlocksWorld::equilibrium() {
		map<int, BaseSimulator::BuildingBlock*>::iterator it;
		for(it = buildingBlocksMap.begin(); 
				it != buildingBlocksMap.end(); it++) {
			BlinkyBlocksBlock* bb = (BlinkyBlocksBlock*) it->second;			
			BlinkyBlocksBlockCode *bc = (BlinkyBlocksBlockCode*) bb->blockCode;
			if (bb->getState() < BlinkyBlocksBlock::ALIVE) {
				continue;
			}
			if (bc->hasWork) {
				return false;
			}
		}
		return true;
	}
   
   void BlinkyBlocksWorld::dump() {
      map<int, BaseSimulator::BuildingBlock*>::iterator it;
      cout << "World:" << endl;
		for(it = buildingBlocksMap.begin(); 
				it != buildingBlocksMap.end(); it++) {
			BlinkyBlocksBlock* bb = (BlinkyBlocksBlock*) it->second;
         cout << *bb << endl;
      }
   }

} // BlinkyBlock namespace
