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
#include "blinkyBlocksGlBlock.h"
#include <boost/asio.hpp> 
#include <stdexcept>

using boost::asio::ip::tcp;

typedef struct VMMessage_tt {
        uint64_t size;
        //uint64_t type;
        //uint64_t timestamp;
        //uint64_t sourcenode;
        uint64_t *message;
} VMMessage_t;

namespace BlinkyBlocks {

enum NeighborDirection { Front=0, Back, Left, Right, Top, Bottom };

class BlinkyBlocksBlockCode;

class BlinkyBlocksBlock : public BaseSimulator::BuildingBlock {
	P2PNetworkInterface *tabInterfaces[6];

public:
	BlinkyBlocksGlBlock *ptrGlBlock;
	Vecteur color; // color of the block
	Vecteur position; // position of the block;

	BlinkyBlocksBlockCode *(*buildNewBlockCode)(BlinkyBlocksBlock*);

	BlinkyBlocksBlock(int bId, boost::shared_ptr<tcp::socket> s, pid_t p, BlinkyBlocksBlockCode *(*blinkyBlocksBlockCodeBuildingFunction)(BlinkyBlocksBlock*));

	~BlinkyBlocksBlock();

	inline BlinkyBlocksGlBlock* getGlBlock() { return ptrGlBlock; };
	inline void setGlBlock(BlinkyBlocksGlBlock*ptr) { ptrGlBlock=ptr;};
	void setColor(const Vecteur &c);
	void setColor(int num);
	void setPosition(const Vecteur &p);
	inline P2PNetworkInterface *getInterface(NeighborDirection d) { return tabInterfaces[d]; }
	void waitVMEnd();
	NeighborDirection getDirection(P2PNetworkInterface*);
	VMMessage_t*  getBufferPtr() { return &buffer;}
	
	void sendMessageToVM(uint64_t size, uint64_t* message);
	void readMessageFromVM();
	
	void tap();
	void accel(int x, int y, int z);	
	void shake(int f);
	
	void addNeighbor(P2PNetworkInterface *ni, BuildingBlock* target);
	void removeNeighbor(P2PNetworkInterface *ni);
	
	void stop();
	
protected:
	boost::shared_ptr<tcp::socket> socket;
	pid_t pid;
	VMMessage_t buffer;
	
	void readMessageHandler(const boost::system::error_code& error, std::size_t bytes_transferred);	
	tcp::socket& getSocket() {return *(socket.get());}
};

}

#endif /* BLINKYBLOCKSBLOCK_H_ */
