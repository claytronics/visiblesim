/*
 * blockCode.cpp
 *
 *  Created on: 22 mars 2013
 *      Author: dom
 */

#include <iostream>
#include "blockCode.h"

using namespace std;

namespace BaseSimulator {

BlockCode::BlockCode(BuildingBlock *host) {
	cout << "BlockCode constructor" << endl;
	hostBlock = host;
	availabilityDate = 0;
}

BlockCode::~BlockCode() {
	cout << "BlockCode destructor" << endl;
}

} // BaseSimulator namespace
