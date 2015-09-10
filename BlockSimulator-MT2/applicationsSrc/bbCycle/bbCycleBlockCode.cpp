/*
 * BbCycleBlockCode.cpp
 *
 *  Created on: 26 mars 2013
 *      Author: dom
 */

#include <iostream>
#include <sstream>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <stdio.h> 
#include "scheduler.h"
#include "network.h"
#include "bbCycleBlockCode.h"
#include "bbCycleEvents.h"
#include "trace.h"

using namespace std;
using namespace BlinkyBlocks;

#define SYNC_PERIOD (2*1000*1000)
#define COLOR_CHANGE_PERIOD_USEC (2*1000*1000)

BbCycleBlockCode::BbCycleBlockCode(BlinkyBlocksBlock *host): BlinkyBlocksBlockCode(host) {
	OUTPUT << "BbCycleBlockCode constructor" << endl;
}

BbCycleBlockCode::~BbCycleBlockCode() {
	OUTPUT << "BbCycleBlockCode destructor" << endl;
}

void BbCycleBlockCode::init() {
	BlinkyBlocksBlock *bb = (BlinkyBlocksBlock*) hostBlock;
	stringstream info;	
	Color c = PINK;
	block2Answer=NULL;
	cycle=true;
	if(hostBlock->blockId==1){
		idMessage=0;	
		BlinkyBlocks::getScheduler()->schedule(new SynchronizeEvent(BlinkyBlocks::getScheduler()->now()+SYNC_PERIOD,hostBlock));	
		info << "This block is the Master Block" << endl;
	}
	BlinkyBlocks::getScheduler()->schedule(new SetColorEvent(COLOR_CHANGE_PERIOD_USEC,bb,c));
	BlinkyBlocks::getScheduler()->trace(info.str(),hostBlock->blockId);
}

void BbCycleBlockCode::startup() {
	stringstream info;
	delay=0;
	info << "  Starting BbCycleBlockCode in block " << hostBlock->blockId;
	init();
}

void BbCycleBlockCode::processLocalEvent(EventPtr pev) {
	stringstream info;
	MessagePtr message;
	BlinkyBlocksBlock *bb = (BlinkyBlocksBlock*) hostBlock;
	info.str("");
	
	OUTPUT << bb->blockId << " processLocalEvent: date: "<< BaseSimulator::getScheduler()->now() << " process event " << pev->getEventName() << "(" << pev->eventType << ")" << ", random number : " << pev->randomNumber << endl;
	
	switch (pev->eventType) {
		case EVENT_SET_COLOR:
			{
			Color color = (boost::static_pointer_cast<SetColorEvent>(pev))->color;
			bb->setColor(color);
			info << "set color "<< color << endl;
			if (cycle){
				color = BLUE;
				cycle = false;
			}
			else{
				color = RED;
				cycle = true;
			}
			BlinkyBlocks::getScheduler()->schedule(new SetColorEvent(bb->getTime()+COLOR_CHANGE_PERIOD_USEC+delay,bb,color));
			info << "Setcolor scheduled" << endl;
			}
			break;
		//case EVENT_PLAY_NOTE:
		case EVENT_NI_RECEIVE:
			{
			message = (boost::static_pointer_cast<NetworkInterfaceReceiveEvent>(pev))->message; 
			P2PNetworkInterface * recvInterface = message->destinationInterface;
			switch(message->id){
				case SYNC_MSG_ID: 
					{
					SynchroMessage_ptr recvMessage = boost::static_pointer_cast<SynchroMessage>(message);
					if (!received[recvMessage->idSync]){
						received[recvMessage->idSync]=true;
						block2Answer=recvInterface;
						sendClockToNeighbors(block2Answer,recvMessage->nbhop+1,recvMessage->time,recvMessage->idSync); 	
						//if ((recvMessage->time + 6000*recvMessage->nbhop) >= bb->getTime())

						delay = recvMessage->time - bb->getTime() + 6000*recvMessage->nbhop;  

						/*else if ((recvMessage->time + 6000*recvMessage->nbhop) < bb->getTime()){
							info << "Paused for : " << ((bb->getTime()-(recvMessage->time+6000*recvMessage->nbhop))-BlinkyBlocks::getScheduler()->now()) << endl;
							BlinkyBlocks::getScheduler()->trace(info.str(),hostBlock->blockId);
							while (bb->getTime() > recvMessage->time+6000*recvMessage->nbhop);
							//while(BlinkyBlocks::getScheduler()->now() < (bb->getTime()-(recvMessage->time + 60000*recvMessage->nbhop)));
							//bb->pauseClock((bb->getTime()-(recvMessage->time+6000*recvMessage->nbhop)),BlinkyBlocks::getScheduler()->now()); 
						}*/
						info<<"synchronized with delay : "<< delay << endl;
						}
					}
					break;					
				default:
					break;
				}
			}
			break;
		case EVENT_SYNC:
			{
			received[idMessage]=true;
			sendClockToNeighbors(NULL,1,bb->getTime(),idMessage);
			idMessage++;
			uint64_t nextSync = bb->getTime()+SYNC_PERIOD;
			BlinkyBlocks::getScheduler()->schedule(new SynchronizeEvent(nextSync,bb));
			info << "scheduled synchro" << endl;
			}
			break;
		default:
			ERRPUT << "*** ERROR *** : unknown local event" << endl;
			break;
		}
		BlinkyBlocks::getScheduler()->trace(info.str(),hostBlock->blockId);
}

BlinkyBlocks::BlinkyBlocksBlockCode* BbCycleBlockCode::buildNewBlockCode(BlinkyBlocksBlock *host) {
	return(new BbCycleBlockCode(host));
}

void BbCycleBlockCode::sendClockToNeighbors (P2PNetworkInterface *p2pExcept, int hop, uint64_t clock, int id){
	P2PNetworkInterface * p2p;
	BlinkyBlocksBlock *bb = (BlinkyBlocksBlock*) hostBlock;
	
	for (int i=0; i<6 ; i++) {
	p2p = bb->getInterface(NeighborDirection::Direction(i));
		if (p2p->connectedInterface && p2p!=p2pExcept){	
			SynchroMessage *message = new SynchroMessage(clock, hop, id);
			BlinkyBlocks::getScheduler()->schedule(new NetworkInterfaceEnqueueOutgoingEvent (BlinkyBlocks::getScheduler()->now(), message, p2p));
		}
	}
}

SynchroMessage::SynchroMessage(uint64_t t, int hop, int ids) :Message(){
	id = SYNC_MSG_ID;
	idSync=ids;
	time = t;
	nbhop = hop;
}

SynchroMessage::~SynchroMessage(){
}
