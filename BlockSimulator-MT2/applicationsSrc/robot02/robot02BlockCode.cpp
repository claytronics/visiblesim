/*
 * Robot02BlockCode.cpp
 *
 *  Created on: 12 avril 2013
 *      Author: ben
 */

#include <iostream>
#include <sstream>
#include "robot02BlockCode.h"
#include "scheduler.h"
#include "robotBlocksEvents.h"
#include <boost/shared_ptr.hpp>


Robot02BlockCode::Robot02BlockCode(RobotBlocksBlock *host):RobotBlocksBlockCode(host) {
	cout << "Robot02BlockCode constructor" << endl;
	scheduler = RobotBlocks::getScheduler();
	block = (RobotBlocksBlock*)hostBlock;
// initialize object deleted in destructor
}

Robot02BlockCode::~Robot02BlockCode() {
	cout << "Robot02BlockCode destructor" << endl;
}

void Robot02BlockCode::startup() {
	stringstream info;
	info << "start #" << block->blockId;
    masterId = block->blockId;
    searchDone=false;
    masterColor = block->color;
    block2answer=NULL;
    nbreOfWaitedAnswers=0;
    colored=false;
    //scheduler->schedule(new BlinkyBlocks::SetColorEvent(scheduler->now(),block,255,255,255,0));
    block->setColor(WHITE);
    if (block->blockId==1) {
      //scheduler->schedule(new BlinkyBlocks::SetColorEvent(scheduler->now(),block,0,0,255,0));
        block->setColor(BLUE);
        sendMasterMessageToAllNeighbors(NULL);
    }
}

void Robot02BlockCode::processLocalEvent(EventPtr pev) {
	MessagePtr message;
    stringstream info;

    if (pev->eventType == EVENT_RECEIVE_MESSAGE) {
        message = (boost::static_pointer_cast<NetworkInterfaceReceiveEvent>(pev))->message;
        
		cout << "INTERFACE" << block->getDirection(message->destinationInterface) << endl;

        switch (message->type) {
            case SEARCH_MASTER_MESSAGE : {
                SearchMasterMessage_ptr recvMessage = boost::static_pointer_cast<SearchMasterMessage>(message);
                if (masterId<recvMessage->blockId) {
                    masterColor = recvMessage->blockColor;
                    masterId = recvMessage->blockId;
                }
                stringstream info;
                info.str("");
                info << block->blockId << " recv(" << masterId << ")";
                scheduler->trace(info.str(),block->blockId);

                if (searchDone) {
                    sendReturnMessageTo(recvMessage->destinationInterface);
                } else {
                    searchDone=true;
                    block2answer=recvMessage->destinationInterface;
                    sendMasterMessageToAllNeighbors(block2answer);
                    block->setColor(MAGENTA);
                    //scheduler->schedule(new BlinkyBlocks::SetColorEvent(scheduler->now(),block,255,0,255,0));

                    if (nbreOfWaitedAnswers==0) {

                        stringstream info;
                        info.str("");
                        info << block->blockId << " Find(" << masterId << ")";
                        scheduler->trace(info.str(),block->blockId);

                        if (block2answer!=NULL) {
                            sendReturnMessageTo(block2answer);
                            block2answer=NULL;
                            block->setColor(CYAN);
                            //scheduler->schedule(new BlinkyBlocks::SetColorEvent(scheduler->now(),block,0,255,255,0));
                        } else {
                            //scheduler->schedule(new BlinkyBlocks::SetColorEvent(scheduler->now(),block,255,0,0,0));
                            block->setColor(RED);
                            sendColorMessageToAllNeighbors(NULL);
                        }
                    }
                }
            }
            break;

            case RETURN_MASTER_MESSAGE : {
                ReturnMasterMessage_ptr recvMessage = boost::static_pointer_cast<ReturnMasterMessage>(message);
                if (masterId<recvMessage->blockId) {
                    masterColor = recvMessage->blockColor;
                    masterId = recvMessage->blockId;
                }

                stringstream info;
                info.str("");
                info << block->blockId << " recv(" << masterId << ")";
                scheduler->trace(info.str(),block->blockId);

                nbreOfWaitedAnswers--;
                if (nbreOfWaitedAnswers==0) {

                    stringstream info;
                    info.str("");
                    info << block->blockId << " Find(" << masterId << ")";
                    scheduler->trace(info.str(),block->blockId);

                    if (block2answer!=NULL) {
                        sendReturnMessageTo(block2answer);
                        block2answer=NULL;
                        block->setColor(masterColor);
                        //scheduler->schedule(new BlinkyBlocks::SetColorEvent(scheduler->now(),block,0,255,255,0));
                    } else {
                        //scheduler->schedule(new BlinkyBlocks::SetColorEvent(scheduler->now(),block,255,0,0,0));
                        block->setColor(RED);
                        sendColorMessageToAllNeighbors(NULL);
                    }
                }
            }
            break;
            case COLOR_MESSAGE : {
                ColorMessage_ptr recvMessage = boost::static_pointer_cast<ColorMessage>(message);
                masterColor = recvMessage->color;
                if (!colored) {
                    colored = true;

                    //scheduler->schedule(new BlinkyBlocks::SetColorEvent(scheduler->now(),block,0,recvMessage->color[0],recvMessage->color[1],recvMessage->color[2]));
                    block->setColor(masterColor);
                    sendColorMessageToAllNeighbors(message->destinationInterface);
                }
            }
			break;
        }
	}
}

RobotBlocks::RobotBlocksBlockCode* Robot02BlockCode::buildNewBlockCode(RobotBlocksBlock *host) {
	return(new Robot02BlockCode(host));
}

void Robot02BlockCode::sendMasterMessageToAllNeighbors(P2PNetworkInterface *except) {
    stringstream info;
    info.str("");
    info << block->blockId << " Search(" << masterId << ")";
    scheduler->trace(info.str(),block->blockId);

    P2PNetworkInterface * p2p;
    nbreOfWaitedAnswers = 0;
    searchDone=true;

    for(int i = 0; i<6; i++) {
        p2p = block->getInterface((RobotBlocks::NeighborDirection::Direction)i);
        if (p2p->connectedInterface && p2p->connectedInterface!=except) {
            nbreOfWaitedAnswers++;
            SearchMasterMessage *message = new SearchMasterMessage(masterId,masterColor);
            scheduler->schedule(new NetworkInterfaceEnqueueOutgoingEvent(scheduler->now()+COM_DELAY, message, p2p));
        }
    }
}

void Robot02BlockCode::sendReturnMessageTo(P2PNetworkInterface *p2p) {
    ReturnMasterMessage *message = new ReturnMasterMessage(masterId,masterColor);
    scheduler->schedule(new NetworkInterfaceEnqueueOutgoingEvent(scheduler->now()+COM_DELAY, message, p2p));
}

void Robot02BlockCode::sendColorMessageToAllNeighbors(P2PNetworkInterface *except) {
    P2PNetworkInterface * p2p;

    for(int i = 0; i<6; i++) {
        p2p = block->getInterface((RobotBlocks::NeighborDirection::Direction)i);
        if (p2p->connectedInterface && p2p->connectedInterface!=except) {
            ColorMessage *message = new ColorMessage(masterColor);
            scheduler->schedule(new NetworkInterfaceEnqueueOutgoingEvent(scheduler->now()+COM_DELAY, message, p2p));
        }
    }
}
