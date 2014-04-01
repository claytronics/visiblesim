/*
 * blinkyBlocksBlockCode.h
 *
 *  Created on: 23 mars 2013
 *      Author: dom
 */

#ifndef BLINKYBLOCKSBLOCKCODE_H_
#define BLINKYBLOCKSBLOCKCODE_H_

#include "blockCode.h"
#include "blinkyBlocksBlock.h"
#include "blinkyBlocksVMCommands.h"
#include <vector>

namespace BlinkyBlocks {

class BlinkyBlocksBlock;
class BarycentricLeaderElectionTimeoutEvent;

class BlinkyBlocksBlockCode : public BaseSimulator::BlockCode {
public:
	// Deterministic mode (fastest mode):
	uint64_t currentLocalDate; 	// Time unit: us 
								//represents the elapsed CPU time 
								// (cycles/cpu_frequency)
	bool hasWork; // fastest mode
	bool polling; // fastest mode
	
	// Clock synchronization
	bool timeLeader;
		// Time Leader Election:
	unsigned int nbNeededAnswers;
	bool electing;
			// MIN ID:
	unsigned int minId;
	P2PNetworkInterface *minIdSender;
			// BARYCENTRIC:
	vector<unsigned int> distances;
	BarycentricLeaderElectionTimeoutEvent *barycentricLETimeoutEv;
	list<pair<unsigned int, unsigned int> > sums;
		// Spanning tree building
		// Breadth-First Spanning Tree, source: Distributed Algorithms
		// for Message-Passing Systems, Michel Raynal, 2013
	P2PNetworkInterface *parent;
	//unsigned int parent;
	unsigned int level;		
	list<P2PNetworkInterface *> clockSyncChildren;
	
	void initClockSync();
	inline bool isTimeLeader() { return timeLeader; }
	void printChildren();
	void synchronizeNeighborClocks(uint8_t nbhops);
	void launchSynchronizationWave(uint64_t t);
	
	void initTimeLeaderElection();
	void initMinIdTimeLeaderElection();
	void initBarycentricTimeLeaderElection();
	void initStaticLeaderElection();
	
	void timeLeaderSubRoutine();
	unsigned int broadcastElectionMessage();
	unsigned int broadcastDistanceMessage(unsigned int id, unsigned int distance, P2PNetworkInterface *excluded);
	void barycentricLeaderElectionTimeout();
	unsigned int  getDistanceSum();
	void barycentricLEtryToElect();
	unsigned int ST_launch_go();
	void ST_send_back(bool a, P2PNetworkInterface *bbi, unsigned int l);
	
	bool belongsTo(list<P2PNetworkInterface*> l, P2PNetworkInterface *bbi);

	BlinkyBlocksBlockCode(BlinkyBlocksBlock *host);
	virtual ~BlinkyBlocksBlockCode();

	virtual void handleCommand(VMCommand &command) = 0;
	virtual void handleDeterministicMode(VMCommand &command) = 0;
	virtual void init() = 0;
	
	inline uint64_t getCurrentLocalDate() { return currentLocalDate; }

	//static BlinkyBlocksBlockCode* buildNewBlockCode(BlinkyBlocksBlock *host);
	virtual void processLocalEvent(EventPtr pev) = 0;
	
	bool handleSystemMessage(EventPtr pev);
};

}

#endif /* BLINKYBLOCKSBLOCKCODE_H_ */
