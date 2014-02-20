/*
 * blinkyBlocksBlockCode.cpp
 *
 *  Created on: 23 mars 2013
 *      Author: dom
 */

#include <iostream>
#include "blinkyBlocksBlockCode.h"
#include "trace.h"

using namespace std;

namespace BlinkyBlocks {

BlinkyBlocksBlockCode::BlinkyBlocksBlockCode(BlinkyBlocksBlock *host):BlockCode(host) {
	OUTPUT << "BlinkyBlocksBlockCode constructor" << endl;
	// mode fastest:
	currentLocalDate = 0; // set correclty in block init function
	hasWork = true;
	polling = false;
}

BlinkyBlocksBlockCode::~BlinkyBlocksBlockCode() {
	OUTPUT << "BlinkyBlocksBlockCode destructor" << endl;
}

/*
BlinkyBlocksBlockCode* BlinkyBlocksBlockCode::buildNewBlockCode(BlinkyBlocksBlock *host) {
	return(new BlinkyBlocksBlockCode(host));
}
*/

}
