/*
 * blinkyBlocksBlockCode.cpp
 *
 *  Created on: 23 mars 2013
 *      Author: dom
 */

#include <iostream>
#include "blinkyBlocksBlockCode.h"
#include "trace.h"
#include "blinkyBlocksEvents.h"
#include <climits>

// 5 messages * 10 ms 
#define BARYCENTRIC_LEADER_ELECTION_TIMEOUT (5*10*1000)
// 2ms
#define SYNC_PERIOD (2*1000)
// Only used if static leader strategy is used
#define STATIC_LEADER_ID 2

#define NB_SYNC 3

using namespace std;

namespace BlinkyBlocks {

BlinkyBlocksBlockCode::BlinkyBlocksBlockCode(BlinkyBlocksBlock *host):BlockCode(host) {
	OUTPUT << "BlinkyBlocksBlockCode constructor" << endl;
	// mode fastest:
	currentLocalDate = 0; // set correclty in block init function
	hasWork = true;
	polling = false;
	
	//Clock sync, leader election	
	timeLeader = false;
	nbNeededAnswers = 0;
	electing = false;
	
	minId = host->blockId;
	minIdSender = NULL;
	
	barycentricLETimeoutEv = NULL;
	
	parent = NULL;
	level = 0;
}

BlinkyBlocksBlockCode::~BlinkyBlocksBlockCode() {
	OUTPUT << "BlinkyBlocksBlockCode destructor" << endl;
}

/*
BlinkyBlocksBlockCode* BlinkyBlocksBlockCode::buildNewBlockCode(BlinkyBlocksBlock *host) {
	return(new BlinkyBlocksBlockCode(host));
}
*/

//======================================================================
//
//          Time Leader Election
//======================================================================

void BlinkyBlocksBlockCode::initTimeLeaderElection() {
	electing = true;
	timeLeader = false;
	nbNeededAnswers = 0;
	
	//initMinIdTimeLeaderElection();
	//initBarycentricTimeLeaderElection();
	initStaticLeaderElection();
}

void BlinkyBlocksBlockCode::timeLeaderSubRoutine() {
	BlinkyBlocksBlock *bb = (BlinkyBlocksBlock*) hostBlock;
	
	cout << "@" <<  hostBlock->blockId << " time leader!" << endl;
	timeLeader = true;
	// RED
	getScheduler()->schedule(new VMSetColorEvent(getScheduler()->now(), bb, 1.0,0.0,0.0,1.0));
}

/** LEADER CRITERIA = STATIC ID  **/
void BlinkyBlocksBlockCode::initStaticLeaderElection() {

	electing = false;
	parent = NULL;
	level = 0;
	if (hostBlock->blockId == STATIC_LEADER_ID) {
		timeLeaderSubRoutine();
		nbNeededAnswers = ST_launch_go();
		if (nbNeededAnswers == 0) {
			launchSynchronizationWave(getScheduler()->now() + 2*1000); // now + 2ms
		}
	}
}

/** LEADER CRITERIA = MIN ID  **/
void BlinkyBlocksBlockCode::initMinIdTimeLeaderElection() {
	BlinkyBlocksBlock *bb = (BlinkyBlocksBlock*) hostBlock;
	
	minId = bb->blockId;
	minIdSender = NULL;
	nbNeededAnswers = broadcastElectionMessage();
	if (nbNeededAnswers == 0) { // No Neighbor
		// TIME LEADER!
		timeLeaderSubRoutine();
		launchSynchronizationWave(getScheduler()->now() + 2*1000); // now + 2ms
	}
	
}

unsigned int BlinkyBlocksBlockCode::broadcastElectionMessage() {
	P2PNetworkInterface *bbi = NULL;
	BlinkyBlocksBlock *bb = (BlinkyBlocksBlock*) hostBlock;
	uint64_t rva1 = 0;
	uint64_t rva2 = 0;
	unsigned int sent = 0;
	
	for (int i=0; i<6; i++) {
		bbi = bb->getInterface(NeighborDirection::Direction(i));
		if (bbi == minIdSender) {
			continue;
		}
		if (bbi->connectedInterface) {
			rva1 = (rand()/(double)RAND_MAX) * (1500-0) + 0; // random variable between each CLOCK_SYNC message sent
												// between 0 and 1500 us
			getScheduler()->schedule(new NetworkInterfaceEnqueueOutgoingEvent(getScheduler()->now() + rva1 + rva2, new BlinkyBlocksLeaderElectionMinIdElectionMsg(minId), bbi));
			rva2 += (rand()/(double)RAND_MAX) * (10-0) + 0; // Simulate loop processing
			sent++;
		}
	}
	return sent;
}

/** LEADER CRITERIA = BARYCENTRIC ELECTION  **/

void BlinkyBlocksBlockCode::initBarycentricTimeLeaderElection() {
	BlinkyBlocksBlock *bb = (BlinkyBlocksBlock*) hostBlock;
	
	distances.clear();
	distances.resize(World::getSize(), 0);
	sums.clear();
	parent = NULL;
	
	if (bb->getNbNeighbors() == 0) {
		barycentricLEtryToElect();
	} else {
		broadcastDistanceMessage(bb->blockId, 1, NULL);
		barycentricLETimeoutEv = new BarycentricLeaderElectionTimeoutEvent(getScheduler()->now() + BARYCENTRIC_LEADER_ELECTION_TIMEOUT, bb);
		getScheduler()->schedule(barycentricLETimeoutEv);
	}
}

unsigned int BlinkyBlocksBlockCode::broadcastDistanceMessage(unsigned int id, unsigned int distance, P2PNetworkInterface *excluded) {
	P2PNetworkInterface *bbi = NULL;
	BlinkyBlocksBlock *bb = (BlinkyBlocksBlock*) hostBlock;
	uint64_t rva1 = 0;
	uint64_t rva2 = 0;
	unsigned int sent = 0;
	
	for (int i=0; i<6; i++) {
		bbi = bb->getInterface(NeighborDirection::Direction(i));
		if (bbi == excluded) {
			continue;
		}
		if (bbi->connectedInterface) {
			rva1 = (rand()/(double)RAND_MAX) * (1500-0) + 0; // random variable between each CLOCK_SYNC message sent
												// between 0 and 1500 us
			getScheduler()->schedule(new NetworkInterfaceEnqueueOutgoingEvent(getScheduler()->now() + rva1 + rva2, new BlinkyBlocksLEBarBlockDistanceMsg(id, distance), bbi));
			rva2 += (rand()/(double)RAND_MAX) * (10-0) + 0; // Simulate loop processing
			sent++;
		}
	}
	return sent;
}

void BlinkyBlocksBlockCode::barycentricLeaderElectionTimeout() {
	// Assume that the timeout event has not been cancelled
	BlinkyBlocksBlock *bb = (BlinkyBlocksBlock*) hostBlock;
	P2PNetworkInterface *bbi = NULL;
	uint64_t rva1 = 0;
	uint64_t rva2 = 0;
	unsigned int sum = getDistanceSum();
	
	for (int i=0; i<6; i++) {
		bbi = bb->getInterface(NeighborDirection::Direction(i));
		if (bbi->connectedInterface) {
			rva1 = (rand()/(double)RAND_MAX) * (1500-0) + 0; // random variable between each CLOCK_SYNC message sent
												// between 0 and 1500 us
			getScheduler()->schedule(new NetworkInterfaceEnqueueOutgoingEvent(getScheduler()->now() + rva1 + rva2,  new BlinkyBlocksLEBarSumInfoMsg(sum), bbi));
			rva2 += (rand()/(double)RAND_MAX) * (10-0) + 0; // Simulate loop processing
		}
	}
	
	barycentricLEtryToElect();
	
	barycentricLETimeoutEv = NULL;
}

void BlinkyBlocksBlockCode::barycentricLEtryToElect() {
	unsigned int sum = getDistanceSum();
	BlinkyBlocksBlock *bb = (BlinkyBlocksBlock*) hostBlock;
	bool stillCandidate = true;
	
	if (sums.size() >= bb->getNbNeighbors()) {
		for (std::list<pair<unsigned int, unsigned int> >::iterator it = sums.begin(); it != sums.end(); it++) {
			if ((sum > it->second) || ( (sum == it->second) && ((unsigned int)bb->blockId > it->first) )) {
				stillCandidate = false;
				break;
			}
		}
		if (stillCandidate) { // LEADER!
			timeLeaderSubRoutine();
			nbNeededAnswers = ST_launch_go();
			if (nbNeededAnswers == 0) {
				launchSynchronizationWave(getScheduler()->now() + 2*1000); // now + 2ms
			}
		}
	}
}

unsigned int  BlinkyBlocksBlockCode::getDistanceSum() {
	BlinkyBlocksBlock *bb = (BlinkyBlocksBlock*) hostBlock;
	unsigned int sum = 0;
	
	for (unsigned int i = 0; i < distances.size(); i++) {
		if ((distances[i] == 0) && ((unsigned int) (bb->blockId-1) != i)) {
			cerr << "Warning Barycentric Leader Election may be wrong..." << endl;
		}
		sum += distances[i];
	}
	return sum;
}

/** SPANNING-TREE BUILDING (ONLY USED WITH THE BARYCENTRIC ELECTION AND STATIC LEADER STRATEGY)  **/

unsigned int BlinkyBlocksBlockCode::ST_launch_go() {
	BlinkyBlocksBlock *bb = (BlinkyBlocksBlock*) hostBlock;
	P2PNetworkInterface *bbi = NULL;
	uint64_t rva1 = 0;
	uint64_t rva2 = 0;
	unsigned int sent = 0;
	
	for (int i=0; i<6; i++) {
		bbi = bb->getInterface(NeighborDirection::Direction(i));
		if ((bbi->connectedInterface) && (bbi != parent)) {
			rva1 = (rand()/(double)RAND_MAX) * (1500-0) + 0; // random variable between each CLOCK_SYNC message sent
												// between 0 and 1500 us
			ST_go_message * message = new ST_go_message(level);
			getScheduler()->schedule(new NetworkInterfaceEnqueueOutgoingEvent(getScheduler()->now() + rva1 + rva2,  message, bbi));
			rva2 += (rand()/(double)RAND_MAX) * (10-0) + 0; // Simulate loop processing
			sent++;
		}
	}
	return sent;
}

void BlinkyBlocksBlockCode::ST_send_back(bool a, P2PNetworkInterface *bbi, unsigned int l) {
	uint64_t processing_time = 0;
	ST_back_message * message = new ST_back_message(a, l);
	
	getScheduler()->schedule(new NetworkInterfaceEnqueueOutgoingEvent(getScheduler()->now() + processing_time, message, bbi));
}

//======================================================================
//
//          Clock Synchronization
//======================================================================

void BlinkyBlocksBlockCode::initClockSync() {	
	// CLOCK SYNC LEADER ELECTION:
	initTimeLeaderElection();
}

bool BlinkyBlocksBlockCode::belongsTo(list<P2PNetworkInterface*> l, P2PNetworkInterface *bbi) {
	 for (list<P2PNetworkInterface*>::iterator it= l.begin(); it != l.end(); it++) {
		if (*it == bbi) {
			return true;
		}
	}
	return false;
}

/*
void BlinkyBlocksBlock::synchronizeNeighborClocks(uint8_t waveId, uint8_t nbhops) {
	BlinkyBlocksBlock *bb = (BlinkyBlocksBlock*) hostBlock;
	stringstream info;
	uint64_t rva1 = 0;
	uint64_t rva2 = 0;
	
	info << "Synchronize neighbor clocks";

	//spreadMessage(msg, clockSyncChildren);
	
	for (list<P2PNetworkInterface *>::iterator it= clockSyncChildren.begin(); it != clockSyncChildren.end(); it++) {
		P2PNetworkInterface *bbi = *it;
		if (bbi->connectedInterface) {
			rva1 = (rand()/(double)RAND_MAX) * (1500-0) + 0; // random variable between each CLOCK_SYNC message sent
												// between 0 and 1500 us
			BlinkyBlocks::getScheduler()->schedule(new NetworkInterfaceEnqueueOutgoingEvent(BaseSimulator::getScheduler()->now() + rva1 + rva2,new BlinkyBlocksClockSyncInfoMsg(bb->localClock.getClockMS(), waveId, nbhops), bbi));
			rva2 += (rand()/(double)RAND_MAX) * (10-0) + 0;
		}
	}	
	BlinkyBlocks::getScheduler()->trace(info.str(), bb->blockId);
}

void BlinkyBlocksBlock::launchSynchronizationWave(uint64_t t) {
	bb->localClock.lastWaveId++;
	BlinkyBlocks::getScheduler()->schedule(new SynchronizeNeighborClocksEvent(t, bb, localClock.lastWaveId, 0));
}*/

void BlinkyBlocksBlockCode::synchronizeNeighborClocks(uint8_t nbhops) {
	BlinkyBlocksBlock *bb = (BlinkyBlocksBlock*) hostBlock;
	stringstream info;
	uint64_t rva1 = 0;
	uint64_t rva2 = 0;
	static unsigned int n = 1;
	info << "Synchronize neighbor clocks";

	//spreadMessage(msg, clockSyncChildren);
	
	for (list<P2PNetworkInterface *>::iterator it= clockSyncChildren.begin(); it != clockSyncChildren.end(); it++) {
		P2PNetworkInterface *bbi = *it;
		if (bbi->connectedInterface) {
			//rva1 = (rand()/(double)RAND_MAX) * (50-0) + 0; // random variable between each CLOCK_SYNC message sent
												// between 0 and 1500 us
			getScheduler()->schedule(new NetworkInterfaceEnqueueOutgoingEvent(getScheduler()->now() + rva1 + rva2,new BlinkyBlocksClockSyncInfoMsg(bb->localClock.getEstimatedGlobalClockMS(), nbhops), bbi));
			rva2 += (rand()/(double)RAND_MAX) * (20-0) + 0;
		}
	}	
	getScheduler()->trace(info.str(), bb->blockId);
	if (timeLeader) {
		if (n < NB_SYNC) {
			launchSynchronizationWave(getScheduler()->now() + 1*1000 + SYNC_PERIOD*1000);
			n++;
		}
	}
}

void BlinkyBlocksBlockCode::launchSynchronizationWave(uint64_t t) {
	BlinkyBlocksBlock *bb = (BlinkyBlocksBlock*) hostBlock;
	getScheduler()->schedule(new SynchronizeNeighborClocksEvent(t, bb, 0));
}

//======================================================================
//
//         	System Message Handler (Leader Election, Clock Synchronization...)
//======================================================================

void BlinkyBlocksBlockCode::printChildren() {
	cout << "@" << hostBlock->blockId << ": ";
	for (list<P2PNetworkInterface*>::iterator it= clockSyncChildren.begin(); it != clockSyncChildren.end(); it++) {
		cout << (*it)->connectedInterface->hostBlock->blockId << ", ";
	}
	cout << endl;
}

bool BlinkyBlocksBlockCode::handleSystemMessage(EventPtr pev) {
	stringstream info;
	BlinkyBlocksBlock *bb = (BlinkyBlocksBlock*) hostBlock;
	unsigned int processingTime = 0;
	bool handled = false;
	
	info << "";
	switch (pev->eventType) {
		case EVENT_RECEIVE_MESSAGE: /*EVENT_NI_RECEIVE: */ {
			MessagePtr message = (boost::static_pointer_cast<NetworkInterfaceReceiveEvent>(pev))->message;
			switch(message->type) {
				case BB_CS_LE_MIN_ID_ELECTION_MESSAGE: {
					BlinkyBlocksLeaderElectionMinIdElectionMsg_ptr recv = boost::static_pointer_cast<BlinkyBlocksLeaderElectionMinIdElectionMsg>(message);
					/*if (!electing) {
						minId = bb->blockId;
						// election!
					}*/
					if(recv->id == minId ) {
							BlinkyBlocksLeaderElectionMinIdBackMsg *back = new BlinkyBlocksLeaderElectionMinIdBackMsg(minId, false); 
							getScheduler()->schedule(new NetworkInterfaceEnqueueOutgoingEvent(getScheduler()->now() + processingTime, back, recv->destinationInterface));
					}
					if (recv->id < minId) {
						minId = recv->id;
						minIdSender = recv->destinationInterface;
						clockSyncChildren.clear();
						//BlinkyBlocksLeaderElectionMinIdElectionMsg *election = new BlinkyBlocksLeaderElectionMinIdElectionMsg(minId);
						nbNeededAnswers = broadcastElectionMessage();
						if (nbNeededAnswers == 0) {
							if (minId == (unsigned int) bb->blockId) {
								// TIME MASTER ELECTED!
								timeLeaderSubRoutine();
								//printChildren();
								launchSynchronizationWave(getScheduler()->now() + 2*1000); // now + 2ms
							} else {
								BlinkyBlocksLeaderElectionMinIdBackMsg *back = new BlinkyBlocksLeaderElectionMinIdBackMsg(minId, true); 
								getScheduler()->schedule(new NetworkInterfaceEnqueueOutgoingEvent(getScheduler()->now() + processingTime, back, minIdSender));
							}
						}
					}
					handled = true;
					info << "Leader Election message received at face " << NeighborDirection::getString(bb->getDirection(message->sourceInterface->connectedInterface)) << " from " << message->sourceInterface->hostBlock->blockId;
					break;
				}
				case BB_CS_LE_MIN_ID_BACK_MESSAGE: {
					BlinkyBlocksLeaderElectionMinIdBackMsg_ptr recv = boost::static_pointer_cast<BlinkyBlocksLeaderElectionMinIdBackMsg>(message);
					if (minId == recv->id) {
						nbNeededAnswers--;
						if (recv->answer == true) {
							clockSyncChildren.push_back(recv->destinationInterface);
						}
						if (nbNeededAnswers == 0) {
							//printChildren();
							if (minId == (unsigned int) bb->blockId) {
								// TIME MASTER ELECTED!
								timeLeaderSubRoutine();
								launchSynchronizationWave(getScheduler()->now() + 2*1000); // now + 2ms
							} else {
								BlinkyBlocksLeaderElectionMinIdBackMsg *back = new BlinkyBlocksLeaderElectionMinIdBackMsg(minId, true); 
								getScheduler()->schedule(new NetworkInterfaceEnqueueOutgoingEvent(getScheduler()->now() + processingTime, back, minIdSender));
							}
						}
					}	
					handled = true;
					break;
				}
				case BB_CS_LE_BAR_BLOCK_DISTANCE_MESSAGE: {
					BlinkyBlocksLEBarBlockDistanceMsg_ptr recv = boost::static_pointer_cast<BlinkyBlocksLEBarBlockDistanceMsg>(message);
					if (recv->id != (unsigned int) bb->blockId) {
						if ((distances[recv->id-1] == 0) || (recv->distance < distances[recv->id-1])) {
							distances[recv->id-1] = recv->distance;
							broadcastDistanceMessage(recv->id, recv->distance+1, recv->destinationInterface);
						}
					}
					if (barycentricLETimeoutEv != NULL) {
						barycentricLETimeoutEv->cancel();
					}
					barycentricLETimeoutEv = new BarycentricLeaderElectionTimeoutEvent(getScheduler()->now() + BARYCENTRIC_LEADER_ELECTION_TIMEOUT, bb);
					getScheduler()->schedule(barycentricLETimeoutEv);
					handled = true;
					break;
				}
				case BB_CS_LE_BAR_SUM_REQUEST_MESSAGE: {
					/*unsigned int sum = 0;
					
					for (int i = 0; i < distances.size(); i++) {
						if ((distances[i] == 0) && (bb->blockId != i)) {
							cerr << "Warning Barycentric Leader Election may be wrong..." << endl;
						}
						sum += distances[i];
					}
					BlinkyBlocksLEBarSumInfoMsg *sumMsg = new BlinkyBlocksLEBarSumInfoMsg(sum); 
					getScheduler()->schedule(new NetworkInterfaceEnqueueOutgoingEvent(getScheduler()->now() + processingTime, sumMsg, recv->destinationInterface));*/
					handled = true;
					break;
				}
				case BB_CS_LE_BAR_SUM_INFO_MESSAGE: {
					BlinkyBlocksLEBarSumInfoMsg_ptr recv = boost::static_pointer_cast<BlinkyBlocksLEBarSumInfoMsg>(message);
					list<pair<unsigned int, unsigned int> >::iterator it;
					unsigned int id = message->sourceInterface->hostBlock->blockId;
					
					for ( it = sums.begin(); it != sums.end(); it++) {
						if (id  == it->first) {
							it->second = recv->sum;
						}
					}
					if (it == sums.end()) {
						sums.push_back(make_pair(id ,recv->sum));
					}
					barycentricLEtryToElect();
					handled = true;
					break;
				}
				case BB_CS_ST_GO_MESSAGE: {
					ST_go_message_ptr recv = boost::static_pointer_cast<ST_go_message>(message);
					
					//cout << "@" << bb->blockId << ": GO MSG" << endl;
					if (!timeLeader) {
						if ( (parent == NULL) || (level > recv->level + 1) ) {
							parent = recv->destinationInterface;
							clockSyncChildren.clear();
							level = recv->level + 1;
							nbNeededAnswers = ST_launch_go();
							//cout << "LAUNCH GO (neededAnswers="<< nbNeededAnswers << endl;
							if (nbNeededAnswers == 0) {
								ST_send_back(true, parent, level);
							}
						} else {
							ST_send_back(false, recv->destinationInterface, recv->level + 1);
						}
					}
				}
				break;
				case BB_CS_ST_BACK_MESSAGE: {
					ST_back_message_ptr recv = boost::static_pointer_cast<ST_back_message>(message);
					//cout << "@" << bb->blockId << ": BACK MSG (nbAnswersNeeded=" << nbNeededAnswers << ", level= " << level << "recv->level= " << recv->level << ", answer= " << recv->answer << ")" << endl;
					
					if(recv->level == (level + 1)) {
						if (recv->answer == true) {
							clockSyncChildren.push_back(recv->destinationInterface);
						}
						nbNeededAnswers--;
						if (nbNeededAnswers == 0) {
							if (parent == NULL) { // MASTER, SP BUILT								
								cout << "spanning tree is built!" << endl;
								// Clock synchronization
								//clock_launch_sync();
								launchSynchronizationWave(getScheduler()->now() + 2*1000); // now + 2ms
							} else {
								ST_send_back(true, parent, level);
							}
						}
					}
				}
				break;			
				case BB_CS_CLOCK_REQUEST_MESSAGE: {
					handled = true;
					break;
				}
				case BB_CS_CLOCK_INFO_MESSAGE: {
					if (bb->localClock.handleSyncMsg(message)) {
						// set a change color event at time 3s = 3 000 000 us
						if (bb->localClock.nbSync == NB_SYNC) {
							uint64_t t = bb->localClock.getSchedulerTimeForLocalClockMS(30000);
							getScheduler()->schedule(new VMSetColorEvent(t, (BlinkyBlocksBlock*) hostBlock, 0.5,0.0,0.2,1.0));
						}
						info << "sync ";
					}
					handled = true;
					break;
				}
				default:
					handled = false;
			}
			break;
		}
		default:
			handled = false;
	}
	if (info.str() != "" ) {
			BlinkyBlocks::getScheduler()->trace(info.str(),hostBlock->blockId);
	}
	return handled;
}

}
