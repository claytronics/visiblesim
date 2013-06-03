/*
 * glBlock.h
 *
 *  Created on: 23 avr. 2013
 *      Author: ben
 */

#ifndef SMARTBLOCKSGLBLOCK_H_
#define SMARTBLOCKSGLBLOCK_H_
#include <string>
#include <objLoader.h>
#include "glBlock.h"

class SmartBlocksGlBlock:public GlBlock {
protected :
public :
	SmartBlocksGlBlock(int id);
	virtual ~SmartBlocksGlBlock();
	virtual string getInfo();

	void glDraw(ObjLoader::ObjLoader *ptrObj);
	void glDrawId(ObjLoader::ObjLoader *ptrObj);
};

#endif /* GLBLOCK_H_ */
