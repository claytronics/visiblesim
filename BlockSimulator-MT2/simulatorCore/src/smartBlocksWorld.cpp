/*
 * smanrtBlockWorld.cpp
 *
 *  Created on: 12 avril 2013
 *      Author: ben
 */

#include <iostream>
#include <stdlib.h>
#include <string>
#include "smartBlocksWorld.h"
#include "smartBlocksScheduler.h"
#include "smartBlocksBlock.h"
#include "scheduler.h"

using namespace std;

namespace SmartBlocks {

SmartBlocksWorld::SmartBlocksWorld(int gw,int gh,int argc, char *argv[]):World() {
	cout << "\033[1;31mSmartBlocksWorld constructor\033[0m" << endl;
	gridWidth = gw;
	gridHeight = gh;
	tabPtrBlocks = new SmartBlocksBlock*[gw*gh];
	int i=gw*gh;
	SmartBlocksBlock **ptr = tabPtrBlocks;
	while (i--) {
		*ptr=NULL;
		ptr++;
	}

	GlutContext::init(argc,argv);
	idTextureFloor=0;
	blockSize[0]=25.0;
	blockSize[1]=25.0;
	blockSize[2]=11.0;
	objBlock = new ObjLoader::ObjLoader("../../simulatorCore/smartBlocksTextures","smartBlockSimple.obj");
	objRepere = new ObjLoader::ObjLoader("../../simulatorCore/smartBlocksTextures","repere25.obj");
	camera = new Camera(-M_PI/2.0,M_PI/3.0,750.0);
	camera->setLightParameters(Vecteur(0,0,0),45.0,80.0,800.0,45.0,10.0,1500.0);
	camera->setTarget(Vecteur(0,0,1.0));
}

SmartBlocksWorld::~SmartBlocksWorld() {
	cout << "\033[1;31mSmartBlocksWorld destructor" << endl;

/*	block linked are deleted by world::~world() */
	delete [] tabPtrBlocks;
	delete objBlock;
	delete objRepere;
	delete camera;
}

void SmartBlocksWorld::createWorld(int gw,int gh,int argc, char *argv[]) {
	world = new SmartBlocksWorld(gw,gh,argc,argv);
}

void SmartBlocksWorld::deleteWorld() {
	delete((SmartBlocksWorld*)world);
	world=NULL;
}

void SmartBlocksWorld::addBlock(int blockId, SmartBlocksBlockCode *(*smartBlockCodeBuildingFunction)(SmartBlocksBlock*),const Vecteur &pos,const Vecteur &col) {
	SmartBlocksBlock *smartBlock = new SmartBlocksBlock(blockId,smartBlockCodeBuildingFunction);
	buildingBlocksMap.insert(std::pair<int,BaseSimulator::BuildingBlock*>(smartBlock->blockId, (BaseSimulator::BuildingBlock*)smartBlock) );
	getScheduler()->schedule(new CodeStartEvent(getScheduler()->now(), smartBlock));

	SmartBlocksGlBlock *glBlock = new SmartBlocksGlBlock(blockId);
	tabGlBlocks.push_back(glBlock);
	smartBlock->setGlBlock(glBlock);
	smartBlock->setPosition(pos);
	smartBlock->setColor(col);

	int x,y;
	smartBlock->getGridPosition(x,y);
	if (x>=0 && x<gridWidth && y>=0 && y<gridHeight) {
		setGridPtr(x,y,smartBlock);
	} else {
		cerr << "ERROR : BLOCK #" << blockId << " out of the grid !!!!!" << endl;
		exit(1);
	}
}

void SmartBlocksWorld::linkBlocks() {
	int ix,iy;
	SmartBlocksBlock *ptrBlock;
	for (iy=0; iy<gridHeight; iy++) {
		for(ix=0; ix<gridWidth; ix++) {
			ptrBlock = getGridPtr(ix,iy);
			if (ptrBlock) {
				if (iy<gridHeight-1 && getGridPtr(ix,iy+1)) {
					(ptrBlock)->getInterface(North)->connect(getGridPtr(ix,iy+1)->getInterface(South));
					cout << "connection #" << (ptrBlock)->blockId << " to #" << getGridPtr(ix,iy+1)->blockId << endl;
				} else {
					(ptrBlock)->getInterface(North)->connect(NULL);
				}
				if (ix<gridWidth-1 && getGridPtr(ix+1,iy)) {
					cout << "id=" << getGridPtr(ix+1,iy)->blockId << endl;
					(ptrBlock)->getInterface(East)->connect(getGridPtr(ix+1,iy)->getInterface(West));
					cout << "connection #" << (ptrBlock)->blockId << " to #" << getGridPtr(ix+1,iy)->blockId << endl;
				} else {
					(ptrBlock)->getInterface(East)->connect(NULL);
				}
				if (iy>0 && getGridPtr(ix,iy-1)) {
					(ptrBlock)->getInterface(South)->connect(getGridPtr(ix,iy-1)->getInterface(North));
					cout << "connection #" << (ptrBlock)->blockId << " to #" << getGridPtr(ix,iy-1)->blockId << endl;
				} else {
					(ptrBlock)->getInterface(South)->connect(NULL);
				}
				if (ix>0 && getGridPtr(ix-1,iy)) {
					(ptrBlock)->getInterface(West)->connect(getGridPtr(ix-1,iy)->getInterface(East));
					cout << "connection #" << (ptrBlock)->blockId << " to #" << getGridPtr(ix-1,iy)->blockId << endl;
				} else {
					(ptrBlock)->getInterface(West)->connect(NULL);
				}
			}

		}
	}
}

void SmartBlocksWorld::glDraw() {
	static const GLfloat white[]={1.0,1.0,1.0,1.0},
			gray[]={0.2,0.2,0.2,1.0};


	glMaterialfv(GL_FRONT,GL_AMBIENT,gray);
	glMaterialfv(GL_FRONT,GL_DIFFUSE,white);
	glMaterialfv(GL_FRONT,GL_SPECULAR,gray);
	glMaterialf(GL_FRONT,GL_SHININESS,40.0);
	glPushMatrix();
		glEnable(GL_TEXTURE_2D);
		glBindTexture(GL_TEXTURE_2D,idTextureFloor);
		glNormal3f(0,0,1.0f);
		glScalef(gridWidth*blockSize[0],gridHeight*blockSize[1],1.0f);
		glBegin(GL_QUADS);
			glTexCoord2f(0,0);
			glVertex3f(0.0f,0.0f,0.0f);
			glTexCoord2f(gridWidth,0);
			glVertex3f(1.0f,0.0f,0.0f);
			glTexCoord2f(gridWidth,gridHeight);
			glVertex3f(1.0,1.0,0.0f);
			glTexCoord2f(0,gridHeight);
			glVertex3f(0.0,1.0,0.0f);
		glEnd();
	glPopMatrix();
	// draw the axes
	objRepere->glDraw();

	glPushMatrix();
	//glTranslatef(-gridWidth/2.0f*blockSize[0],-gridHeight/2.0f*blockSize[1],0);
	glDisable(GL_TEXTURE_2D);
	vector <GlBlock*>::iterator ic=tabGlBlocks.begin();
	lock();
	while (ic!=tabGlBlocks.end()) {
		((SmartBlocksGlBlock*)(*ic))->glDraw(objBlock);
		ic++;
	}
	unlock();

	/*// drawing the mobiles
	Physics::glDraw();
*/
	glPopMatrix();

}

void SmartBlocksWorld::glDrawId() {
	static const GLfloat white[]={1.0,1.0,1.0,1.0},
				gray[]={0.2,0.2,0.2,1.0};

	glPushMatrix();
	glDisable(GL_TEXTURE_2D);
	//glTranslatef(-gridWidth/2.0f*blockSize[0],-gridHeight/2.0f*blockSize[1],0);
	vector <GlBlock*>::iterator ic=tabGlBlocks.begin();
	int n=1;
	lock();
	while (ic!=tabGlBlocks.end()) {
		glLoadName(n++);
		((SmartBlocksGlBlock*)(*ic))->glDrawId(objBlock);
	   	ic++;
	}
	unlock();
	glPopMatrix();
}

void SmartBlocksWorld::loadTextures(const string &str) {
	string path = str+"/circuit.tga";
	idTextureFloor = GlutWindow::loadTexture(path.c_str());
}

void SmartBlocksWorld::updateGlData(SmartBlocksBlock*blc) {
	SmartBlocksGlBlock *glblc = blc->getGlBlock();
	if (glblc) {
		lock();

		Vecteur pos(blockSize[0]*blc->position[0],blockSize[1]*blc->position[1],0.0);
		glblc->setPosition(pos);
		glblc->setColor(blc->color);
		unlock();
	}
}


} // SmartBlocks namespace
