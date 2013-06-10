/*
 * blinkyBlocksBlock.h
 *
 *  Created on: 23 mars 2013
 *      Author: dom
 */

#ifndef BLINKYBLOCKSBLOCK_H_
#define BLINKYBLOCKSBLOCK_H_

#include "buildingBlock.h"
#include "blinkyBlocksBlockCode.h"
#include <boost/asio.hpp> 

using boost::asio::ip::tcp;

namespace BlinkyBlocks {

class BlinkyBlocksBlockCode;

class BlinkyBlocksBlock : public BaseSimulator::BuildingBlock {
public:
	BlinkyBlocksBlockCode *(*buildNewBlockCode)(BlinkyBlocksBlock*);
	BlinkyBlocksBlock(int bId, boost::shared_ptr<tcp::socket> s, pid_t p, BlinkyBlocksBlockCode *(*blinkyBlocksBlockCodeBuildingFunction)(BlinkyBlocksBlock *));
	~BlinkyBlocksBlock();
protected:
	boost::shared_ptr<tcp::socket> socket;
	pid_t pid;
};

}

#endif /* BLINKYBLOCKSBLOCK_H_ */
