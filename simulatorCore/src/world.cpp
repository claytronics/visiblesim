/*
 * world.cpp
 *
 *  Created on: 23 mars 2013
 *      Author: dom
 */

#include <stdlib.h>

#include "world.h"
#include "trace.h"
#include "openglViewer.h"

using namespace std;

namespace BaseSimulator {

World *World::world=NULL;
map<bID, BuildingBlock*>World::buildingBlocksMap;
vector <GlBlock*>World::tabGlBlocks;

World::World(int argc, char *argv[]) {
	OUTPUT << "World constructor" << endl;
	selectedGlBlock = NULL;
	numSelectedFace=0;
	numSelectedGlBlock=0;
	menuId = 0;

	if (world == NULL) {
		world = this;

		if (GlutContext::GUIisEnabled) {
			GlutContext::init(argc,argv);
			camera = new Camera(-M_PI/2.0,M_PI/3.0,750.0);
			camera->setLightParameters(Vector3D(0,0,0),45.0,80.0,800.0,45.0,10.0,1500.0);
			camera->setTarget(Vector3D(0,0,1.0));
		}
	} else {
		ERRPUT << "\033[1;31m" << "Only one World instance can be created, aborting !" << "\033[0m" << endl;
		exit(EXIT_FAILURE);
	}
}

World::~World() {
	// free building blocks
	std::map<bID, BuildingBlock*>::iterator it;
	for( it = buildingBlocksMap.begin() ; it != buildingBlocksMap.end() ; ++it) {
		delete it->second;
	}

	// free glBlocks
	std::vector<GlBlock*>::const_iterator cit=tabGlBlocks.begin();
	while (cit!=tabGlBlocks.end()) {
		delete *cit;
		cit++;
	}

	// /* free Scenario Events */
	// vector<ScenarioEvent*>::const_iterator it=tabEvents.begin();
	// while (it!=tabEvents.end()) {
	// 	delete (*it);
	// 	it++;
	// }
	// tabEvents.clear();

	delete lattice;
	delete camera;
	// delete [] targetGrid;
	delete objBlock;
    delete objBlockForPicking;
    delete objRepere;

	OUTPUT << "World destructor" << endl;
}


BuildingBlock* World::getBlockById(int bId) {
	map<bID, BuildingBlock*>::iterator it;
	it = buildingBlocksMap.find(bId);
	if (it == buildingBlocksMap.end()) {
		return(NULL);
	} else {
		return(it->second);
	}
}

void World::updateGlData(BuildingBlock *bb) {
	GlBlock *glblc = bb->getGlBlock();
	if (glblc) {
		lock();
		glblc->setPosition(lattice->gridToWorldPosition(bb->position));
		glblc->setColor(bb->color);
		unlock();
	}
}

void World::updateGlData(BuildingBlock*blc, Vector3D &p) {
	GlBlock *glblc = blc->getGlBlock();
	if (glblc) {
		lock();
		glblc->setPosition(p);
		unlock();
	}
}

void World::linkBlocks() {
	Cell3DPosition p;

	for (p.pt[2] = 0; p[2] < lattice->gridSize[2]; p.pt[2]++) { // z
		for (p.pt[1] = 0; p[1] < lattice->gridSize[1]; p.pt[1]++) { // y
			for(p.pt[0] = 0; p[0] < lattice->gridSize[0]; p.pt[0]++) { // x
				if (lattice->cellHasBlock(p)) {
					// cerr << "l.cellHasBlock(" << p << "/" << lattice->getIndex(p) << ")  = true ; id: "
					//	 << lattice->getBlock(p)->blockId << endl;
					linkBlock(p);
				}
			}
		}
	}
}

void World::linkNeighbors(const Cell3DPosition &pos) {
	vector<Cell3DPosition> nCells = lattice->getActiveNeighborCells(pos);

	// Check neighbors for each interface
	for (Cell3DPosition nPos : nCells) {
		linkBlock(nPos);
	}
}


void World::connectBlock(BuildingBlock *block) {
    Cell3DPosition pos = block->position;
    OUTPUT << "Connect Block " << block->blockId << " pos = " << pos << endl;
    lattice->insert(block, pos);
    linkBlock(pos);
    linkNeighbors(pos);
}

void World::disconnectBlock(BuildingBlock *block) {
    P2PNetworkInterface *fromBlock,*toBlock;

    for(int i = 0; i < block->getNbInterfaces(); i++) {
        fromBlock = block->getInterface(i);
        if (fromBlock && fromBlock->connectedInterface) {
	    toBlock = fromBlock->connectedInterface;

	    // Clear message queue
	    fromBlock->outgoingQueue.clear();
	    toBlock->outgoingQueue.clear();

	    // Notify respective codeBlocks
	    block->removeNeighbor(fromBlock);
	    fromBlock->connectedInterface->hostBlock->removeNeighbor(fromBlock->connectedInterface);

	    // Disconnect the interfaces
            fromBlock->connectedInterface = NULL;
            toBlock->connectedInterface = NULL;
        }
    }

    lattice->remove(block->position);

    OUTPUT << getScheduler()->now() << " : Disconnect Block " << block->blockId <<
        " pos = " << block->position << endl;
}

void World::deleteBlock(BuildingBlock *bb) {
    if (bb->getState() >= BuildingBlock::ALIVE ) {
        // cut links between bb and others and remove it from the grid
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

void World::stopSimulation() {
	map<bID, BuildingBlock*>::iterator it;
	for( it = buildingBlocksMap.begin() ; it != buildingBlocksMap.end() ; it++) {
		// it->second->stop();
	}
}

bool World::canAddBlockToFace(bID numSelectedGlBlock, int numSelectedFace) {
	BuildingBlock *bb = getBlockById(tabGlBlocks[numSelectedGlBlock]->blockId);
	Cell3DPosition pos = bb->position;
	vector<Cell3DPosition> nCells = lattice->getRelativeConnectivity(pos);
	// if (numSelectedFace < lattice->getMaxNumNeighbors())
	// 	cerr << "numSelectedFace: " << numSelectedFace << " f"
	// 		 << pos << "+" << nCells[numSelectedFace]
	// 		 << " = " << lattice->isFree(pos + nCells[numSelectedFace]) << endl;

	return numSelectedFace < lattice->getMaxNumNeighbors() ?
		lattice->isFree(pos + nCells[numSelectedFace]) : false;
}

void World::menuChoice(int n) {
    BuildingBlock *bb = getSelectedBuildingBlock();

	switch (n) {
	case 1 : {
		OUTPUT << "ADD block link to : " << bb->blockId << "     num Face : " << numSelectedFace << endl;
		vector<Cell3DPosition> nCells = lattice->getRelativeConnectivity(bb->position);
		Cell3DPosition nPos = bb->position + nCells[numSelectedFace];

		addBlock(0, bb->buildNewBlockCode, nPos, bb->color);
		linkBlock(nPos);
		linkNeighbors(nPos);
	} break;
	case 2 : {
		OUTPUT << "DEL num block : " << tabGlBlocks[numSelectedGlBlock]->blockId << endl;
		deleteBlock(bb);
	} break;
	case 3 : {
		tapBlock(getScheduler()->now(), bb->blockId, numSelectedFace);
	} break;
	case 4:                 // Save current configuration
		exportConfiguration();
		break;
	}
}

void World::createHelpWindow() {
	if (GlutContext::helpWindow)
		delete GlutContext::helpWindow;
	GlutContext::helpWindow = new GlutHelpWindow(NULL,10,40,540,500,"../../simulatorCore/resources/help/genericHelp.txt");
}

void World::tapBlock(Time date, bID bId, int face) {
	BuildingBlock *bb = getBlockById(bId);
	// cerr << bb->blockId << " : " << bb->position << " : " << face << endl;
	bb->tap(date, face < lattice->getMaxNumNeighbors() ? face : -1);
}

void World::addObstacle(const Cell3DPosition &pos,const Color &col) {
	GlBlock *glBlock = new GlBlock(-1);
    Vector3D position(lattice->gridScale[0]*pos[0],
					  lattice->gridScale[1]*pos[1],
					  lattice->gridScale[2]*pos[2]);
	glBlock->setPosition(position);
	glBlock->setColor(col);
	tabGlBlocks.push_back(glBlock);
}


void World::createPopupMenu(int ix, int iy) {
	if (!GlutContext::popupMenu) {
		GlutContext::popupMenu = new GlutPopupMenuWindow(NULL,0,0,200,180);
		GlutContext::popupMenu->addButton(1,"../../simulatorCore/resources/textures/menuTextures/menu_add.tga");
		GlutContext::popupMenu->addButton(2,"../../simulatorCore/resources/textures/menuTextures/menu_del.tga");
		GlutContext::popupMenu->addButton(3,"../../simulatorCore/resources/textures/menuTextures/menu_tap.tga");
		GlutContext::popupMenu->addButton(4,"../../simulatorCore/resources/textures/menuTextures/menu_save.tga");
		GlutContext::popupMenu->addButton(5,"../../simulatorCore/resources/textures/menuTextures/menu_cancel.tga");
	}

	if (iy < GlutContext::popupMenu->h) iy = GlutContext::popupMenu->h;
cerr << "Block " << numSelectedGlBlock << ":" << lattice->getDirectionString(numSelectedFace)
         << " selected" << endl;
	// cerr << "Block " << numSelectedGlBlock << ":" << numSelectedFace << " selected" << endl;

	GlutContext::popupMenu->activate(1, canAddBlockToFace((int)numSelectedGlBlock, (int)numSelectedFace));
	GlutContext::popupMenu->setCenterPosition(ix,GlutContext::screenHeight-iy);
	GlutContext::popupMenu->show(true);
}

} // BaseSimulator namespace
