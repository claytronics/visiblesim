/*
 * smartBlocksGlBlock.h
 *
 *  Created on: 23 avr. 2013
 *      Author: ben
 */

#ifndef SMARTBLOCKSGLBLOCK_H_
#define SMARTBLOCKSGLBLOCK_H_
#include <string>
#include <objLoader.h>
#include "glBlock.h"

namespace SmartBlocks {
class SmartBlocksGlBlock:public GlBlock {
protected :
//	string popupInfo,textInfo;
	int displayedValue;
public :
	SmartBlocksGlBlock(int id);
	virtual ~SmartBlocksGlBlock();
	virtual string getInfo();
	/*	virtual string getPopupInfo();*/
	void setDisplayedValue(int n) { displayedValue=n; }
	void glDraw(ObjLoader::ObjLoader *ptrObj);
	void glDrawId(ObjLoader::ObjLoader *ptrObj);
	void glDrawIdByMaterial(ObjLoader::ObjLoader *ptrObj,int &n);
};
}
#endif /* SMARTBLOCKSGLBLOCK_H_ */
