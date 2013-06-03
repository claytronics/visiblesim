/*
 * blinkyBlocksBlockCode.cpp
 *
 *  Created on: 23 mars 2013
 *      Author: dom
 */

#include <iostream>
#include "blinkyBlocksBlockCode.h"

using namespace std;

namespace BlinkyBlocks {

BlinkyBlocksBlockCode::BlinkyBlocksBlockCode(BlinkyBlocksBlock *host):BlockCode(host) {
	cout << "BlinkyBlocksBlockCode constructor" << endl;
}

BlinkyBlocksBlockCode::~BlinkyBlocksBlockCode() {
	cout << "BlinkyBlocksBlockCode destructor" << endl;
}
/*
BlinkyBlocksBlockCode* BlinkyBlocksBlockCode::buildNewBlockCode(BlinkyBlocksBlock *host) {
	return(new BlinkyBlocksBlockCode(host));
}
*/

}
