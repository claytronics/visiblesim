/*
 * blinkyBlockWorld.cpp
 *
 *  Created on: 23 mars 2013
 *      Author: dom
 */

#include <iostream>
#include <stdlib.h>
#include "blinkyBlocksWorld.h"
#include "blinkyBlocksBlock.h"

using namespace std;

namespace BlinkyBlocks {

BlinkyBlocksWorld::BlinkyBlocksWorld(int slx,int sly,int slz,int argc, char *argv[]):World(), ios() {
	cout << "\033[1;31mBlinkyBlocksWorld constructor\033[0m" << endl;
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
	acceptor =  new tcp::acceptor(ios, tcp::endpoint(tcp::v4(), 7800));
}

BlinkyBlocksWorld::~BlinkyBlocksWorld() {
	cout << "BlinkyBlocksWorld destructor" << endl;
	/*	block linked are deleted by world::~world() */
	delete [] gridPtrBlocks;
	delete objBlock;
	delete objBlockForPicking;
	delete camera;
}


void BlinkyBlocksWorld::createWorld(int slx,int sly,int slz,int argc, char *argv[]) {
	world = new BlinkyBlocksWorld(slx,sly,slz,argc,argv);
}

void BlinkyBlocksWorld::deleteWorld() {
	delete((BlinkyBlocksWorld*)world);
}

void BlinkyBlocksWorld::addBlock(int blockId, BlinkyBlocksBlockCode *(*blinkyBlockCodeBuildingFunction)(BlinkyBlocksBlock*),const Vecteur &pos,const Vecteur &col) {

	if (blockId==-1) {// rechercher un blockID valide
		vector <GlBlock*>::const_iterator ci = tabGlBlocks.begin();
		while (ci!=tabGlBlocks.end()) {
			if ((*ci)->blockId>blockId) blockId=(*ci)->blockId;
			ci++;
		}
		blockId++;
	}

	// Start the VM
	pid_t VMPid = 0;
	char* cmd[] = {(char*)"VMEmulator", (char*)"-f", (char*)"program.meld", NULL };
	VMPid = fork();	
	if(VMPid < 0) {cerr << "Error when starting the VM" << endl;}
    	if(VMPid == 0) {execv("VMEmulator", const_cast<char**>(cmd));}

	// Wait for an incoming connection	
	boost::shared_ptr<tcp::socket> socket(new tcp::socket(ios));	
	acceptor->accept(*(socket.get()));
	cout << "VM "<< blockId << " connected" << endl;

	BlinkyBlocksBlock *blinkyBlock = new BlinkyBlocksBlock(blockId, socket, VMPid, blinkyBlockCodeBuildingFunction);
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
		cerr << "ERROR : BLOCK #" << blockId << " out of the grid !!!!!" << endl;
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
						(ptrBlock)->getInterface(Top)->connect(getGridPtr(ix,iy,iz+1)->getInterface(Bottom));
						cout << "connection #" << (ptrBlock)->blockId << " to #" << getGridPtr(ix,iy,iz+1)->blockId << endl;
					} else {
						(ptrBlock)->getInterface(Top)->connect(NULL);
					}
					if (iy<gridSize[1]-1 && getGridPtr(ix,iy+1,iz)) {
						(ptrBlock)->getInterface(Right)->connect(getGridPtr(ix,iy+1,iz)->getInterface(Left));
						cout << "connection #" << (ptrBlock)->blockId << " to #" << getGridPtr(ix,iy+1,iz)->blockId << endl;
					} else {
						(ptrBlock)->getInterface(Right)->connect(NULL);
					}
					if (ix<gridSize[0]-1 && getGridPtr(ix+1,iy,iz)) {
						(ptrBlock)->getInterface(Front)->connect(getGridPtr(ix+1,iy,iz)->getInterface(Back));
						cout << "connection #" << (ptrBlock)->blockId << " to #" << getGridPtr(ix+1,iy,iz)->blockId << endl;
					} else {
						(ptrBlock)->getInterface(Front)->connect(NULL);
					}
					if (iy>0 && getGridPtr(ix,iy-1,iz)) {
						(ptrBlock)->getInterface(Left)->connect(getGridPtr(ix,iy-1,iz)->getInterface(Right));
						cout << "connection #" << (ptrBlock)->blockId << " to #" << getGridPtr(ix,iy-1,iz)->blockId << endl;
					} else {
						(ptrBlock)->getInterface(Left)->connect(NULL);
					}
					if (iz>0 && getGridPtr(ix,iy,iz-1)) {
						(ptrBlock)->getInterface(Bottom)->connect(getGridPtr(ix,iy,iz-1)->getInterface(Top));
						cout << "connection #" << (ptrBlock)->blockId << " to #" << getGridPtr(ix,iy,iz-1)->blockId << endl;
					} else {
						(ptrBlock)->getInterface(Bottom)->connect(NULL);
					}
					if (ix>0 && getGridPtr(ix-1,iy,iz)) {
						(ptrBlock)->getInterface(Back)->connect(getGridPtr(ix-1,iy,iz)->getInterface(Front));
						cout << "connection #" << (ptrBlock)->blockId << " to #" << getGridPtr(ix-1,iy,iz)->blockId << endl;
					} else {
						(ptrBlock)->getInterface(Back)->connect(NULL);
					}
				}
			}

		}
	}
}

void BlinkyBlocksWorld::deleteBlock(BlinkyBlocksBlock *bb) {
	// cut links between bb and others
	for(int i=0; i<6; i++) {
		P2PNetworkInterface *bbi = bb->getInterface(NeighborDirection(i));
		if (bbi->connectedInterface) {
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
	buildingBlocksMap.erase(bb->blockId);

	// remove event from the list
	getScheduler()->removeEventsToBlock(bb);
	//getScheduler()->schedule(new CodeStartEvent(getScheduler()->now(), blinkyBlock));

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
	delete bb;

	linkBlocks();
}


void BlinkyBlocksWorld::glDraw() {
	static const GLfloat white[]={1.0,1.0,1.0,1.0},
			gray[]={0.6,0.6,0.6,1.0};

	glPushMatrix();
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

void BlinkyBlocksWorld::createPopupMenu(int ix,int iy) {
	if (!GlutContext::popupMenu) {
		GlutContext::popupMenu = new GlutPopupMenuWindow(NULL,0,0,200,75);
		GlutContext::popupMenu->addButton(1,"../../simulatorCore/blinkyBlocksTextures/menu_add.tga");
		GlutContext::popupMenu->addButton(2,"../../simulatorCore/blinkyBlocksTextures/menu_del.tga");
	}
	GlutContext::popupMenu->setCenterPosition(ix,GlutContext::screenHeight-iy);
	GlutContext::popupMenu->show(true);
}

void BlinkyBlocksWorld::menuChoice(int n) {
	switch (n) {
		case 1 : {
			BlinkyBlocksBlock *bb = (BlinkyBlocksBlock *)getBlockById(tabGlBlocks[numSelectedBlock]->blockId);
			cout << "ADD block link to : " << bb->blockId << "     num Face : " << numSelectedFace << endl;
			Vecteur pos=bb->position;
			switch (numSelectedFace) {
				case Left :
					pos.pt[0]--;
					break;
				case Right :
					pos.pt[0]++;
				break;
				case Front :
					pos.pt[1]--;
					break;
				case Back :
					pos.pt[1]++;
				break;
				case Bottom :
					pos.pt[2]--;
					break;
				case Top :
					pos.pt[2]++;
				break;
			}
			cout << "pos = " << pos << endl;
			addBlock(-1, bb->buildNewBlockCode,pos,bb->color);
			linkBlocks();
		} break;
		case 2 : {
			cout << "DEL num block : " << tabGlBlocks[numSelectedBlock]->blockId << endl;
			BlinkyBlocksBlock *bb = (BlinkyBlocksBlock *)getBlockById(tabGlBlocks[numSelectedBlock]->blockId);
			deleteBlock(bb);
		} break;
	}
}

void BlinkyBlocksWorld::setSelectedFace(int n) {
	numSelectedBlock=n/6;
	string name = objBlockForPicking->getObjMtlName(n%6);
    if (name=="face_top") numSelectedFace=Top;
    else if (name=="face_bottom") numSelectedFace=Bottom;
    else if (name=="face_right") numSelectedFace=Right;
    else if (name=="face_left") numSelectedFace=Left;
    else if (name=="face_front") numSelectedFace=Front;
    else if (name=="face_back") numSelectedFace=Back;
}
} // BlinkyBlock namespace
