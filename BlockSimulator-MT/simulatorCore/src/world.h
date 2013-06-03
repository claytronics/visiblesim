/*
 * world.h
 *
 *  Created on: 16 févr. 2013
 *      Author: dom
 */

#ifndef WORLD_H_
#define WORLD_H_

#include <vector>
#include "buildingBlock.h"

using namespace std;

class World {
protected:
	//vector <BuildingBlock*>blockVect;
public:
	vector <BuildingBlock*>blockVect;
	//vector <BuildingBlock*>blockVect;
	World();
	virtual ~World();
};

#endif /* WORLD_H_ */
