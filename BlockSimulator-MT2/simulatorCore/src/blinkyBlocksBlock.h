/*
 * blinkyBlocksBlock.h
 *
 *  Created on: 23 mars 2013
 *      Author: dom
 */

#ifndef BLINKYBLOCKSBLOCK_H_
#define BLINKYBLOCKSBLOCK_H_

#include "buildingBlock.h"
#include "blinkyBlocksVM.h"
#include "blinkyBlocksBlockCode.h"
#include "blinkyBlocksGlBlock.h"
#include <boost/asio.hpp> 
#include <stdexcept>

/* Inside VM:
enum face_t {
   INVALID_FACE = -1,
   BOTTOM = 0,
   NORTH = 1,
   EAST = 2,
   WEST = 3,
   SOUTH = 4,
   TOP = 5
};
*/
namespace BlinkyBlocks {

//enum NeighborDirection { Front=0, Back, Left, Right, Top, Bottom };
// if we consider that the NORTH in the Back at the beginning:

class NeighborDirection {
public:
	enum Direction { Bottom = 0, Back = 1, Right, Front, Left, Top };
	static int getOpposite(int d);
	static string getString(int d);
};

class BlinkyBlocksBlockCode;
class BlinkyBlocksVM;

class BlinkyBlocksBlock : public BaseSimulator::BuildingBlock {
	P2PNetworkInterface *tabInterfaces[6];

public:
	BlinkyBlocksGlBlock *ptrGlBlock;
	Vecteur color; // color of the block
	Vecteur position; // position of the block;
	BlinkyBlocksVM *vm;

	BlinkyBlocksBlockCode *(*buildNewBlockCode)(BlinkyBlocksBlock*);
	BlinkyBlocksBlock(int bId, BlinkyBlocksBlockCode *(*blinkyBlocksBlockCodeBuildingFunction)(BlinkyBlocksBlock*));
	~BlinkyBlocksBlock();

	inline BlinkyBlocksGlBlock* getGlBlock() { return ptrGlBlock; };
	inline void setGlBlock(BlinkyBlocksGlBlock*ptr) { ptrGlBlock=ptr;};
	void setColor(const Vecteur &c);
	void setColor(int num);
	void setPosition(const Vecteur &p);
	inline P2PNetworkInterface *getInterface(NeighborDirection::Direction d) { return tabInterfaces[d]; }
	inline P2PNetworkInterface *getInterfaceDestId(int id) {
		for (int i=0; i<6; i++) {
			if (tabInterfaces[i]->connectedInterface != NULL) {
					if (tabInterfaces[i]->connectedInterface->hostBlock->blockId == id) {
							return tabInterfaces[i];
					}
			}
		}
		return NULL;
	}
	NeighborDirection::Direction getDirection(P2PNetworkInterface*);

	void stopVM();

	/* schedule the appropriate event for this action */
	void tap();
	void accel(int x, int y, int z);	
	void shake(int f);	
	void addNeighbor(P2PNetworkInterface *ni, BuildingBlock* target);
	void removeNeighbor(P2PNetworkInterface *ni);	
	void stop();

};

}

#endif /* BLINKYBLOCKSBLOCK_H_ */
