/*
 * blinkyBlocksGlBlock.h
 *
 *  Created on: 05 juin 2013
 *      Author: ben
 */

#ifndef BLINKYBLOCKSGLBLOCK_H_
#define BLINKYBLOCKSGLBLOCK_H_
#include <string>
#include <objLoader.h>
#include "glBlock.h"

namespace BlinkyBlocks {
class BlinkyBlocksGlBlock:public GlBlock {
protected :
public :
	BlinkyBlocksGlBlock(bID id) : GlBlock(id) {};
	virtual ~BlinkyBlocksGlBlock() {};

	virtual void glDraw(ObjLoader::ObjLoader *ptrObj);
};
}
#endif /* BLINKYBLOCKSGLBLOCK_H_ */
