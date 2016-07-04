/*
 * robotBlockWorld.cpp
 *
 *  Created on: 12 janvier 2014
 *      Author: Benoît
 */

#include <iostream>
#include <string>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <signal.h>

#include "robotBlocksWorld.h"
#include "robotBlocksBlock.h"
#include "trace.h"
#include "configExporter.h"

using namespace std;

namespace RobotBlocks {

RobotBlocksWorld::RobotBlocksWorld(const Cell3DPosition &gridSize, const Vector3D &gridScale,
								   int argc, char *argv[]):World(argc, argv) {
	OUTPUT << "\033[1;31mRobotBlocksWorld constructor\033[0m" << endl;

	targetGrid=NULL;

	idTextureWall=0;
	objBlock = new ObjLoader::ObjLoader("../../simulatorCore/robotBlocksTextures","robotBlock.obj");
	objBlockForPicking = new ObjLoader::ObjLoader("../../simulatorCore/robotBlocksTextures",
												  "robotBlockPicking.obj");
	objRepere = new ObjLoader::ObjLoader("../../simulatorCore/smartBlocksTextures","repere25.obj");

	lattice = new SCLattice(gridSize, gridScale.hasZero() ? defaultBlockSize : gridScale);
}

RobotBlocksWorld::~RobotBlocksWorld() {
	OUTPUT << "RobotBlocksWorld destructor" << endl;
	/*	block linked are deleted by world::~world() */
	delete [] targetGrid;
	delete objBlock;
	delete objBlockForPicking;
	delete objRepere;
}

void RobotBlocksWorld::deleteWorld() {
	delete((RobotBlocksWorld*)world);
}

void RobotBlocksWorld::addBlock(int blockId, BlockCodeBuilder bcb, const Cell3DPosition &pos,
								const Color &col, short orientation, bool master) {
	if (blockId > maxBlockId)
		maxBlockId = blockId;
	else if (blockId == -1)
		blockId = incrementBlockId();

	RobotBlocksBlock *robotBlock = new RobotBlocksBlock(blockId, bcb);
	buildingBlocksMap.insert(std::pair<int,BaseSimulator::BuildingBlock*>
							 (robotBlock->blockId, (BaseSimulator::BuildingBlock*)robotBlock));

	getScheduler()->schedule(new CodeStartEvent(getScheduler()->now(), robotBlock));

	RobotBlocksGlBlock *glBlock = new RobotBlocksGlBlock(blockId);
	tabGlBlocks.push_back(glBlock);
	robotBlock->setGlBlock(glBlock);
	robotBlock->setPosition(pos);
	robotBlock->setColor(col);
	robotBlock->isMaster=master;

	if (lattice->isInGrid(pos)) {
		lattice->insert(robotBlock, pos);
	} else {
		ERRPUT << "ERROR : BLOCK #" << blockId << " out of the grid !!!!!" << endl;
		exit(1);
	}
}

void RobotBlocksWorld::connectBlock(RobotBlocksBlock *block) {
	Cell3DPosition pos = block->position;
	lattice->insert(block, pos);
	OUTPUT << "Reconnection " << block->blockId << " pos = " << pos << endl;
	linkBlock(pos);
	linkNeighbors(pos);
}

void RobotBlocksWorld::disconnectBlock(RobotBlocksBlock *block) {
	P2PNetworkInterface *fromBlock,*toBlock;

	for(int i=0; i<6; i++) {
		fromBlock = block->getInterface(SCLattice::Direction(i));
		if (fromBlock && fromBlock->connectedInterface) {
			toBlock = fromBlock->connectedInterface;
			fromBlock->connectedInterface=NULL;
			toBlock->connectedInterface=NULL;
		}
	}

	lattice->remove(block->position);
	OUTPUT << getScheduler()->now() << " : Disconnection " << block->blockId
		   << " pos =" << block->position << endl;
}

void RobotBlocksWorld::linkBlock(const Cell3DPosition &pos) {
	RobotBlocksBlock *ptrNeighbor;
	RobotBlocksBlock *ptrBlock = (RobotBlocksBlock*)lattice->getBlock(pos);
	vector<Cell3DPosition> nRelCells = lattice->getRelativeConnectivity(pos);
	Cell3DPosition nPos;

	// Check neighbors for each interface
	for (int i = 0; i < 6; i++) {
		nPos = pos + nRelCells[i];
		ptrNeighbor = (RobotBlocksBlock*)lattice->getBlock(nPos);
		if (ptrNeighbor) {
			(ptrBlock)->getInterface(SCLattice::Direction(i))->
				connect(ptrNeighbor->getInterface(SCLattice::Direction(
													  SCLattice::getOpposite(i))));

			OUTPUT << "connection #" << (ptrBlock)->blockId << ":" << SCLattice::getString(i) <<
				" to #" << ptrNeighbor->blockId << ":"
				   << SCLattice::getString(SCLattice::getOpposite(i)) << endl;
		} else {
			(ptrBlock)->getInterface(SCLattice::Direction(i))->connect(NULL);
		}
	}
}

void RobotBlocksWorld::deleteBlock(BuildingBlock *blc) {
	RobotBlocksBlock *bb = (RobotBlocksBlock *)blc;

	if (bb->getState() >= RobotBlocksBlock::ALIVE ) {
		// cut links between bb and others
		for(int i=0; i<6; i++) {
			P2PNetworkInterface *bbi = bb->getInterface(SCLattice::Direction(i));
			if (bbi->connectedInterface) {
				//bb->removeNeighbor(bbi); //Useless
				bbi->connectedInterface->hostBlock->removeNeighbor(bbi->connectedInterface);
				bbi->connectedInterface->connectedInterface=NULL;
				bbi->connectedInterface=NULL;
			}
		}

		disconnectBlock(bb);
	}
	if (selectedGlBlock == bb->ptrGlBlock) {
		selectedGlBlock = NULL;
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

void RobotBlocksWorld::glDraw() {
	static const GLfloat white[]={0.8f,0.8f,0.8f,1.0f},
		gray[]={0.2f,0.2f,0.2f,1.0f};


		glPushMatrix();
		glTranslatef(0.5*lattice->gridScale[0],0.5*lattice->gridScale[1],0.5*lattice->gridScale[2]);
		glDisable(GL_TEXTURE_2D);
		vector <GlBlock*>::iterator ic=tabGlBlocks.begin();
		lock();
		while (ic!=tabGlBlocks.end()) {
			((RobotBlocksGlBlock*)(*ic))->glDraw(objBlock);
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
		glScalef(lattice->gridSize[0]*lattice->gridScale[0],
				 lattice->gridSize[1]*lattice->gridScale[1],
				 lattice->gridSize[2]*lattice->gridScale[2]);
		glBegin(GL_QUADS);
		// bottom
		glNormal3f(0,0,1.0f);
		glTexCoord2f(0,0);
		glVertex3f(0.0f,0.0f,0.0f);
		glTexCoord2f(lattice->gridSize[0],0);
		glVertex3f(1.0f,0.0f,0.0f);
		glTexCoord2f(lattice->gridSize[0],lattice->gridSize[1]);
		glVertex3f(1.0,1.0,0.0f);
		glTexCoord2f(0,lattice->gridSize[1]);
		glVertex3f(0.0,1.0,0.0f);
		// top
		glNormal3f(0,0,-1.0f);
		glTexCoord2f(0,0);
		glVertex3f(0.0f,0.0f,1.0f);
		glTexCoord2f(0,lattice->gridSize[1]);
		glVertex3f(0.0,1.0,1.0f);
		glTexCoord2f(lattice->gridSize[0],lattice->gridSize[1]);
		glVertex3f(1.0,1.0,1.0f);
		glTexCoord2f(lattice->gridSize[0],0);
		glVertex3f(1.0f,0.0f,1.0f);
		// left
		glNormal3f(1.0,0,0);
		glTexCoord2f(0,0);
		glVertex3f(0.0f,0.0f,0.0f);
		glTexCoord2f(lattice->gridSize[1],0);
		glVertex3f(0.0f,1.0f,0.0f);
		glTexCoord2f(lattice->gridSize[1],lattice->gridSize[2]);
		glVertex3f(0.0,1.0,1.0f);
		glTexCoord2f(0,lattice->gridSize[2]);
		glVertex3f(0.0,0.0,1.0f);
		// right
		glNormal3f(-1.0,0,0);
		glTexCoord2f(0,0);
		glVertex3f(1.0f,0.0f,0.0f);
		glTexCoord2f(0,lattice->gridSize[2]);
		glVertex3f(1.0,0.0,1.0f);
		glTexCoord2f(lattice->gridSize[1],lattice->gridSize[2]);
		glVertex3f(1.0,1.0,1.0f);
		glTexCoord2f(lattice->gridSize[1],0);
		glVertex3f(1.0f,1.0f,0.0f);
		// back
		glNormal3f(0,-1.0,0);
		glTexCoord2f(0,0);
		glVertex3f(0.0f,1.0f,0.0f);
		glTexCoord2f(lattice->gridSize[0],0);
		glVertex3f(1.0f,1.0f,0.0f);
		glTexCoord2f(lattice->gridSize[0],lattice->gridSize[2]);
		glVertex3f(1.0f,1.0,1.0f);
		glTexCoord2f(0,lattice->gridSize[2]);
		glVertex3f(0.0,1.0,1.0f);
		// front
		glNormal3f(0,1.0,0);
		glTexCoord2f(0,0);
		glVertex3f(0.0f,0.0f,0.0f);
		glTexCoord2f(0,lattice->gridSize[2]);
		glVertex3f(0.0,0.0,1.0f);
		glTexCoord2f(lattice->gridSize[0],lattice->gridSize[2]);
		glVertex3f(1.0f,0.0,1.0f);
		glTexCoord2f(lattice->gridSize[0],0);
		glVertex3f(1.0f,0.0f,0.0f);
		glEnd();
		glPopMatrix();
		// draw the axes
		glPushMatrix();
		glScalef(0.2f,0.2f,0.2f);
		objRepere->glDraw();
		glPopMatrix();
}

void RobotBlocksWorld::glDrawId() {
	glPushMatrix();
	glTranslatef(0.5*lattice->gridScale[0],0.5*lattice->gridScale[1],0.5*lattice->gridScale[2]);
	glDisable(GL_TEXTURE_2D);
	vector <GlBlock*>::iterator ic=tabGlBlocks.begin();
	int n=1;
	lock();
	while (ic!=tabGlBlocks.end()) {
		((RobotBlocksGlBlock*)(*ic))->glDrawId(objBlock,n);
		ic++;
	}
	unlock();
	glPopMatrix();
}

void RobotBlocksWorld::glDrawIdByMaterial() {
	glPushMatrix();
	glTranslatef(0.5*lattice->gridScale[0],0.5*lattice->gridScale[1],0.5*lattice->gridScale[2]);

	glDisable(GL_TEXTURE_2D);
	vector <GlBlock*>::iterator ic=tabGlBlocks.begin();
	int n=1;
	lock();
	while (ic!=tabGlBlocks.end()) {
		((RobotBlocksGlBlock*)(*ic))->glDrawIdByMaterial(objBlockForPicking,n);
		ic++;
	}
	unlock();
	glPopMatrix();
}


void RobotBlocksWorld::loadTextures(const string &str) {
	string path = str+"/texture_plane.tga";
	int lx,ly;
	idTextureWall = GlutWindow::loadTexture(path.c_str(),lx,ly);
}

void RobotBlocksWorld::updateGlData(RobotBlocksBlock*blc,int prev,int next) {
	RobotBlocksGlBlock *glblc = blc->getGlBlock();
	if (glblc) {
		lock();
		glblc->setPrevNext(prev,next);
		unlock();
	}
}

void RobotBlocksWorld::menuChoice(int n) {
	RobotBlocksBlock *bb = (RobotBlocksBlock *)getBlockById(tabGlBlocks[numSelectedGlBlock]->blockId);
	switch (n) {
	case 1 : {
		OUTPUT << "ADD block link to : " << bb->blockId << "     num Face : " << numSelectedFace << endl;
		vector<Cell3DPosition> nCells = lattice->getRelativeConnectivity(bb->position);
		Cell3DPosition nPos = bb->position + nCells[numSelectedFace];

		addBlock(-1, bb->buildNewBlockCode, nPos, bb->color);
		linkBlock(nPos);
		linkNeighbors(nPos);
	} break;
	case 2 : {
		OUTPUT << "DEL num block : " << tabGlBlocks[numSelectedGlBlock]->blockId << endl;
		deleteBlock(bb);
	} break;
	case 3 : {
		tapBlock(getScheduler()->now(), bb->blockId);
	} break;
	case 4:                 // Save current configuration
		exportConfiguration();
		break;
	}
}

void RobotBlocksWorld::setSelectedFace(int n) {
	numSelectedGlBlock=n/6;
	string name = objBlockForPicking->getObjMtlName(n%6);
	if (name=="face_top") numSelectedFace=SCLattice::Top;
	else if (name=="face_bottom") numSelectedFace=SCLattice::Bottom;
	else if (name=="face_right") numSelectedFace=SCLattice::Right;
	else if (name=="face_left") numSelectedFace=SCLattice::Left;
	else if (name=="face_front") numSelectedFace=SCLattice::Front;
	else if (name=="face_back") numSelectedFace=SCLattice::Back;
}

void RobotBlocksWorld::exportConfiguration() {
	RobotBlocksConfigExporter *exporter = new RobotBlocksConfigExporter(this);
	exporter->exportConfiguration();
	delete exporter;
}

} // RobotBlock namespace
