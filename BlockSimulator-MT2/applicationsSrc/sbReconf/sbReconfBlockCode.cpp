/*
 * SbReconfBlockCode.cpp
 *
 *  Created on: 12 avril 2013
 *      Author: ben
 */

#include <iostream>
#include <sstream>
#include "sbReconfBlockCode.h"
#include "scheduler.h"
#include "smartBlocksEvents.h"
#include <boost/shared_ptr.hpp>

/*************************
STEP 0: get map
STEP 1: recieves all ackmap from neighbors
STEP 2: recieves searchHeadOfTrainMessage
STEP 3: recieves headFoundMessage

**************************/

#define verbose

using namespace std;
using namespace SmartBlocks;

const int time_offset=100;

SbReconfBlockCode::SbReconfBlockCode(SmartBlocksBlock *host):SmartBlocksBlockCode(host) {
	cout << "SbReconfBlockCode constructor" << endl;
	scheduler = SmartBlocks::getScheduler();
	smartBlock = (SmartBlocksBlock*)hostBlock;
}

SbReconfBlockCode::~SbReconfBlockCode() {
	cout << "SbReconfBlockCode destructor" << endl;
	delete [] targetGrid;
	delete [] unlockPathTab;
}

void SbReconfBlockCode::startup() {
	stringstream info;
	block = (SmartBlocksBlock*)(hostBlock);
	wrl = SmartBlocksWorld::getWorld();
	info << "Starting ";

	nbreOfWaitedAnswers=0;
    block2Answer = NULL;
    _next = NULL;
    _previous = NULL;
	block->_isBorder=false;
	_isTrain=false;
	_isHead=false;
	_isEnd=false;
	_motionDir.set(0,0);
	//_currentStage=1;
	_currentMove=0;
	possibleRules=NULL;
    unlockPathTab=NULL;
	unlockPathTabSize=0;
	unlockMotionStep=0;

    // initialise la liste des étapes
    for (int i=0; i<4; i++) {
		tabSteps[i] = false;
	}
	//If i am master block
	if(block->blockId == 1)
	{ posGrid.x = block->position[0];
	  posGrid.y = block->position[1];
	  presence *tab = wrl->getTargetGridPtr(gridSize);
#ifdef verbose
      scheduler->trace(info.str(),block->blockId);
#endif
	  targetGrid = new presence[gridSize[0]*gridSize[1]];
	  memcpy(targetGrid,tab,gridSize[0]*gridSize[1]*sizeof(presence));
	  block->wellPlaced = targetGrid[posGrid.y*gridSize[0]+posGrid.x]==fullCell;
	  tabSteps[0] = true;

	  // compte le nombre de cellules pleines
	  int n=gridSize[0]*gridSize[1];
	  _nbreGoalCells=0;
	  presence *ptr=tab;
	  while (n--) {
        _nbreGoalCells+=(*ptr==fullCell);
        ptr++;
	  }

	  sendMapToNeighbors(NULL);
	} else {
		targetGrid=NULL;
#ifdef verbose
		scheduler->trace(info.str(),block->blockId);
#endif
	}

	wrl->addStat(0,0);
    wrl->addStat(1,0);
    wrl->addStat(2,0);
    wrl->printStats();
}

/** Prepare unlock table for next unlock action **/
void SbReconfBlockCode::prepareUnlock(const vector<short>&path,int step) {
	vector<short>::const_iterator cs=path.begin();
	unlockPathTabSize = path.size();
	if (unlockPathTabSize) {
		delete [] unlockPathTab;
		unlockPathTab = new short[unlockPathTabSize];
		int i=0;
		while (cs!=path.end()) {
			unlockPathTab[i++] = *cs;
			cs++;
		}
	}
	unlockMotionStep = step;
}

void SbReconfBlockCode::printRules() {
#ifdef verbose
    /* affiche les règles*/
    stringstream info;
    info.str("");
    if (!possibleRules || possibleRules->empty()) {
        info << "no rule";
    } else {
        if (_isHead) info << "H";
        if (_isEnd) info << "E";
        info << possibleRules->size();
        std::reverse_iterator<vector<Validation*>::iterator> rev_until (possibleRules->begin());
        std::reverse_iterator<vector<Validation*>::iterator> rev_from (possibleRules->end());
        while (rev_from!=rev_until) {
            info  << "/" << (*rev_from)->gain << (*rev_from)->capa->name;
            rev_from++;
        }
    }
    scheduler->trace(info.str(),hostBlock->blockId,GOLD);
#endif
}

void SbReconfBlockCode::applyRules() {
	// supprime l'ancienne liste de règles
	if (possibleRules) {
		while (!possibleRules->empty()) {
			delete (possibleRules->back());
			possibleRules->pop_back();
		}
		delete possibleRules;
		possibleRules=NULL;
	}

// apply rules
    posGrid.x = block->position[0];
    posGrid.y = block->position[1];
    wrl->getPresenceMatrix(posGrid,_pm);
    PresenceMatrix localTargetGrid;
    getLocalTargetGrid(posGrid,localTargetGrid);
    //OUTPUT << localTargetGrid;
    SmartBlocks::PointCel neighborsDirection[4];
    SbReconfBlockCode*bc;
    P2PNetworkInterface *ni;
    /*for (int i=0; i<4; i++) {
        ni = block->getInterface(NeighborDirection(i));
        if (ni->connectedInterface!=NULL) {
            bc = (SbReconfBlockCode*)(ni->connectedInterface->hostBlock->blockCode);
            neighborsDirection[i]=bc->_motionDir;
        } else {
            neighborsDirection[i].set(0,0);
        }
    }*/
    //OUTPUT << "_previous =" << (_previous?_previous->connectedInterface->hostBlock->blockId:-1) << endl;

    for (int i=0; i<4; i++) {
        ni = block->getInterface(NeighborDirection(i));
        if (_previous && _previous==ni) {
            //OUTPUT << "previous=" << i << endl;
            bc = (SbReconfBlockCode*)(ni->connectedInterface->hostBlock->blockCode);
            neighborsDirection[i]=bc->_motionDir;
        } else {
            if (ni->connectedInterface!=NULL && ((SmartBlocksBlock*)(ni->connectedInterface->hostBlock))->wellPlaced) {
                neighborsDirection[i].set(0,0);
            } else {
                neighborsDirection[i].unSet();
            }
        }
    }
    possibleRules = wrl->getCapabilities()->validateDirection(_pm,localTargetGrid,neighborsDirection);
    if (possibleRules && !possibleRules->empty()) {
        _motionDir = possibleRules->back()->capa->tabMotions[0]->vect;
        OUTPUT << "_motionDir =" << _motionDir << endl;
    }
    printRules();
    setRulesColor();
}

void SbReconfBlockCode::setRulesColor() {
    if (block->_isBorder) {
        if (_isTrain) {
            block->setColor(_isHead?RED:_isEnd?GOLD:ORANGE);
        } else {
            block->setColor(_isHead?RED:_isEnd?GREY:ORANGE);
        }

    } else {
        block->setColor(_isHead?RED:block->wellPlaced?YELLOW:GREEN);
    }
}

void SbReconfBlockCode::startMotion(uint64_t t,const SmartBlocks::PointCel &mv,int step,const vector<short>&path) {
	prepareUnlock(path,step);
	Vecteur finalPosition;
	finalPosition.set(block->position.pt[0]+mv.x,block->position.pt[1]+mv.y,0);
	scheduler->schedule(new MotionStartEvent(t,block,finalPosition));
#ifdef verbose
	stringstream info;
	info.str("");
	info << "MotionStartEvent(" << t << ") vect=" << mv << "  unlock=" << path.size() << " step=" << step;
	scheduler->trace(info.str(),block->blockId,LIGHTGREY);
#endif
}

void SbReconfBlockCode::processLocalEvent(EventPtr pev) {
	stringstream info;
	MessagePtr message;

	switch (pev->eventType) {
		case EVENT_MOTION_END: {
#ifdef verbose
			info.str("");
			info << "rec.: EVENT_MOTION_END, tabUnlockPathSize=" << unlockPathTabSize << " order=" << unlockMotionStep;
			scheduler->trace(info.str(),hostBlock->blockId);
#endif
            posGrid.x = block->position[0];
            posGrid.y = block->position[1];
            wrl->getPresenceMatrix(posGrid,_pm);
	        block->wellPlaced = targetGrid[posGrid.y*gridSize[0]+posGrid.x]==fullCell;

            SmartBlocksWorld *wrl = SmartBlocks::getWorld();
            wrl->addStat(2,1);
            if (_isEnd) { // c'est une fin de train (et de ligne)
                wrl->addStat(0,1);
            }
            if (unlockPathTabSize>0) { // c'est une fin de ligne (sauf train)
                wrl->addStat(1,1);
                wrl->printStats();
            }
            //wrl->printStats();

            _isTrain=false;
			 // prepare for next motion
			if (unlockPathTabSize>0) {

				// envoie le message de déblocage
				P2PNetworkInterface *p2p = block->getInterface(NeighborDirection(unlockPathTab[0]));
				UnlockMessage *message = new UnlockMessage(unlockPathTab+1,unlockPathTabSize-1,unlockMotionStep);
				scheduler->schedule(new NetworkInterfaceEnqueueOutgoingEvent(scheduler->now() + time_offset, message, p2p));
#ifdef verbose
				info.str("");
				info << "send UnlockMessage("<< unlockMotionStep << ") to " << p2p->connectedInterface->hostBlock->blockId;
				scheduler->trace(info.str(),hostBlock->blockId);
#endif
			} else {
/***************************************************************
 si c'est le dernier du train et qu'il a fini son déplacement
 on reconnecte le train !
 Si il existe une règle de fin il reste la fin du train suivant
 ***************************************************************/
                setRulesColor();
                if (_isEnd) {
                    sendAsk4EndToNeighbors(NULL);
                }

			}
		}
		break;

		case EVENT_NI_RECEIVE:
			message = (boost::static_pointer_cast<NetworkInterfaceReceiveEvent>(pev))->message;
			P2PNetworkInterface * recvInterface = message->destinationInterface;
			switch(message->id) {
				case MAP_MSG_ID : {
					MapMessage_ptr recvMessage = boost::static_pointer_cast<MapMessage>(message);
#ifdef verbose
					unsigned int sourceId = recvMessage->sourceInterface->hostBlock->blockId;
					info.str("");
					info << hostBlock->blockId << " rec. MapMessage : " << recvMessage->posx << "," << recvMessage->posy << " from " << sourceId;
					scheduler->trace(info.str(),hostBlock->blockId);
#endif
					if (targetGrid) {
						sendAckMap(recvInterface);
					} else {
						tabSteps[0] = true;
						posGrid.x = recvMessage->posx;
						posGrid.y = recvMessage->posy;
						gridSize[0] = recvMessage->gridw;
						gridSize[1] = recvMessage->gridh;
						_nbreGoalCells = recvMessage->nbreGoalCells;
						targetGrid = new presence[gridSize[0]*gridSize[1]];
						memcpy(targetGrid,recvMessage->targetGrid,gridSize[0]*gridSize[1]*sizeof(presence));

						block->wellPlaced = targetGrid[posGrid.y*gridSize[0]+posGrid.x]==fullCell;
						//block->setDisplayedValue(-1);
						setRulesColor();
						block2Answer=recvInterface;
						sendMapToNeighbors(block2Answer);
#ifdef verbose
						info.str("");
						info << "TargetState(" << posGrid.x << "," << posGrid.y << ")  " << block->wellPlaced;
						scheduler->trace(info.str(),hostBlock->blockId);
#endif
					}
					if (nbreOfWaitedAnswers==0) {
						if (block2Answer!=NULL) {
							sendAckMap(block2Answer);
							block2Answer=NULL;
#ifdef verbose
							info.str("");
							info << " READY";
							scheduler->trace(info.str(),hostBlock->blockId);
#endif
						} else {
#ifdef verbose
							info.str("");
							info << "Master READY";
							scheduler->trace(info.str(),hostBlock->blockId);
#endif
							setRulesColor();
							//block->setDisplayedValue(-1);

						}
						tabSteps[1] = true;
						createBorder();
					}
				}
				break;

				case ACKMAP_MSG_ID : {
					AckMapMessage_ptr recvMessage = boost::static_pointer_cast<AckMapMessage>(message);
#ifdef verbose
					unsigned int sourceId = recvMessage->sourceInterface->hostBlock->blockId;
					info.str("");
					info << "rec. AckMapMessage(" << nbreOfWaitedAnswers << ") from " << sourceId;
					scheduler->trace(info.str(),hostBlock->blockId);
#endif
					nbreOfWaitedAnswers--;
					if (nbreOfWaitedAnswers==0) {
						if (block2Answer!=NULL) {
							sendAckMap(block2Answer);
							block2Answer=NULL;
#ifdef verbose
							info.str("");
							info << " READY";
							scheduler->trace(info.str(),hostBlock->blockId);
#endif
						} else {
#ifdef verbose
							info.str("");
							info << "MASTER READY";
							scheduler->trace(info.str(),hostBlock->blockId);
#endif // verbose
							setRulesColor();
							//block->setDisplayedValue(-1);
						}
						tabSteps[1] = true;
						createBorder();
					}
				}
				break;

				case HEAD_MSG_ID : {
					SearchHeadMessage_ptr recvMessage = boost::static_pointer_cast<SearchHeadMessage>(message);
#ifdef verbose
					unsigned int sourceId = recvMessage->sourceInterface->hostBlock->blockId;
					info.str("");
					info << "rec. SearchHeadMessage() from " << sourceId;
					scheduler->trace(info.str(),hostBlock->blockId);
#endif // verbose
					if (!tabSteps[1]) {
						tabMemorisedMessages[2] = message;
						tabSteps[2]=true;
					} else {
						step2(message);
					}

				}
				break;

				case HBCK_MSG_ID : {
					SearchBackHeadMessage_ptr recvMessage = boost::static_pointer_cast<SearchBackHeadMessage>(message);
#ifdef verbose
					unsigned int sourceId = recvMessage->sourceInterface->hostBlock->blockId;
					info.str("");
					info << "rec. SearchBackHeadMsg("<<(recvMessage->exceptionBlock==NULL?-1:recvMessage->exceptionBlock->hostBlock->blockId)<<") from " << sourceId;
					scheduler->trace(info.str(),hostBlock->blockId);
#endif // verbose
                    if (recvMessage->exceptionBlock!=NULL) {
                        NeighborDirection dir=North;
                        int i=4;
                        while (i-- && block->getInterface(dir)->connectedInterface!=recvMessage->exceptionBlock) {
                            dir=NeighborDirection((int(dir)+1)%4);
                        }
                        dir=NeighborDirection((int(dir)+1)%4);
                        i=3;
                        while (i-- && block->getInterface(dir)==NULL) {
                            dir=NeighborDirection((int(dir)+1)%4);
                        }
                        if (i) {
                            sendSearchBackHeadMessage(block->getInterface(dir));
                        }
                        break;
                    }
					_previous = recvMessage->sourceInterface->connectedInterface;
                    applyRules();
                    if (possibleRules && !possibleRules->empty()) {
                        // on recherche une règle de tete
                        //possibleRules->pop_back();
                        while (!possibleRules->empty() && !possibleRules->back()->capa->isHead) possibleRules->pop_back();

                        if (!possibleRules->empty()) {
                            printRules();
/********************************************/
/*** cas particulier des éléments uniques ***/
/*** qui sont head et end en meme temps   ***/
                            if (possibleRules->back()->capa->isEnd) {
                                uint64_t t = scheduler->now();
                                Capability *capa = possibleRules->back()->capa;
#ifdef verbose
                                info.str("");
                                info << "special motion :" << capa->name ;
                                scheduler->trace(info.str(),hostBlock->blockId);
#endif // verbose
                                vector<Motion*>::const_iterator cm = capa->tabMotions.begin();
                                uint64_t st = t+20*time_offset;
                                Motion *currentMotion=NULL;
                                while (cm!=capa->tabMotions.end()) {
                                    if (currentMotion->PathToBlock.empty()) {
#ifdef verbose
                                        info.str("");
                                        info << "simple motion " << currentMotion->vect;
                                        scheduler->trace(info.str(),hostBlock->blockId);
#endif // verbose
                                        startMotion(t+2*time_offset,currentMotion->vect,0,capa->tabUnlockPath);
                                    } else {
#ifdef verbose
                                        info.str("");
                                        info << "multiple motion :" << capa->name ;
                                        scheduler->trace(info.str(),hostBlock->blockId);
#endif // verbose
                                        vector<short>::const_iterator cs=currentMotion->PathToBlock.begin();
                                        int n = currentMotion->PathToBlock.size();
                                        if (n) {
#ifdef verbose
                                            info.str("");
#endif // verbose
                                            short *tabDir = new short[n];
                                            int i=0;
                                            while (cs!=currentMotion->PathToBlock.end()) {
                                                tabDir[i++] = *cs;
#ifdef verbose
                                                info << *cs << " ";
#endif // verbose
                                                cs++;
                                            }
#ifdef verbose
                                            scheduler->trace(info.str(),hostBlock->blockId);
#endif // verbose
                                            // envoie le message de déplacement
                                            P2PNetworkInterface *p2p = block->getInterface(NeighborDirection(tabDir[0]));
                                            SingleMoveMessage *message = new SingleMoveMessage(tabDir+1,n-1,st,currentMotion->vect,currentMotion->UnlockPath,1);
                                            scheduler->schedule(new NetworkInterfaceEnqueueOutgoingEvent(scheduler->now() + time_offset, message, p2p));
#ifdef verbose
                                            info.str("");
                                            info << "send SingleMoveMessage("<<n-1<<") to " << p2p->connectedInterface->hostBlock->blockId;
                                            scheduler->trace(info.str(),hostBlock->blockId);
#endif // verbose
                                            delete [] tabDir;
                                        }
                                    }
                                    cm++;
                                }
                            } else {
                                // si le block était le block de départ de la recherche (_isEnd)
                                if (_isEnd) {
                                    OUTPUT << "le block de départ de la recherche " << endl;
                                    _previous = getBorderPreviousNeightbor(NULL);
                                    _next = getBorderNextNeightbor();
                                    block->setDisplayedValue(0);
                                    _isTrain = true;
                                    _isHead = true;
                                    _isEnd = false;
                                    _motionDir = possibleRules->back()->capa->tabMotions[0]->vect;

#ifdef verbose
                                    info.str("");
                                    info << "_previous =" << (_previous?_previous->connectedInterface->hostBlock->blockId:-1)
                                        << "  _next =" << (_next?_next->connectedInterface->hostBlock->blockId:-1)
                                        << " _isHead = " << _isHead << " _isEnd = " << _isEnd;
                                    scheduler->trace(info.str(),hostBlock->blockId);
#endif // verbose
                                    SearchEndTrainMessage *message = new SearchEndTrainMessage(1);
                                    scheduler->schedule(new NetworkInterfaceEnqueueOutgoingEvent(scheduler->now() + time_offset, message, _next));
#ifdef verbose
                                    info.str("");
                                    info << "send SearchEndTrainMessage to " << _next->connectedInterface->hostBlock->blockId;
                                    scheduler->trace(info.str(),hostBlock->blockId);
#endif // verbose
                                    setRulesColor();
                                } else {
                                    // on a trouvé la tete on cherche la queue du train
                                    _motionDir = possibleRules->back()->capa->tabMotions[0]->vect;
                                    _next = block->getP2PNetworkInterfaceByRelPos(*possibleRules->back()->capa->linkNextPos);
                                    SearchEndTrainMessage *message = new SearchEndTrainMessage(1);
                                    scheduler->schedule(new NetworkInterfaceEnqueueOutgoingEvent(scheduler->now() + time_offset, message, _next));
#ifdef verbose
                                    info.str("");
                                    info << "send SearchEndTrainMessage to " << _next->connectedInterface->hostBlock->blockId;
                                    scheduler->trace(info.str(),hostBlock->blockId);
#endif // verbose
                                    setRulesColor();
                                    block->setDisplayedValue(0);
                                    _isTrain = true;
                                    _isHead = true;
                                    _isEnd = false;
                                    _previous = NULL;
                                }
                            }
                        } else {
                            // si pas de rule de tete : on demande un retour en arriere
                            if (_isEnd || _next==NULL) {
                                OUTPUT << "le block de départ de la recherche 2" << endl;
                                setRulesColor();
/**
On peut envoyer un message searchHeadBack particulier qui demande d'ometre le block courant car il est un isthme !
on le renvoie au sender
                                SearchBackHeadMessage *message = new SearchBackHeadMessage(blockId);

**/
                                sendSearchBackHeadMessage(_previous,recvMessage->destinationInterface);
                            } else {
                                _previous = recvMessage->sourceInterface->connectedInterface;
                                applyRules();
                                _next = block->getP2PNetworkInterfaceByRelPos(*possibleRules->back()->capa->linkNextPos);
#ifdef verbose
                                info.str("");
                                info << "_previous =" << (_previous?_previous->connectedInterface->hostBlock->blockId:-1)
                                    << "  _next =" << (_next?_next->connectedInterface->hostBlock->blockId:-1)
                                     << " _isHead = " << _isHead << " _isEnd = " << _isEnd;
                                scheduler->trace(info.str(),hostBlock->blockId);
#endif // verbose
                                sendSearchBackHeadMessage(_next);
                            }
                        }
                    } else {
                        sendSearchBackHeadMessage(_next);
                    }
				}
				break;

				case END_MSG_ID : {
					SearchEndTrainMessage_ptr recvMessage = boost::static_pointer_cast<SearchEndTrainMessage>(message);
#ifdef verbose
					unsigned int sourceId = recvMessage->sourceInterface->hostBlock->blockId;
					info.str("");
					info << "rec. SearchEndTrainMessage("<< recvMessage->num <<") from " << sourceId;
					scheduler->trace(info.str(),hostBlock->blockId);
#endif // verbose
					if (!tabSteps[1]) {
						tabMemorisedMessages[3] = message;
                        tabSteps[3]=true;
					} else {
                        _previous = recvMessage->sourceInterface->connectedInterface;
						step3(message);
						tabSteps[3]=false;
					}

				}
				break;

				case TRAIN_READY_MSG_ID : {
					TrainReadyMessage_ptr recvMessage = boost::static_pointer_cast<TrainReadyMessage>(message);
#ifdef verbose
					unsigned int sourceId = recvMessage->sourceInterface->hostBlock->blockId;
					info.str("");
					info << "rec. TrainReadyMessage(" << recvMessage->queueFound << ") from " << sourceId;
					scheduler->trace(info.str(),hostBlock->blockId);
#endif
					if (recvMessage->queueFound) {
#ifdef verbose
					    info.str("");
                        info << "_previous =" << (_previous?_previous->connectedInterface->hostBlock->blockId:-1)
                                        << "  _next =" << (_next?_next->connectedInterface->hostBlock->blockId:-1)
                                        << " _isHead = " << _isHead << " _isEnd = " << _isEnd ;
                        scheduler->trace(info.str(),hostBlock->blockId);
#endif // verbose
                        printRules();
//                        Capability *capa = possibleRules->back()->capa;

                        _isTrain=true;
// si on est à la tete du train on peut créer des lignes
						if (_isHead) {
                            createLine(scheduler->now(),true);
						} else { // sinon on propage
							TrainReadyMessage *message = new TrainReadyMessage(true);
							scheduler->schedule(new NetworkInterfaceEnqueueOutgoingEvent(scheduler->now() + time_offset, message, _previous));
#ifdef verbose
							info.str("");
							info << "send TrainReadyMessage(true) to " << _previous->connectedInterface->hostBlock->blockId;
							scheduler->trace(info.str(),hostBlock->blockId);
#endif // verbose
							_isEnd=false;
						}
					} else {
                        applyRules();
						Capability *capa = possibleRules->back()->capa;
						if (capa && !capa->isHead) {
							// il faut chercher une règle de queue dans le bloc courant.
							while (!possibleRules->empty() && !possibleRules->back()->capa->isEnd) {
								possibleRules->pop_back();
							}
							if (!possibleRules->empty()) {
                                TrainReadyMessage *message = new TrainReadyMessage(true);
								scheduler->schedule(new NetworkInterfaceEnqueueOutgoingEvent(scheduler->now() + time_offset, message, _previous));
#ifdef verbose
								info.str("");
								info << "send TrainReadyMessage(true) to " << _previous->connectedInterface->hostBlock->blockId;
								scheduler->trace(info.str(),hostBlock->blockId);
#endif // verbose
								_isTrain = true;
								_isEnd = true;
							} else {
								// sinon on propage faux vers le début
								TrainReadyMessage *message = new TrainReadyMessage(false);
								scheduler->schedule(new NetworkInterfaceEnqueueOutgoingEvent(scheduler->now() + time_offset, message, _previous));
#ifdef verbose
								info.str("");
								info << "send TrainReadyMessage(false) to " << _previous->connectedInterface->hostBlock->blockId;
								scheduler->trace(info.str(),hostBlock->blockId);
#endif // verbose
								_isTrain = false;
								_isEnd = false;
							}
						} else {
                            /*DisableTrainMessage *message = new DisableTrainMessage();
                            scheduler->schedule(new NetworkInterfaceEnqueueOutgoingEvent(scheduler->now() + time_offset, message, _next));
							info.str("");
							info << "send DisableTrainMessage() to " << _next->connectedInterface->hostBlock->blockId;
							scheduler->trace(info.str(),hostBlock->blockId);
							_isTrain = false;
							_isEnd=false;*/
						}
					}
				}
				break;

				case CREATE_LINE_MSG_ID : {
					CreateLineMessage_ptr recvMessage = boost::static_pointer_cast<CreateLineMessage>(message);
#ifdef verbose
					unsigned int sourceId = recvMessage->sourceInterface->hostBlock->blockId;
					info.str("");
					info << " rec. CreateLineMessage(" << recvMessage->etime << ") from " << sourceId;
					scheduler->trace(info.str(),hostBlock->blockId);
#endif
				    createLine(recvMessage->etime,false);
				}
				break;

				case SET_RDV_MSG_ID : {
					SetRDVMessage_ptr recvMessage = boost::static_pointer_cast<SetRDVMessage>(message);
#ifdef verbose
					unsigned int sourceId = recvMessage->sourceInterface->hostBlock->blockId;
					info.str("");
					info << hostBlock->blockId << " rec. SetRDVMessage(" << recvMessage->rdvTime << "," << recvMessage->motionVector << ") from " << sourceId;
					scheduler->trace(info.str(),hostBlock->blockId);
#endif // verbose
					if (!_isHead && !_isHeadOfLine) {
						SetRDVMessage *message = new SetRDVMessage(recvMessage->rdvTime ,recvMessage->motionVector);
						scheduler->schedule(new NetworkInterfaceEnqueueOutgoingEvent(scheduler->now() + time_offset, message, _previous));
#ifdef verbose
						info.str("");
						info << "send SetRDVMessage to " << _previous->connectedInterface->hostBlock->blockId;
						scheduler->trace(info.str(),hostBlock->blockId);
#endif // verbose
                    }
					startMotion(recvMessage->rdvTime,recvMessage->motionVector,0,possibleRules->back()->capa->tabUnlockPath);
				}
				break;

				case UNLOCK_MSG_ID : {
					UnlockMessage_ptr recvMessage = boost::static_pointer_cast<UnlockMessage>(message);
#ifdef verbose
					unsigned int sourceId = recvMessage->sourceInterface->hostBlock->blockId;
					info.str("");
					info << hostBlock->blockId << " rec. UnlockMessage(" << recvMessage->sz << "," << recvMessage->step << ") from " << sourceId;
					scheduler->trace(info.str(),hostBlock->blockId);
#endif // verbose
					if (recvMessage->sz==0) {
						//_previous = NULL;
						_isHeadOfLine = true;
						uint64_t t = scheduler->now();
						// case ou le bloc de fin est lui meme sur un angle
						Capability *capa = possibleRules->back()->capa;
						if (capa->isAngle || capa->isEnd) {
							vector<Motion*>::const_iterator cm = capa->tabMotions.begin();
							uint64_t st = t+20*time_offset;
							Motion *currentMotion=NULL;
							while (cm!=capa->tabMotions.end()) {
								if ((*cm)->time==recvMessage->step) {
									currentMotion = *cm;
									if (currentMotion->PathToBlock.empty()) {
#ifdef verbose
										info.str("");
										info << "simple motion " << currentMotion->vect;
										scheduler->trace(info.str(),hostBlock->blockId);
#endif
										startMotion(t+2*time_offset,currentMotion->vect,0,capa->tabUnlockPath);
									} else {
#ifdef verbose
										info.str("");
										info << "multiple motion :" << capa->name ;
										scheduler->trace(info.str(),hostBlock->blockId);
#endif // verbose
										vector<short>::const_iterator cs=currentMotion->PathToBlock.begin();
										int n = currentMotion->PathToBlock.size();
										if (n) {
#ifdef verbose
											info.str("");
#endif
											short *tabDir = new short[n];
											int i=0;
											while (cs!=currentMotion->PathToBlock.end()) {
												tabDir[i++] = *cs;
#ifdef verbose
												info << *cs << " ";
#endif // verbose
												cs++;
											}
#ifdef verbose
											scheduler->trace(info.str(),hostBlock->blockId);
#endif
											// envoie le message de déplacement
											P2PNetworkInterface *p2p = block->getInterface(NeighborDirection(tabDir[0]));
											SingleMoveMessage *message = new SingleMoveMessage(tabDir+1,n-1,st,currentMotion->vect,currentMotion->UnlockPath,recvMessage->step+1);
											scheduler->schedule(new NetworkInterfaceEnqueueOutgoingEvent(scheduler->now() + time_offset, message, p2p));
#ifdef verbose
											info.str("");
											info << "send SingleMoveMessage("<<n-1<<") to " << p2p->connectedInterface->hostBlock->blockId;
											scheduler->trace(info.str(),hostBlock->blockId);
#endif // verbose
											delete [] tabDir;
										}
									}
								}
								cm++;
							}
							// si on a pas trouve de déplacement valide : fin de déplacement
							if (!currentMotion) {
								setRulesColor();
								// apply rules
								applyRules();
								// on cherche la premiere règle qui est un dernier
								while (!possibleRules->empty() && !possibleRules->back()->capa->isEnd) possibleRules->pop_back();

								printRules();

								_previous = getBorderPreviousNeightbor(NULL);
								_next = NULL;
								_isTrain = false;
								// envoie le message de reconstruction du train
								ReconnectTrainMessage *message = new ReconnectTrainMessage(possibleRules && possibleRules->size()>0);
								scheduler->schedule(new NetworkInterfaceEnqueueOutgoingEvent(scheduler->now() + time_offset, message, _previous));
#ifdef verbose
								info.str("");
								info << "send ReconnectTrainMessage to " << _previous->connectedInterface->hostBlock->blockId;
								scheduler->trace(info.str(),hostBlock->blockId,YELLOW);
#endif // verbose
							}
						} else { // si non on propage
							createLine(t,true);
						}
					} else {
						// envoie le message de déblocage
						P2PNetworkInterface *p2p = block->getInterface(NeighborDirection(recvMessage->tab[0]));
						UnlockMessage *message = new UnlockMessage(recvMessage->tab+1,recvMessage->sz-1,recvMessage->step);
						scheduler->schedule(new NetworkInterfaceEnqueueOutgoingEvent(scheduler->now() + time_offset, message, p2p));
#ifdef verbose
						info.str("");
						info << "send UnlockMessage to " << p2p->connectedInterface->hostBlock->blockId;
						scheduler->trace(info.str(),hostBlock->blockId);
#endif
					}
				}
				break;

				case SINGLEMV_MSG_ID : {
					SingleMoveMessage_ptr recvMessage = boost::static_pointer_cast<SingleMoveMessage>(message);
#ifdef verbose
					unsigned int sourceId = recvMessage->sourceInterface->hostBlock->blockId;
					info.str("");
					info << hostBlock->blockId << " rec. SingleMoveMessage(" << recvMessage->sz << "," << recvMessage->step << ") from " << sourceId;
					scheduler->trace(info.str(),hostBlock->blockId);
#endif
					if (recvMessage->sz==0) {
						startMotion(recvMessage->startTime,recvMessage->motionVector,recvMessage->step,recvMessage->unlockPath);
					} else {
						// envoie le message de déblocage
						P2PNetworkInterface *p2p = block->getInterface(NeighborDirection(recvMessage->tab[0]));
						SingleMoveMessage *message = new SingleMoveMessage(recvMessage->tab+1,recvMessage->sz-1,recvMessage->startTime,recvMessage->motionVector,recvMessage->unlockPath,recvMessage->step);
						scheduler->schedule(new NetworkInterfaceEnqueueOutgoingEvent(scheduler->now() + time_offset, message, p2p));
#ifdef verbose
						info.str("");
						info << "send SingleMoveMessage("<< recvMessage->sz-1 <<") to " << p2p->connectedInterface->hostBlock->blockId;
						scheduler->trace(info.str(),hostBlock->blockId);
#endif // verbose
					}
				}
				break;

				case RECONNECT_MSG_ID : {
					ReconnectTrainMessage_ptr recvMessage = boost::static_pointer_cast<ReconnectTrainMessage>(message);
#ifdef verbose
					unsigned int sourceId = recvMessage->sourceInterface->hostBlock->blockId;
					info.str("");
					info << "rec. ReconnectTrainMsg("<< recvMessage->hasRule <<") from " << sourceId;
					scheduler->trace(info.str(),hostBlock->blockId);
#endif // verbose
                    if (recvMessage->hasRule) {
                        _next = recvMessage->sourceInterface->connectedInterface;
                        _previous = getBorderPreviousNeightbor(_next);
                        if (_previous==_next) {
                            _previous=NULL;
                        }
                    } else {
                        _previous = recvMessage->sourceInterface->connectedInterface;
                        _next = getBorderNextNeightbor();
                    }
                    reconnect(recvMessage->hasRule);
				}
				break;
/*
				case DISABLE_MSG_ID : {
					DisableTrainMessage_ptr recvMessage = boost::static_pointer_cast<DisableTrainMessage>(message);
					unsigned int sourceId = recvMessage->sourceInterface->hostBlock->blockId;
					info.str("");
					info << hostBlock->blockId << " rec. DisableTrainMessage() from " << sourceId;
					scheduler->trace(info.str(),hostBlock->blockId);

					if (!_isEnd) {
						DisableTrainMessage *message = new DisableTrainMessage();
						scheduler->schedule(new NetworkInterfaceEnqueueOutgoingEvent(scheduler->now() + time_offset, message, _next));
						info.str("");
						info << "send DisableTrainMessage to " << _next->connectedInterface->hostBlock->blockId;
						scheduler->trace(info.str(),hostBlock->blockId);
					} else {
						sendNoActivity(block->getDirection(recvMessage->sourceInterface->connectedInterface),block->blockId);
					}
					_previous=NULL;
					_next=NULL;
					_isTrain = false;
					block->wellPlaced = targetGrid[posGrid.y*gridSize[0]+posGrid.x]==fullCell;
					//block->setDisplayedValue(-1);
					setRulesColor();
				}
				break;

				case NOACTIVITY_MSG_ID : {
					NoActivityMessage_ptr recvMessage = boost::static_pointer_cast<NoActivityMessage>(message);
					unsigned int sourceId = recvMessage->sourceInterface->hostBlock->blockId;
					info.str("");
					info << " rec. NoActivityMessage : " << recvMessage->senderID << " from " << sourceId;
					scheduler->trace(info.str(),hostBlock->blockId);

					if (recvMessage->senderID==block->blockId) {
						_currentStage++;
						init();
						sendInitToNeighbors(NULL,_currentStage);
					} else {
						if (!_isTrain) {
							sendNoActivity(block->getDirection(recvMessage->sourceInterface->connectedInterface),recvMessage->senderID);
							//block->setDisplayedValue(recvMessage->senderID);
							setRulesColor();
						} else {
							info.str("");
							info << block->blockId << " is train!";
							scheduler->trace(info.str(),hostBlock->blockId);
						}
					}
				}
				break;

				case REINIT_MSG_ID : {
					ReInitMessage_ptr recvMessage = boost::static_pointer_cast<ReInitMessage>(message);
					unsigned int sourceId = recvMessage->sourceInterface->hostBlock->blockId;
					info.str("");
					info << hostBlock->blockId << " rec. ReInitMessage(" << recvMessage->stage << "/" << _currentStage << ") from " << sourceId;
					scheduler->trace(info.str(),hostBlock->blockId);

					if (_currentStage==recvMessage->stage) {
						sendAckInit(recvInterface);
					} else {
					// premier message
						_currentStage = recvMessage->stage;
						init();
						block2Answer=recvInterface;
						sendInitToNeighbors(block2Answer,_currentStage);
					}

					if (nbreOfWaitedAnswers==0) {
						if (block2Answer!=NULL) {
							sendAckInit(block2Answer);
							block2Answer=NULL;

							info.str("");
							info << " READY";
							scheduler->trace(info.str(),hostBlock->blockId);
						} else {
							info.str("");
							info << "Master READY";
							scheduler->trace(info.str(),hostBlock->blockId);
							setRulesColor();
							//block->setDisplayedValue(-1);

						}
						////////
						tabSteps[1] = true;
						createBorder();
					}
				}
				break;
				case ACKINIT_MSG_ID : {
					AckInitMessage_ptr recvMessage = boost::static_pointer_cast<AckInitMessage>(message);
					unsigned int sourceId = recvMessage->sourceInterface->hostBlock->blockId;
					info.str("");
					info << hostBlock->blockId << " rec. AckInitMessage(" << nbreOfWaitedAnswers << ") from " << sourceId;
					scheduler->trace(info.str(),hostBlock->blockId);

					nbreOfWaitedAnswers--;
					if (nbreOfWaitedAnswers==0) {
						if (block2Answer!=NULL) {
							sendAckInit(block2Answer);
							block2Answer=NULL;

							info.str("");
							info << " READY";
							scheduler->trace(info.str(),hostBlock->blockId);
						} else {
							info.str("");
							info << "MASTER READY";
							scheduler->trace(info.str(),hostBlock->blockId);
						}
						tabSteps[1] = true;
						createBorder();
                        // Gère les messages en attente
                        if (tabSteps[2]) {
                            step2(tabMemorisedMessages[2]);
                            tabSteps[2]=false;
                        }
                        if (tabSteps[3]) {
                            step3(tabMemorisedMessages[3]);
                            tabSteps[3]=false;
                        }
					}
				}
				break;*/
                case ASK4END_MSG_ID : {
                    Ask4EndMessage_ptr recvMessage = boost::static_pointer_cast<Ask4EndMessage>(message);
                    block->setDisplayedValue(-1);
#ifdef verbose
					unsigned int sourceId = recvMessage->sourceInterface->hostBlock->blockId;
					info.str("");
					info << "rec. Ask4EndMessage(" << recvMessage->currentMove << ") from " << sourceId;
					scheduler->trace(info.str(),hostBlock->blockId);
#endif
                    if (_currentMove==recvMessage->currentMove) {
						sendAns4EndMessage(recvInterface,0);
					} else {
                        init();
                        block2Answer=recvInterface;
						sendAsk4EndToNeighbors(block2Answer);
                        if (nbreOfWaitedAnswers==0) {
                            if (block2Answer!=NULL) {
                                sendAns4EndMessage(block2Answer,_nbreWellPlacedBlocks);
                                block2Answer=NULL;
                            } else {
                                setRulesColor();
                            }
                        }
					}
                }
                break;

                case ANS4END_MSG_ID : {
					Ans4EndMessage_ptr recvMessage = boost::static_pointer_cast<Ans4EndMessage>(message);
#ifdef verbose
					unsigned int sourceId = recvMessage->sourceInterface->hostBlock->blockId;
					info.str("");
					info << "rec. Ans4EndMessage(" << recvMessage->nbreWellPlaced << "," << nbreOfWaitedAnswers << ") from " << sourceId;
					scheduler->trace(info.str(),hostBlock->blockId);
#endif
					nbreOfWaitedAnswers--;
					_nbreWellPlacedBlocks+=recvMessage->nbreWellPlaced;
					if (nbreOfWaitedAnswers==0) {
						if (block2Answer!=NULL) {
							sendAns4EndMessage(block2Answer,_nbreWellPlacedBlocks);
							block2Answer=NULL;
						} else {
							setRulesColor();
							//block->setDisplayedValue(_nbreWellPlacedBlocks);

                            _next = getBorderNextNeightborNoWellPlaced();
                            OUTPUT << "  _next =" << (_next?_next->connectedInterface->hostBlock->blockId:-1) << endl;
                            _previous = getBorderPreviousNeightborNoWellPlaced(_next);
                            /*if (((SmartBlocksBlock*)(_previous->connectedInterface->hostBlock))->wellPlaced) {
                                P2PNetworkInterface *tmp = _previous;
                                _previous = _next;
                                _next = tmp;
                            }*/
#ifdef verbose
                            info.str("");
                            info << "_previous =" << (_previous?_previous->connectedInterface->hostBlock->blockId:-1)
                                 << "  _next =" << (_next?_next->connectedInterface->hostBlock->blockId:-1)
                                 << " _isHead = " << _isHead << " _isEnd = " << _isEnd;
                            scheduler->trace(info.str(),hostBlock->blockId);
#endif // verbose
                            applyRules();

                            if (_nbreWellPlacedBlocks!=_nbreGoalCells) {

                                if (possibleRules && possibleRules->back()) {
                                    /*if (!possibleRules->back()->capa->isHead) {
                                        _previous = block->getP2PNetworkInterfaceByRelPos(*possibleRules->back()->capa->linkPrevPos);
                                    }
                                    if (possibleRules->back()->capa->linkNextPos) {
                                        _next = block->getP2PNetworkInterfaceByRelPos(*possibleRules->back()->capa->linkNextPos);
                                    }*/
#ifdef verbose
                                    info.str("");
                                    info << "_previous =" << (_previous?_previous->connectedInterface->hostBlock->blockId:-1)
                                        << "  _next =" << (_next?_next->connectedInterface->hostBlock->blockId:-1)
                                        << " _isHead = " << _isHead << " _isEnd = " << _isEnd;
                                    scheduler->trace(info.str(),hostBlock->blockId);
#endif
                                }
            /********************************************/
            /*** cas particulier des éléments uniques ***/
            /*** qui sont head et end en meme temps   ***/
                                if (possibleRules->back()->capa->isEnd && possibleRules->back()->capa->isHead) {
                                    uint64_t t = scheduler->now();
                                    Capability *capa = possibleRules->back()->capa;
#ifdef verbose
                                    info.str("");
                                    info << "special motion :" << capa->name ;
                                    scheduler->trace(info.str(),hostBlock->blockId);
#endif // verbose
                                    vector<Motion*>::const_iterator cm = capa->tabMotions.begin();
                                    uint64_t st = t+20*time_offset;
                                    Motion *currentMotion=NULL;
                                    while (cm!=capa->tabMotions.end()) {
                                        currentMotion = *cm;
                                        if (currentMotion->PathToBlock.empty()) {
#ifdef verbose
                                            info.str("");
                                            info << "simple motion " << currentMotion->vect;
                                            scheduler->trace(info.str(),hostBlock->blockId);
#endif // verbose
                                            startMotion(t+2*time_offset,currentMotion->vect,0,capa->tabUnlockPath);
                                        } else {
#ifdef verbose
                                            info.str("");
                                            info << "multiple motion :" << capa->name ;
                                            scheduler->trace(info.str(),hostBlock->blockId);
#endif // verbose
                                            vector<short>::const_iterator cs=currentMotion->PathToBlock.begin();
                                            int n = currentMotion->PathToBlock.size();
                                            if (n) {
                                                short *tabDir = new short[n];
                                                int i=0;
                                                while (cs!=currentMotion->PathToBlock.end()) {
                                                    tabDir[i++] = *cs;
                                                    cs++;
                                                }
#ifdef verbose
                                                scheduler->trace(info.str(),hostBlock->blockId);
#endif // verbose
                                                // envoie le message de déplacement
                                                P2PNetworkInterface *p2p = block->getInterface(NeighborDirection(tabDir[0]));
                                                SingleMoveMessage *message = new SingleMoveMessage(tabDir+1,n-1,st,currentMotion->vect,currentMotion->UnlockPath,1);
                                                scheduler->schedule(new NetworkInterfaceEnqueueOutgoingEvent(scheduler->now() + time_offset, message, p2p));
#ifdef verbose
                                                info.str("");
                                                info << "send SingleMoveMessage("<<n-1<<") to " << p2p->connectedInterface->hostBlock->blockId;
                                                scheduler->trace(info.str(),hostBlock->blockId);
#endif // verbose
                                                delete [] tabDir;
                                            }
                                        }
                                        cm++;
                                    }
                                } else
                                // on garde en priorité une règle de queue
                                // il faut que la règle courante soit une regle de fin
                                if (possibleRules) {
                                    while (!possibleRules->empty() && !possibleRules->back()->capa->isEnd) {
                                        possibleRules->pop_back();
                                    }
                                    printRules();
                                    if (!possibleRules->empty()) {
                /** on peut poursuivre le déplacement du train à partir du meme bloc de fin **/
                                        _isEnd=true;
                                        _next=NULL;
                                        ReconnectTrainMessage *message = new ReconnectTrainMessage(possibleRules && possibleRules->size()>0);
                                        scheduler->schedule(new NetworkInterfaceEnqueueOutgoingEvent(scheduler->now() + time_offset, message, _previous));
#ifdef verbose
                                        info.str("");
                                        info << "send ReconnectTrainMessage to " << _previous->connectedInterface->hostBlock->blockId;
                                        scheduler->trace(info.str(),hostBlock->blockId,YELLOW);
#endif // verbose
                                    } else {
                /** on ne peut pas repartir de la queue courante, on recherche un train complet **/
                                        applyRules();

                                        // send searchHeadMessage
                                        SearchHeadMessage *message = new SearchHeadMessage();
                                        scheduler->schedule(new NetworkInterfaceEnqueueOutgoingEvent(scheduler->now() + time_offset, message, _previous));
#ifdef verbose
                                        info.str("");
                                        info << "send SearchHeadMessage to " << _previous->connectedInterface->hostBlock->blockId;
                                        scheduler->trace(info.str(),hostBlock->blockId);
#endif // verbose
                                    }
                                } else {
                                /** on ne peut pas repartir de la queue courante, on recherche un train complet **/
                                    applyRules();

                                    // send searchHeadMessage
                                    SearchHeadMessage *message = new SearchHeadMessage();
                                    scheduler->schedule(new NetworkInterfaceEnqueueOutgoingEvent(scheduler->now() + time_offset, message, _previous));
#ifdef verbose
                                    info.str("");
                                    info << "send SearchHeadMessage to " << _previous->connectedInterface->hostBlock->blockId;
                                    scheduler->trace(info.str(),hostBlock->blockId);
#endif // verbose
                                }
                            } else {
                                wrl->printStats();
                            }
						}
					}
				}
				break;


				default :
					cerr << "Block " << hostBlock->blockId << " received an unrecognized message from " << message->sourceInterface->hostBlock->blockId << endl;
				break;
			}
		break;
	}
	setRulesColor();
}

void SbReconfBlockCode::init() {
	/*tabSteps[1] = false;
	tabSteps[2] = false;
	tabSteps[3] = false;*/
	posGrid.x = block->position[0];
	posGrid.y = block->position[1];
	block->wellPlaced = targetGrid[posGrid.y*gridSize[0]+posGrid.x]==fullCell;
	setRulesColor();
	//block->setDisplayedValue(-1);
	_isTrain = false;
	block->_isBorder = false;
	/*_isHead = false;
	_isEnd = false;*/
	_motionDir.set(0,0);

	setRulesColor();
}

SmartBlocks::SmartBlocksBlockCode* SbReconfBlockCode::buildNewBlockCode(SmartBlocksBlock *host) {
	return(new SbReconfBlockCode(host));
}

void SbReconfBlockCode::step2(MessagePtr message) {
	SearchHeadMessage_ptr recvMessage = boost::static_pointer_cast<SearchHeadMessage>(message);
	stringstream info;

    _isTrain = false;
    _next = recvMessage->sourceInterface->connectedInterface;
    applyRules();
	if (possibleRules && possibleRules->back()) {
		Capability *capa = possibleRules->back()->capa;
		_isTrain = false;
        if (capa->isHead) {
			// send searchEndTrainMessage
#ifdef verbose
            info.str("");
            info << "  _next =" << (_next?_next->connectedInterface->hostBlock->blockId:-1);
            scheduler->trace(info.str(),hostBlock->blockId);
#endif // verbose
            _previous = NULL;
			SearchEndTrainMessage *message = new SearchEndTrainMessage(1);
			scheduler->schedule(new NetworkInterfaceEnqueueOutgoingEvent(scheduler->now() + time_offset, message, _next));
#ifdef verbose
			info.str("");
			info << "send SearchEndTrainMessage to " << _next->connectedInterface->hostBlock->blockId;
			scheduler->trace(info.str(),hostBlock->blockId);
#endif
			setRulesColor();
			block->setDisplayedValue(0);
			_isHead = true;
		} else /*if (capa->linkPrevPos)*/ {
            _previous = getBorderPreviousNeightbor(_next);
#ifdef verbose
            info.str("");
            info << "_previous =" << (_previous?_previous->connectedInterface->hostBlock->blockId:-1);
            scheduler->trace(info.str(),hostBlock->blockId);
#endif // verbose
            _isHead = false;
            block->setDisplayedValue(-1);

		    // send searchHeadMessage
			SearchHeadMessage *message = new SearchHeadMessage();
			scheduler->schedule(new NetworkInterfaceEnqueueOutgoingEvent(scheduler->now() + time_offset, message, _previous));
#ifdef verbose
			info.str("");
			info << "send SearchHeadMessage to " << _previous->connectedInterface->hostBlock->blockId;
			scheduler->trace(info.str(),hostBlock->blockId);
#endif // verbose
		}
	} else /*if (block->_isBorder && _next) */{
        // si pas de rule : on demande un retour en arriere
#ifdef verbose
		info.str("");
        info << "send SearchBackHeadMessage to " << _next->connectedInterface->hostBlock->blockId;
        scheduler->trace(info.str(),hostBlock->blockId);
#endif // verbose
        _isHead = false;
		_isTrain = false;
		block->_isBorder = false;
		block->setDisplayedValue(-1);

        setRulesColor();
        SearchBackHeadMessage *message = new SearchBackHeadMessage();
        scheduler->schedule(new NetworkInterfaceEnqueueOutgoingEvent(scheduler->now() + time_offset, message,_next));
	}
}

void SbReconfBlockCode::step3(MessagePtr message) {
	SearchEndTrainMessage_ptr recvMessage = boost::static_pointer_cast<SearchEndTrainMessage>(message);
#ifdef verbose
	stringstream info;
#endif // verbose

    applyRules();
/** attention,
on test d'abord si c'est un isthme avant de faire tout déplacement

**/
    if (possibleRules && !possibleRules->empty()) {
        Capability *capa = possibleRules->back()->capa;
        bool test = testIsthmus(capa->linkPrevPos->x,capa->linkPrevPos->y);
        //OUTPUT << "TEST ISTHMUS :" << test << endl;

        if (test) {
#ifdef verbose
            info.str("");
			info << "isthmus " << posGrid << ": " << capa->isAngle << ";" << _previous->connectedInterface->hostBlock->blockId;
			scheduler->trace(info.str(),hostBlock->blockId);
#endif // verbose
			// il faut trouver un bloc de fin de train avant le bloc courant.
            TrainReadyMessage *message = new TrainReadyMessage(false);

            scheduler->schedule(new NetworkInterfaceEnqueueOutgoingEvent(scheduler->now() + time_offset, message, _previous));
#ifdef verbose
			info.str("");
			info << "send TrainReadyMessage(false) to " << _previous->connectedInterface->hostBlock->blockId;
            scheduler->trace(info.str(),hostBlock->blockId);
#endif // verbose
			_isTrain = false;
            _isEnd = false;
            block->_isBorder = false;
            return;
        }
    }


    if (possibleRules) {
/** Si il existe une règle de corps, on propage **/
// il faut que la règle courante soit ni tete ni queue
        while (!possibleRules->empty() && (possibleRules->back()->capa->isHead || possibleRules->back()->capa->isEnd)) {
            possibleRules->pop_back();
        }
        printRules();
    }

    if (possibleRules && !possibleRules->empty()) {
        Capability *capa = possibleRules->back()->capa;
            block->setDisplayedValue(recvMessage->num);

    /* on vérifie que l'on passe pas un isthme
        bool test = testIsthmus(capa->linkPrevPos->x,capa->linkPrevPos->y);
		if (!capa->isAngle && test) {
			info.str("");
			info << "isthmus " << posGrid << ": " << *(capa->linkPrevPos) << "," << *(capa->linkNextPos) << "," << capa->isAngle << ";" << _previous->connectedInterface->hostBlock->blockId;
			scheduler->trace(info.str(),hostBlock->blockId);
			// il faut trouver un bloc de fin de train avant le bloc courant.
            TrainReadyMessage *message = new TrainReadyMessage(false);
            scheduler->schedule(new NetworkInterfaceEnqueueOutgoingEvent(scheduler->now() + time_offset, message, _previous));
			info.str("");
			info << "send TrainReadyMessage(false) to " << _previous->connectedInterface->hostBlock->blockId;
            scheduler->trace(info.str(),hostBlock->blockId);
			_isTrain = false;
            _isEnd = false;
            block->_isBorder = false;
        } else {*/
            // send searchEndTrainMessage
            PointCel np = *capa->linkNextPos;
            _next = block->getP2PNetworkInterfaceByRelPos(np);
#ifdef verbose
            info.str("");
            info << "next = " << np << "," << (_next->connectedInterface?_next->connectedInterface->hostBlock->blockId:-1);
            scheduler->trace(info.str(),hostBlock->blockId);
#endif // verbose
            SearchEndTrainMessage *message = new SearchEndTrainMessage(recvMessage->num+1);
            scheduler->schedule(new NetworkInterfaceEnqueueOutgoingEvent(scheduler->now() + time_offset, message, _next));
#ifdef verbose
            info.str("");
            info << "send SearchEndTrainMessage to " << _next->connectedInterface->hostBlock->blockId;
            scheduler->trace(info.str(),hostBlock->blockId);
#endif // verbose
            _isTrain = true;
            block->_isBorder = true;
            _isEnd = false;
        //}
	} else {
/** si echec de propagation, on répond si le bloc peut être une queue **/
        applyRules();
        if (possibleRules) {
            while (!possibleRules->empty() && !possibleRules->back()->capa->isEnd) {
                possibleRules->pop_back();
            }
            printRules();
        }
        if (possibleRules && !possibleRules->empty()) {
            // send trainReadyMessage
            TrainReadyMessage *message = new TrainReadyMessage(true);
            scheduler->schedule(new NetworkInterfaceEnqueueOutgoingEvent(scheduler->now() + time_offset, message, _previous));
#ifdef verbose
            info.str("");
            info << "send TrainReadyMessage(true) to " << _previous->connectedInterface->hostBlock->blockId;
            scheduler->trace(info.str(),hostBlock->blockId);
#endif // verbose
            _isTrain = true;
            _isEnd = true;
            block->_isBorder = true;
        } else {
// il faut trouver un bloc de fin de train avant le bloc courant.
            TrainReadyMessage *message = new TrainReadyMessage(false);
			scheduler->schedule(new NetworkInterfaceEnqueueOutgoingEvent(scheduler->now() + time_offset, message, _previous));
#ifdef verbose
			info.str("");
			info << "send TrainReadyMessage(false) to " << _previous->connectedInterface->hostBlock->blockId;
			scheduler->trace(info.str(),hostBlock->blockId);
#endif // verbose
			_isTrain = false;
            _isEnd = false;
            block->_isBorder = false;
        }
	}

}

void SbReconfBlockCode::reconnect(bool hasRule) {
#ifdef verbose
	stringstream info;
    info.str("");
    info << "_previous =" << (_previous?_previous->connectedInterface->hostBlock->blockId:-1)
                        << "  _next =" << (_next?_next->connectedInterface->hostBlock->blockId:-1)
                        << " _isHead = " << _isHead << " _isEnd = " << _isEnd;
    scheduler->trace(info.str(),hostBlock->blockId);
#endif // verbose
    // apply rules
    applyRules();

    if (!hasRule) {
        //if (_previous==_next) {
        // on recherche une règle de tete et queue (déplacement spéciaux)
            while (!possibleRules->empty() && (!possibleRules->back()->capa->isHead || !possibleRules->back()->capa->isEnd)) possibleRules->pop_back();
        //}

        if (!possibleRules->empty()) {
            printRules();
            if (possibleRules->back()->capa->isHead) {
            // cas particulier des singletons en mouvement
                if (possibleRules->back()->capa->isEnd) {
                    uint64_t t = scheduler->now();
                    Capability *capa = possibleRules->back()->capa;
    #ifdef verbose
                    info.str("");
                    info << "special motion :" << capa->name ;
                    scheduler->trace(info.str(),hostBlock->blockId);
    #endif // verbose
                    vector<Motion*>::const_iterator cm = capa->tabMotions.begin();
                    uint64_t st = t+20*time_offset;
                    Motion *currentMotion=NULL;
                    while (cm!=capa->tabMotions.end()) {
                        currentMotion = *cm;
                        if (currentMotion->PathToBlock.empty()) {
    #ifdef verbose
                            info.str("");
                            info << "simple motion " << currentMotion->vect;
                            scheduler->trace(info.str(),hostBlock->blockId);
    #endif // verbose
                            startMotion(t+2*time_offset,currentMotion->vect,0,capa->tabUnlockPath);
                        } else {
    #ifdef verbose
                            info.str("");
                            info << "multiple motion :" << capa->name ;
                            scheduler->trace(info.str(),hostBlock->blockId);
    #endif // verbose
                            vector<short>::const_iterator cs=currentMotion->PathToBlock.begin();
                            int n = currentMotion->PathToBlock.size();
                            if (n) {
                                short *tabDir = new short[n];
                                int i=0;
                                while (cs!=currentMotion->PathToBlock.end()) {
                                    tabDir[i++] = *cs;
                                    cs++;
                                }
    #ifdef verbose
                                scheduler->trace(info.str(),hostBlock->blockId);
    #endif // verbose
                                // envoie le message de déplacement
                                P2PNetworkInterface *p2p = block->getInterface(NeighborDirection(tabDir[0]));
                                SingleMoveMessage *message = new SingleMoveMessage(tabDir+1,n-1,st,currentMotion->vect,currentMotion->UnlockPath,1);
                                scheduler->schedule(new NetworkInterfaceEnqueueOutgoingEvent(scheduler->now() + time_offset, message, p2p));
    #ifdef verbose
                                info.str("");
                                info << "send SingleMoveMessage("<<n-1<<") to " << p2p->connectedInterface->hostBlock->blockId;
                                scheduler->trace(info.str(),hostBlock->blockId);
    #endif // verbose
                                delete [] tabDir;
                            }
                        }
                        cm++;
                    }
                    return;
                } else {
 // cas général on cherche la queue
                    _isHead = true;
                    block->setDisplayedValue(0);

                }
            }
        }

    }



/********************************************************************************
* on remonte le bord jusqu'à la tête
********************************************************************************/
    if (!_previous || _isHead) {
        setRulesColor();
        // on recherche une rêgle de tete
        bool headFound=false;
        if (possibleRules && !possibleRules->empty()) {
            // on recherche une règle de tete
            while (!possibleRules->empty() && !possibleRules->back()->capa->isHead) possibleRules->pop_back();

            if (!possibleRules->empty()) {
                printRules();
                _isHead = true;
                _previous = NULL;
                _isTrain = true;
                block->setDisplayedValue(0);

                _motionDir = possibleRules->back()->capa->tabMotions[0]->vect;
                // send searchEndTrainMessage
                SearchEndTrainMessage *message = new SearchEndTrainMessage(1);
                scheduler->schedule(new NetworkInterfaceEnqueueOutgoingEvent(scheduler->now() + time_offset, message, _next));
#ifdef verbose
                info.str("");
                info << "send SearchEndTrainMessage to " << _next->connectedInterface->hostBlock->blockId;
                scheduler->trace(info.str(),hostBlock->blockId);
#endif
                setRulesColor();
                headFound=true;
                //block->setDisplayedValue(1);
            }
        }
        if (!headFound) {
            sendSearchBackHeadMessage(_next);
        }
    } else {
        // envoie le message de reconstruction du train
        //_next = recvMessage->sourceInterface->connectedInterface;
        _previous = getBorderPreviousNeightbor(_next);
#ifdef verbose
        info.str("");
        info << "_previous =" << (_previous?_previous->connectedInterface->hostBlock->blockId:-1)
                        << "  _next =" << (_next?_next->connectedInterface->hostBlock->blockId:-1)
                        << " _isHead = " << _isHead << " _isEnd = " << _isEnd;
        scheduler->trace(info.str(),hostBlock->blockId);
#endif // verbose
        _isTrain = false;
        if (possibleRules && possibleRules->size()>0) {
            ReconnectTrainMessage *message = new ReconnectTrainMessage(true);
            scheduler->schedule(new NetworkInterfaceEnqueueOutgoingEvent(scheduler->now() + time_offset, message, _previous));
#ifdef verbose
            info.str("");
            info << "send ReconnectTrainMsg(true) to " << _previous->connectedInterface->hostBlock->blockId;
            scheduler->trace(info.str(),hostBlock->blockId,YELLOW);
#endif // verbose
        } else {
            ReconnectTrainMessage *message = new ReconnectTrainMessage(false);
            scheduler->schedule(new NetworkInterfaceEnqueueOutgoingEvent(scheduler->now() + time_offset, message, _next));
#ifdef verbose
            info.str("");
            info << "send ReconnectTrainMsg(false) to " << _next->connectedInterface->hostBlock->blockId;
            scheduler->trace(info.str(),hostBlock->blockId,YELLOW);
#endif // verbose
        }
        setRulesColor();
    }

}

void SbReconfBlockCode::createLine(uint64_t t,bool hol) {
	static PointCel tabDirections[4] = { PointCel(0,1),PointCel(1,0),PointCel(0,-1),PointCel(-1,0)};
#ifdef verbose
	stringstream info;
#endif // verbose
	Capability *capa = possibleRules->back()->capa;
	/*info.str("");
	info << "capa " << capa->name << " " << capa->isAngle;
	scheduler->trace(info.str(),hostBlock->blockId);*/
    _isHeadOfLine=hol;

	if (capa->isAngle || !_next || capa->isEnd) {
		PointCel motionVector;
		if (!_isHead && _previous) {
			motionVector = tabDirections[block->getDirection(_previous)];
		} else {
			motionVector = capa->tabMotions[0]->vect;
		}
		uint64_t t0 = scheduler->now(),
		t1 = t0+(t0-t)*5.0;
        if (_previous) {
			SetRDVMessage *message = new SetRDVMessage(t1,motionVector);
			scheduler->schedule(new NetworkInterfaceEnqueueOutgoingEvent(scheduler->now() + time_offset, message, _previous));
#ifdef verbose
			info.str("");
			info << "send SetRDVMessage("<<motionVector<<") to " << _previous->connectedInterface->hostBlock->blockId;
			scheduler->trace(info.str(),hostBlock->blockId);
#endif // verbose
		}
		startMotion(t1,motionVector,0,capa->tabUnlockPath);
	} else {
        CreateLineMessage *message = new CreateLineMessage(t);
		scheduler->schedule(new NetworkInterfaceEnqueueOutgoingEvent(scheduler->now() + time_offset, message, _next));
#ifdef verbose
		info.str("");
		info << "send CreateLineMessage to " << _next->connectedInterface->hostBlock->blockId;
		scheduler->trace(info.str(),hostBlock->blockId);
#endif // verbose
	}
}

void SbReconfBlockCode::sendMapToNeighbors(P2PNetworkInterface *p2pExcept) {
	static const int dirx[4]={0,1,0,-1}, diry[4]={1,0,-1,0};
	P2PNetworkInterface *p2p;
#ifdef verbose
	stringstream info;
#endif // verbose

	nbreOfWaitedAnswers=0;
	for(int i = North; i <= West; i++) {
		p2p = smartBlock->getInterface( NeighborDirection(i));
		if(p2p->connectedInterface && p2p!=p2pExcept) {
				MapMessage *message = new MapMessage(posGrid.x+dirx[i],posGrid.y+diry[i],gridSize[0],gridSize[1],_nbreGoalCells,targetGrid);
				scheduler->schedule(new NetworkInterfaceEnqueueOutgoingEvent(scheduler->now() + time_offset, message, p2p));
				nbreOfWaitedAnswers++;
#ifdef verbose
				info.str("");
				info << "send MapMessage to " << p2p->connectedInterface->hostBlock->blockId;
				scheduler->trace(info.str(),hostBlock->blockId);
#endif // verbose
		}
	}
}

void SbReconfBlockCode::sendAsk4EndToNeighbors(P2PNetworkInterface *p2pExcept) {
//	static const int dirx[4]={0,1,0,-1}, diry[4]={1,0,-1,0};
	P2PNetworkInterface *p2p;
#ifdef verbose
	stringstream info;
#endif // verbose

	nbreOfWaitedAnswers=0;
	_nbreWellPlacedBlocks=int(block->wellPlaced);
	_currentMove++;
	for(int i = North; i <= West; i++) {
		p2p = smartBlock->getInterface( NeighborDirection(i));
		if(p2p->connectedInterface && p2p!=p2pExcept) {
				Ask4EndMessage *message = new Ask4EndMessage(_currentMove);
				scheduler->schedule(new NetworkInterfaceEnqueueOutgoingEvent(scheduler->now() + time_offset, message, p2p));
				nbreOfWaitedAnswers++;
#ifdef verbose
				info.str("");
				info << "send Ask4EndMessage to " << p2p->connectedInterface->hostBlock->blockId;
				scheduler->trace(info.str(),hostBlock->blockId);
#endif // verbose
		}
	}
}

void SbReconfBlockCode::sendAns4EndMessage(P2PNetworkInterface *p2p,int value) {
	Ans4EndMessage *message = new Ans4EndMessage(value);
	scheduler->schedule(new NetworkInterfaceEnqueueOutgoingEvent(scheduler->now() + time_offset, message, p2p));
#ifdef verbose
	stringstream info;
	info.str("");
	info << "send Ans4EndMessage(" << value << "," << nbreOfWaitedAnswers << ") to " << p2p->connectedInterface->hostBlock->blockId;
	scheduler->trace(info.str(),hostBlock->blockId);
#endif // verbose
}


P2PNetworkInterface *SbReconfBlockCode::getBorderPreviousNeightborNoWellPlaced(P2PNetworkInterface *next) {
    //static int border[8][2] = {{0,1},{1,1},{1,0},{1,-1},{0,-1},{-1,-1},{-1,0},{-1,1}};
    static int border[8][2] = {{0,1},{-1,1},{-1,0},{-1,-1},{0,-1},{1,-1},{1,0},{1,1}};
//    OUTPUT << "getBorderPreviousNeightbor()" << endl;
//    OUTPUT << _pm;

    NeighborDirection dir;
    int i;
    // on recherche une cellule vide
    if (next) {
    // cherche la direction de next
        dir=North;
        i=4;
        while (i-- && block->getInterface(dir)!=next) {
            dir=NeighborDirection((int(dir)+1)%4);
        }
//        OUTPUT << "next :" << (i==-1?-1:dir) << endl;
        dir=NeighborDirection((int(dir)+3)%4);
        // on cherche une cellule pleine
        i=3;
        while (i-- && (block->getInterface(dir)->connectedInterface==NULL
                   || ((SmartBlocksBlock*)(block->getInterface(dir)->connectedInterface->hostBlock))->wellPlaced)) {
            dir=NeighborDirection((int(dir)+3)%4);
//		OUTPUT << dir << " ";
        }

    } else {
/************ on recherche le cas des isthmes ************/
/** existe-t-il un voisin avec un vide avant et un après */
        for (i=0; i<4; i++) {
            if (_pm.get(border[i*2][0],border[i*2][1])!=emptyCell &&
                _pm.get(border[(i*2+7)%8][0],border[(i*2+7)%8][1])==emptyCell &&
                _pm.get(border[(i*2+1)%8][0],border[(i*2+1)%8][1])==emptyCell) {
                dir=NeighborDirection(((8-2*i)%8)/2);
                return (block->getInterface(dir)->connectedInterface==NULL?NULL:block->getInterface(dir));
            }
        }

// on cherche une case vide
        i=7;
        while (i>=0 && _pm.get(border[i][0],border[i][1])!=emptyCell) {
            i--;
        }
        if (i==-1) return NULL;
        dir=NeighborDirection(((8-i)%8)/2);
//OUTPUT << "une case vide = " << i << "(" << border[i][0] << "," << border[i][1]<< ")" << endl;
// puis on cherche la case non vide la plus eloignée en tournant vers la gauche
        if (block->wellPlaced) {
            i=3;
            while (i-- && (block->getInterface(dir)->connectedInterface==NULL
                   || ((SmartBlocksBlock*)(block->getInterface(dir)->connectedInterface->hostBlock))->wellPlaced)) {
                dir=NeighborDirection((int(dir)+3)%4);
            }
        } else {
            i=3;
            while (i-- && (block->getInterface(dir)->connectedInterface==NULL)) {
                dir=NeighborDirection((int(dir)+3)%4);
            }
        }
    }
//OUTPUT << "previous :" << (block->getInterface(dir)->connectedInterface==NULL?-1:dir) << endl;
	return (block->getInterface(dir)->connectedInterface==NULL?NULL:block->getInterface(dir));
}

P2PNetworkInterface *SbReconfBlockCode::getBorderPreviousNeightbor(P2PNetworkInterface *next) {
    //static int border[8][2] = {{0,1},{1,1},{1,0},{1,-1},{0,-1},{-1,-1},{-1,0},{-1,1}};
    static int border[8][2] = {{0,1},{-1,1},{-1,0},{-1,-1},{0,-1},{1,-1},{1,0},{1,1}};
//    OUTPUT << "getBorderPreviousNeightbor()" << endl;
//    OUTPUT << _pm;

    NeighborDirection dir;
    int i;
    // on recherche une cellule vide
    if (next) {
    // cherche la direction de next
        dir=North;
        i=4;
        while (i-- && block->getInterface(dir)!=next) {
            dir=NeighborDirection((int(dir)+1)%4);
        }
//        OUTPUT << "next :" << (i==-1?-1:dir) << endl;
        dir=NeighborDirection((int(dir)+3)%4);
        // on cherche une cellule pleine
        i=3;
        while (i-- && block->getInterface(dir)->connectedInterface==NULL) {
            dir=NeighborDirection((int(dir)+3)%4);
//		OUTPUT << dir << " ";
        }

    } else {
/************ on recherche le cas des isthmes ************/
/** existe-t-il un voisin avec un vide avant et un après */
        for (i=0; i<4; i++) {
            if (_pm.get(border[i*2][0],border[i*2][1])!=emptyCell &&
                _pm.get(border[(i*2+7)%8][0],border[(i*2+7)%8][1])==emptyCell &&
                _pm.get(border[(i*2+1)%8][0],border[(i*2+1)%8][1])==emptyCell) {
                dir=NeighborDirection(((8-2*i)%8)/2);
                return (block->getInterface(dir)->connectedInterface==NULL?NULL:block->getInterface(dir));
            }
        }

// on cherche une case vide
        i=7;
        while (i>=0 && _pm.get(border[i][0],border[i][1])!=emptyCell) {
            i--;
        }
        if (i==-1) return NULL;
        dir=NeighborDirection(((8-i)%8)/2);
//OUTPUT << "une case vide = " << i << "(" << border[i][0] << "," << border[i][1]<< ")" << endl;
// puis on cherche la case non vide la plus eloignée en tournant vers la gauche
        if (block->wellPlaced) {
            i=3;
            while (i-- && block->getInterface(dir)->connectedInterface==NULL) {
                dir=NeighborDirection((int(dir)+3)%4);
            }
        } else {
            i=3;
            while (i-- && (block->getInterface(dir)->connectedInterface==NULL)) {
                dir=NeighborDirection((int(dir)+3)%4);
            }
        }
    }
//OUTPUT << "previous :" << (block->getInterface(dir)->connectedInterface==NULL?-1:dir) << endl;
	return (block->getInterface(dir)->connectedInterface==NULL?NULL:block->getInterface(dir));
}

P2PNetworkInterface *SbReconfBlockCode::getBorderNextNeightbor() {
	static int border[8][2] = {{0,1},{1,1},{1,0},{1,-1},{0,-1},{-1,-1},{-1,0},{-1,1}};
//    OUTPUT << "getBorderNextNeightbor()" << endl;
//    OUTPUT << _pm;

    // on recherche une cellule vide
    int i=0;
    while (i<8 && _pm.get(border[i][0],border[i][1])!=emptyCell) {
        i++;
    }
    NeighborDirection dir=NeighborDirection((i/2+1)%4);
//    OUTPUT << i << "," << dir << endl;

    i=3;
	while (i-- && block->getInterface(dir)->connectedInterface==NULL) {
		dir=NeighborDirection((int(dir)+1)%4);
//		OUTPUT << dir << " ";
	}
//	OUTPUT << dir << endl;
	return (block->getInterface(dir)->connectedInterface==NULL?NULL:block->getInterface(dir));
}

P2PNetworkInterface *SbReconfBlockCode::getBorderNextNeightborNoWellPlaced() {
	static int border[8][2] = {{0,1},{1,1},{1,0},{1,-1},{0,-1},{-1,-1},{-1,0},{-1,1}};
//    OUTPUT << "getBorderNextNeightbor()" << endl;
//    OUTPUT << _pm;

    // on recherche une cellule vide
    int i=0;
    while (i<8 && _pm.get(border[i][0],border[i][1])!=emptyCell) {
        i++;
    }
    NeighborDirection dir=NeighborDirection((i/2+1)%4);
//    OUTPUT << i << "," << dir << endl;

    i=3;
	while (i-- && (block->getInterface(dir)->connectedInterface==NULL
                   || ((SmartBlocksBlock*)(block->getInterface(dir)->connectedInterface->hostBlock))->wellPlaced)) {
		dir=NeighborDirection((int(dir)+1)%4);
//		OUTPUT << dir << " ";
	}
//	OUTPUT << dir << endl;
	return (block->getInterface(dir)->connectedInterface==NULL?NULL:block->getInterface(dir));
}

/*
void SbReconfBlockCode::sendNoActivity(NeighborDirection dir, int id) {
	stringstream info;

// on recherche le premier voisin dans le sens horaire
	dir=NeighborDirection((int(dir)+1)%4);
	int i=3;
	while (i-- && block->getInterface(dir)->connectedInterface==NULL) {
		dir=NeighborDirection((int(dir)+1)%4);
	}
	P2PNetworkInterface *p2p = block->getInterface(dir);

	NoActivityMessage *message = new NoActivityMessage(id);
	scheduler->schedule(new NetworkInterfaceEnqueueOutgoingEvent(scheduler->now() + time_offset, message, p2p));
	info.str("");
	info << "send NoActivityMessage("<< id <<") to " << p2p->connectedInterface->hostBlock->blockId;
	scheduler->trace(info.str(),hostBlock->blockId);
}
*/

void SbReconfBlockCode::sendAckMap(P2PNetworkInterface *p2p) {
	AckMapMessage *message = new AckMapMessage ();
	scheduler->schedule(new NetworkInterfaceEnqueueOutgoingEvent(scheduler->now() + time_offset, message, p2p));
#ifdef verbose
	stringstream info;
	info.str("");
	info << "send AckMapMessage(" << nbreOfWaitedAnswers << ") to " << p2p->connectedInterface->hostBlock->blockId;
	scheduler->trace(info.str(),hostBlock->blockId);
#endif // verbose
}

void SbReconfBlockCode::sendSearchBackHeadMessage(P2PNetworkInterface *dest,P2PNetworkInterface *except) {
#ifdef verbose
    stringstream info;
    info.str("");
    info << "send SearchBackHeadMessage(" << (except==NULL?-1:except->connectedInterface->hostBlock->blockId) << ") to " << dest->connectedInterface->hostBlock->blockId;
    scheduler->trace(info.str(),hostBlock->blockId);
#endif // verbose
    //setRulesColor();
    SearchBackHeadMessage *message = new SearchBackHeadMessage(except);
    scheduler->schedule(new NetworkInterfaceEnqueueOutgoingEvent(scheduler->now() + time_offset, message,dest));

    _isTrain = false;
    _isHead = false;
    block->_isBorder = false;
    block->setDisplayedValue(-1);

}

bool SbReconfBlockCode::testIsthmus(int dx,int dy) {
	int lx,ly;
	wrl->getGridSize(lx,ly);

/**
il faut aussi interdir <-XB0 ou B est un bord
**/
    SmartBlocksBlock *support;
    SmartBlocksBlock *supportDiag;
	SmartBlocksBlock *voisin;
//	SmartBlocksBlock *voisin2;
	// recherche la présence d'isthme de niveau 1
	if (dy==1) {
		support = (posGrid.x>0)?wrl->getGridPtr(posGrid.x-1,posGrid.y):NULL;
		supportDiag = (posGrid.x>0 && posGrid.y>0)?wrl->getGridPtr(posGrid.x-1,posGrid.y-1):NULL;
        voisin = (posGrid.y>0 && wrl->getGridPtr(posGrid.x,posGrid.y-1))?wrl->getGridPtr(posGrid.x,posGrid.y-1):NULL;
//        voisin2 = (posGrid.y>1 && wrl->getGridPtr(posGrid.x,posGrid.y-2))?wrl->getGridPtr(posGrid.x,posGrid.y-2):NULL;
	} else if (dy==-1) {
		support = (posGrid.x<lx-1)?wrl->getGridPtr(posGrid.x+1,posGrid.y):NULL;
		supportDiag = (posGrid.x<lx-1 && posGrid.y<ly-1)?wrl->getGridPtr(posGrid.x+1,posGrid.y+1):NULL;
        voisin = (posGrid.y<ly-1 && wrl->getGridPtr(posGrid.x,posGrid.y+1))?wrl->getGridPtr(posGrid.x,posGrid.y+1):NULL;
//        voisin2 = (posGrid.y<ly-2 && wrl->getGridPtr(posGrid.x,posGrid.y+2))?wrl->getGridPtr(posGrid.x,posGrid.y+2):NULL;
	} else if (dx==1) {
		support = (posGrid.y<ly-1)?wrl->getGridPtr(posGrid.x,posGrid.y+1):NULL;
		supportDiag = (posGrid.y<ly-1 && posGrid.x>0)?wrl->getGridPtr(posGrid.x-1,posGrid.y+1):NULL;
        voisin = (posGrid.x>0 && wrl->getGridPtr(posGrid.x-1,posGrid.y))?wrl->getGridPtr(posGrid.x-1,posGrid.y):NULL;
//        voisin2 = (posGrid.x>1 && wrl->getGridPtr(posGrid.x-2,posGrid.y))?wrl->getGridPtr(posGrid.x-2,posGrid.y):NULL;
    } else {
	// dx==-1
        support = (posGrid.y>0)?wrl->getGridPtr(posGrid.x,posGrid.y-1):NULL;
        supportDiag = (posGrid.y>0 && posGrid.x<lx-1)?wrl->getGridPtr(posGrid.x+1,posGrid.y-1):NULL;
        voisin = (posGrid.x<lx-1 && wrl->getGridPtr(posGrid.x+1,posGrid.y))?wrl->getGridPtr(posGrid.x+1,posGrid.y):NULL;
//        voisin2 = (posGrid.x<lx-2 && wrl->getGridPtr(posGrid.x+2,posGrid.y))?wrl->getGridPtr(posGrid.x+2,posGrid.y):NULL;
    }
    if (support && ((SbReconfBlockCode*)support->blockCode)->_isTrain) OUTPUT << "ISTHME SUPPORT("<< support->blockId <<")=TRAIN" << endl;
    if ((supportDiag && ((SbReconfBlockCode*)supportDiag->blockCode)->_isTrain)) OUTPUT << "ISTHME SUPPORT_DIAG("<< supportDiag->blockId <<")=TRAIN" << endl;
	if ((voisin && ((SbReconfBlockCode*)voisin->blockCode)->_isTrain)) OUTPUT << "ISTHME VOISIN("<< voisin->blockId <<")=TRAIN" << endl;

	return (support && ((SbReconfBlockCode*)support->blockCode)->_isTrain) ||
           (supportDiag && ((SbReconfBlockCode*)supportDiag->blockCode)->_isTrain) ||
           (voisin && ((SbReconfBlockCode*)voisin->blockCode)->_isTrain) ;/*||
           (voisin && !((SbReconfBlockCode*)voisin->blockCode)->_isTrain && voisin2==NULL);*/
}

void SbReconfBlockCode::createBorder() {
	wrl->getPresenceMatrix(posGrid,_pm);
	block->_isBorder = _pm.isBorder();

	block->setDisplayedValue(-1);

#ifdef verbose
    stringstream info;
	info.str("");
	info << "block->_isBorder = " << block->_isBorder;
    scheduler->trace(info.str(),hostBlock->blockId,GREEN);
#endif // verbose
	if (block->_isBorder) {
		setRulesColor();
		_previous = getBorderPreviousNeightbor(NULL);
		applyRules();
		if (possibleRules && !possibleRules->empty()) {
			Capability *capa = possibleRules->back()->capa;
			if (!capa->isHead && capa->linkPrevPos) {
				_previous = block->getP2PNetworkInterfaceByRelPos(*capa->linkPrevPos);
#ifdef verbose
				info.str("");
				info << "previous = " << *capa->linkPrevPos << "," << ((_previous->connectedInterface)?_previous->connectedInterface->hostBlock->blockId:-1);
				scheduler->trace(info.str(),hostBlock->blockId);
#endif // verbose
			} else _previous=NULL;
			if (!capa->isEnd && capa->linkNextPos) {
				PointCel np = *capa->linkNextPos;
				_next = block->getP2PNetworkInterfaceByRelPos(np);
#ifdef verbose
				info.str("");
				info << "next = " << np << "," << (_next->connectedInterface?_next->connectedInterface->hostBlock->blockId:-1);
				scheduler->trace(info.str(),hostBlock->blockId);
#endif // verbose
			} else _next=NULL;

			if (possibleRules->back()->isZeroDistance) {
				// send searchHeadMessage
				SearchHeadMessage *message = new SearchHeadMessage();
				scheduler->schedule(new NetworkInterfaceEnqueueOutgoingEvent(scheduler->now() + time_offset, message, _previous));
#ifdef verbose
				info.str("");
				info << "send SearchHeadMessage to " << _previous->connectedInterface->hostBlock->blockId;
				scheduler->trace(info.str(),hostBlock->blockId);
#endif // verbose
			}
			//block->setDisplayedValue(possibleRules->back()->gain);

			// Gère les messages en attente
			if (tabMemorisedMessages[2]) {
				step2(tabMemorisedMessages[2]);
				tabSteps[2]=false;
			}
			if (tabMemorisedMessages[3]) {
				step3(tabMemorisedMessages[3]);
				tabSteps[3]=false;
			}
		} else {
			_previous = getBorderPreviousNeightbor(NULL);
            _next = getBorderNextNeightbor();
#ifdef verbose
            info.str("");
            info << "no rule, previous = " << ((_previous->connectedInterface)?_previous->connectedInterface->hostBlock->blockId:-1) <<
                    " next = " << ((_next->connectedInterface)?_next->connectedInterface->hostBlock->blockId:-1);
			scheduler->trace(info.str(),hostBlock->blockId);
#endif // verbose
		}
	}
}

/*
void SbReconfBlockCode::sendInitToNeighbors(P2PNetworkInterface *p2pExcept,int stage) {
	P2PNetworkInterface *p2p;
	stringstream info;

	nbreOfWaitedAnswers=0;
	for(int i = North; i <= West; i++) {
		p2p = smartBlock->getInterface( NeighborDirection(i));
		if(p2p->connectedInterface && p2p!=p2pExcept) {
				ReInitMessage *message = new ReInitMessage(stage);
				scheduler->schedule(new NetworkInterfaceEnqueueOutgoingEvent(scheduler->now() + time_offset, message, p2p));
				nbreOfWaitedAnswers++;
				info.str("");
				info << "send ReInitMessage to " << p2p->connectedInterface->hostBlock->blockId;
				scheduler->trace(info.str(),hostBlock->blockId);
		}
	}
}

void SbReconfBlockCode::sendAckInit(P2PNetworkInterface *p2p) {
	AckInitMessage *message = new AckInitMessage();
	scheduler->schedule(new NetworkInterfaceEnqueueOutgoingEvent(scheduler->now() + time_offset, message, p2p));
	stringstream info;
	info.str("");
	info << "send AckInitMessage(" << nbreOfWaitedAnswers << ") to " << p2p->connectedInterface->hostBlock->blockId;
	scheduler->trace(info.str(),hostBlock->blockId);
}
*/

MapMessage::MapMessage(int x,int y,int w,int h,int n,SmartBlocks::presence *tg):Message(){
	id = MAP_MSG_ID;
	posx = x;
	posy = y;
	gridw = w;
	gridh = h;
	nbreGoalCells = n;
	targetGrid = tg;
}

MapMessage::~MapMessage() {
}

AckMapMessage::AckMapMessage():Message(){
	id = ACKMAP_MSG_ID;
}

AckMapMessage::~AckMapMessage() {
}

SearchHeadMessage::SearchHeadMessage():Message(){
	id = HEAD_MSG_ID;
}

SearchHeadMessage::~SearchHeadMessage() {
}

SearchBackHeadMessage::SearchBackHeadMessage(P2PNetworkInterface *except):Message(){
    exceptionBlock = except;
	id = HBCK_MSG_ID;
}

SearchBackHeadMessage::~SearchBackHeadMessage() {
}

SearchEndTrainMessage::SearchEndTrainMessage(int n):Message(){
	id = END_MSG_ID;
	num = n;
}

SearchEndTrainMessage::~SearchEndTrainMessage() {
}

TrainReadyMessage::TrainReadyMessage(bool qf):Message(){
	id = TRAIN_READY_MSG_ID;
	queueFound = qf;
}

TrainReadyMessage::~TrainReadyMessage() {
}

CreateLineMessage::CreateLineMessage(uint64_t t):Message(){
	id = CREATE_LINE_MSG_ID;
	etime = t;
}

CreateLineMessage::~CreateLineMessage() {
}

SetRDVMessage::SetRDVMessage(uint64_t t,const SmartBlocks::PointCel &v):Message(){
	id = SET_RDV_MSG_ID;
	rdvTime = t;
	motionVector = v;
}

SetRDVMessage::~SetRDVMessage() {
}

UnlockMessage::UnlockMessage(short *t,int n,int s):Message(){
	id = UNLOCK_MSG_ID;
	if (n>0) {
		tab = new short[n];
		memcpy(tab,t,n*sizeof(short));
		sz = n;
	} else {
		tab=NULL;
		sz=0;
	}
	step = s;
}

UnlockMessage::~UnlockMessage() {
	delete [] tab;
}

ReconnectTrainMessage::ReconnectTrainMessage(bool hr):Message(){
	id = RECONNECT_MSG_ID;
	hasRule=hr;
}

ReconnectTrainMessage::~ReconnectTrainMessage() {
}

/*
DisableTrainMessage::DisableTrainMessage():Message(){
	id = DISABLE_MSG_ID;
}

DisableTrainMessage::~DisableTrainMessage() {
}

NoActivityMessage::NoActivityMessage(int sid):Message() {
	id = NOACTIVITY_MSG_ID;
	senderID=sid;
}

NoActivityMessage::~NoActivityMessage() {
}

ReInitMessage::ReInitMessage(int s):Message(){
	id = REINIT_MSG_ID;
	stage=s;
}

ReInitMessage::~ReInitMessage() {
}

AckInitMessage::AckInitMessage():Message(){
	id = ACKINIT_MSG_ID;
}

AckInitMessage::~AckInitMessage() {
}*/

SingleMoveMessage::SingleMoveMessage(short *t,int n,uint64_t st,const SmartBlocks::PointCel &mv,const vector<short>&up,int s):Message() {
	id = SINGLEMV_MSG_ID;
	if (n>0) {
		tab = new short[n];
		memcpy(tab,t,n*sizeof(short));
		sz = n;
	} else {
		tab=NULL;
		sz=0;
	}
	startTime = st;
	motionVector = mv;
	step = s;
	// copy up dans unlockPath
	unlockPath = up;
}

SingleMoveMessage::~SingleMoveMessage() {
	delete [] tab;
}

Ask4EndMessage::Ask4EndMessage(int cm):Message(){
	id = ASK4END_MSG_ID;
	currentMove = cm;
}

Ask4EndMessage::~Ask4EndMessage() {
}

Ans4EndMessage::Ans4EndMessage(int n):Message() {
    nbreWellPlaced = n;
	id = ANS4END_MSG_ID;
}

Ans4EndMessage::~Ans4EndMessage() {
}

/****************************************************/
void SbReconfBlockCode::getLocalTargetGrid(const PointCel &pos,PresenceMatrix &pm) {
    presence *gpm=pm.grid;
    presence *tg = targetGrid;

    for (int i=0; i<9; i++) { *gpm++ = wallCell; };

    int ix0 = (pos.x<1)?1-pos.x:0,
        ix1 = (pos.x>gridSize[0]-2)?gridSize[0]-pos.x+1:3,
        iy0 = (pos.y<1)?1-pos.y:0,
        iy1 = (pos.y>gridSize[1]-2)?gridSize[1]-pos.y+1:3,
        ix,iy;

    for (iy=iy0; iy<iy1; iy++) {
        gpm = pm.grid+(iy*3+ix0);
        tg = targetGrid+(ix0+pos.x-1+(iy+pos.y-1)*gridSize[0]);
        for (ix=ix0; ix<ix1; ix++) {
            *gpm++ = *tg++;
        }
    }
}

