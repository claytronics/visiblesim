/*
 * network.cpp
 *
 *  Created on: 24 mars 2013
 *      Author: dom
 */

#include <iostream>
#include <sstream>
#include "scheduler.h"
#include "blinkyBlocksNetwork.h"
#include "trace.h"

using namespace std;


//===========================================================================================================
//
//          Chunk  (class)
//
//===========================================================================================================

Chunk::Chunk(char *d, unsigned int size) : Message() {
	memcpy(data,d,size);	
}

Chunk::~Chunk() {
	nbMessages--;
}

unsigned int Chunk::size() {
	// without
	// data : 17 bytes
	// Frame delimiter (FD) : 1 byte
	// Pointer message handler: 2 bytes
	// Checkum : 1 byte
	return (17);
}

//===========================================================================================================
//
//          P2PNetworkInterface  (class)
//
//===========================================================================================================

// 1 start + 8 data + 1 stop
#define BITS_PER_SYMBOL 10 
// ms
#define MIN_TIME 6.08
#define MAX_TIME 6.11
SerialNetworkInterface::SerialNetworkInterface(BaseSimulator::BuildingBlock *b) : P2PNetworkInterface(b) {
	dataRate = 38400;
}

SerialNetworkInterface::~SerialNetworkInterface() {}

unsigned int SerialNetworkInterface::computeTransmissionDuration(unsigned int size) {
	float t = 0.0;
	unsigned int nbChunks = size/17;
	 
	if ( (size % 17) != 0) {
		nbChunks++;
	}
	for (int i = 0; i < nbChunks; i++) {
		t += (rand()/(double)RAND_MAX ) * (MAX_TIME-MIN_TIME) + MIN_TIME;
	}
	//cout << "size: " << size << endl;
	//cout << "nbChunks: " << nbChunks << ", time: " << t << " ms" << endl;
	return (t * 1000); // ms to us (simulator unit)
}
