/*
 * blinkyBlocksNetwork.h
 *
 *  Created on: 19 fevrier 2014
 *      Author: andre
 */

#ifndef BLINKYBLOCKSNETWORK_H_
#define BLINKYBLOCKSNETWORK_H_

#include <network.h>

using namespace std;

class Chunk;
class SerialNetworkInterface;


//===========================================================================================================
//
//          Chunk  (class)
//
//===========================================================================================================

class Chunk: public Message {
protected:
public:
	char data[17]; 

	Chunk(char *d, unsigned int size);
	virtual ~Chunk();

	unsigned int size();
};

//===========================================================================================================
//
//          SerialNetworkInterface  (class)
//
//===========================================================================================================

class SerialNetworkInterface : public P2PNetworkInterface {

public:
	SerialNetworkInterface(BaseSimulator::BuildingBlock *b);
	~SerialNetworkInterface();
	unsigned int computeTransmissionDuration(unsigned int size);
};

#endif /* BLINKYBLOCKSNETWORK_H_ */
