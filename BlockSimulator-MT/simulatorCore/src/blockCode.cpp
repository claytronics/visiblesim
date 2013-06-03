/*
 * blockCode.cpp
 *
 *  Created on: 15 févr. 2013
 *      Author: dom
 */

#include <iostream>
#include "blockCode.h"

using namespace std;

BlockCode::BlockCode(BuildingBlock *host) {
	cout << "BlockCode constructor" << endl;
	hostBlock = host;
	availabilityDate = 0;
}

BlockCode::~BlockCode() {
	cout << "BlockCode destructor" << endl;
}

/*
BlockCode *BlockCode::buildNewBlockCode() {
	return(new BlockCode());
}
*/
