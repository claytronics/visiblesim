/*
 * CodeBlocDemo.cpp
 *
 *  Created on: 1 févr. 2012
 *      Author: dom
 */

#include <iostream>
#include <sstream>
#include "CodeBlocAgents01.h"
#include "Scheduler.h"

using namespace std;

MessageDiscoverAreaRequest::MessageDiscoverAreaRequest(unsigned int sourceID, int requestID, int agentID, int x, int y, int distance, int xmin, int ymin, int xmax, int ymax) : Message() {
	typeMessage = TYPE_MSG_DISCOVER_AREA_REQUEST;
	posX = x;
	posY = y;
	this->sourceID = sourceID;
	this->requestID = requestID;
	this->distance = distance;
	this->agentID = agentID;
	this->xmin = xmin;
	this->ymin = ymin;
	this->xmax = xmax;
	this->ymax = ymax;
}

MessageDiscoverAreaReply::MessageDiscoverAreaReply(int agentID, int agentX, int agentY, int x, int y, int distance) : Message() {
	typeMessage = TYPE_MSG_DISCOVER_AREA_REPLY;
	posX = x;
	posY = y;
	this->distance = distance;
	this->agentID = agentID;
	this->agentX = agentX;
	this->agentY = agentY;
}
//===========================================================================================================
//
//          Agent  (class)
//
//===========================================================================================================

int Agent::controledAreaWidth = 3;
int Agent::controledAreaHeight = 3;

int Agent::nextID = 0;

Agent::Agent(CodeBlocAgents01 *cb, int locX, int locY) {
	int i,j;

	cout << "Constructeur Agent\n";

	agentID = nextID++;
	codeBloc = cb;
	controledAreaXMin = 0;
	controledAreaYmin = 0;
	controledAreaXMax = controledAreaWidth-1;
	controledAreaYmax = controledAreaHeight-1;
	localX = locX;
	localY = locY;
	localMap = new bool[controledAreaWidth * controledAreaHeight];

	for (i=0; i<controledAreaWidth; i++) {
		for (j=0; j<controledAreaHeight; j++) {
			localMap[i+j*controledAreaWidth] = false;
		}
	}

}

Agent::~Agent() {
	cout << "Destructeur Agent\n";
	delete [] localMap;
	localMap = NULL;
}

void Agent::takeControlOfArea() {

	cout << "Taking control of area" << endl;

	codeBloc->northExplored = UNKNOWN;
	codeBloc->southExplored = UNKNOWN;
	codeBloc->eastExplored = UNKNOWN;
	codeBloc->westExplored = UNKNOWN;

	codeBloc->currentRequestID++;

	MessageDiscoverAreaRequest *message = new MessageDiscoverAreaRequest(codeBloc->bloc->blocID, codeBloc->currentRequestID, agentID, localX,localY+1,0,0,0,controledAreaWidth-1,controledAreaHeight-1);
	message->interfaceDestination = NULL;
	codeBloc->broadcastDiscoverAreaRequest(message);
	delete(message);

	/*
	Bloc *bloc;

	bloc = codeBloc->bloc;
	uint64_t heureActuelle = Scheduler::getHeureActuelle();
	if (localY < (controledAreaHeight-1) && bloc->voisinNord != NULL) {
		Scheduler::schedule(new EvenementBlocEnvoieMessage(heureActuelle,
				new MessageDiscoverAreaRequest(agentID, localX, localY, localX,localY+1,1,0,0,controledAreaWidth-1,controledAreaHeight-1), bloc->interfaceReseauNord));
	}
	if (localY > 0 && bloc->voisinSud != NULL) {
		Scheduler::schedule(new EvenementBlocEnvoieMessage(heureActuelle,
				new MessageDiscoverAreaRequest(agentID, localX, localY, localX,localY-1,1,0,0,controledAreaWidth-1,controledAreaHeight-1), bloc->interfaceReseauSud));
	}
	if (localX < (controledAreaWidth-1) && bloc->voisinEst != NULL) {
		Scheduler::schedule(new EvenementBlocEnvoieMessage(heureActuelle,
				new MessageDiscoverAreaRequest(agentID, localX, localY, localX+1,localY,1,0,0,controledAreaWidth-1,controledAreaHeight-1), bloc->interfaceReseauEst));
	}
	if (localX > 0 && bloc->voisinOuest != NULL) {
		Scheduler::schedule(new EvenementBlocEnvoieMessage(heureActuelle,
				new MessageDiscoverAreaRequest(agentID, localX, localY, localX-1,localY,1,0,0,controledAreaWidth-1,controledAreaHeight-1), bloc->interfaceReseauOuest));
	}
	 */

}
//===========================================================================================================
//
//          CodeBlocAgents01  (class)
//
//===========================================================================================================

CodeBlocAgents01::CodeBlocAgents01() : CodeBloc() {
	cout << "Constructeur CodeBlocAgents01\n";

	currentRequestID=-1;
	northExplored = UNKNOWN;
	southExplored = UNKNOWN;
	eastExplored = UNKNOWN;
	westExplored = UNKNOWN;

	currentRequestID = -1;
	agent = NULL;
}

void CodeBlocAgents01::demarre() {

#if DEBUG_LEVEL > 3
	stringstream info;
	info << "Demarrage du CodeBlocAgents01 dans le bloc " << bloc->blocID;
	Scheduler::trace(info.str());
#endif

	GLfloat posX,posY;
	bloc->get2DPosition(posX,posY);
	if (posX == 8 && posY == 3) {
		bloc->setColor(255,255,0);
		localX = 2;
		localY = 0;
		agent = new Agent(this,localX,localY);
		agent->takeControlOfArea();
	}

}

void CodeBlocAgents01::traiteEvenementLocal(Evenement *ev) {
	if (ev->typeEvenement == TYPE_EV_BLOC_RECOIT_MESSAGE) {
		EvenementBlocRecoitMessage *evm = (EvenementBlocRecoitMessage*)ev;
		if (evm->message->typeMessage == TYPE_MSG_DISCOVER_AREA_REQUEST) {
			MessageDiscoverAreaRequest *message = (MessageDiscoverAreaRequest*)evm->message;

			if (message->distance > 0) bloc->setColor(0,0,255);
			localX = message->posX;
			localY = message->posY;

			broadcastDiscoverAreaRequest(message);

			/*
			bool hasToForward = false;

			posX = message->posX;
			posY = message->posY;
			if (masterAgentID != message->agentID) {
				localRoutingTable.clear();
				masterAgentID = message->agentID;
			}
			map<int, RoutingTableEntry>::iterator it;
			it = localRoutingTable.find(message->agentID);
			if (it == localRoutingTable.end() ) {
				cout << "Insertion d'une nouvelle route ";
				cout << "[ " << posX << "," << posY << " ] pour aller en [ " << message->agentX << "," << message->agentY << " ]";
				cout << "  distance : " << message->distance << endl;
				localRoutingTable.insert(pair<int,RoutingTableEntry>(message->agentID, RoutingTableEntry(message->interfaceSource, message->distance)));
				hasToForward = true;
			} else {
				if (it->second.distance > message->distance) {
					cout << "Chemin plus court trouvé" << endl;
					it->second.distance = message->distance;
					it->second.interface = message->interfaceDestination;
					hasToForward = true;
				}
			}

			if (hasToForward && !(message->agentX == message->posX && message->agentY == message->posY)) {
				cout << "forward request et construction reply (" << posX << "," << posY << ")" << endl;
				uint64_t heureActuelle = Scheduler::getHeureActuelle();

				Scheduler::schedule(new EvenementBlocEnvoieMessage(heureActuelle,
							new MessageDiscoverAreaReply(masterAgentID, message->agentX, message->agentY, posX,posY,message->distance),
							message->interfaceDestination));

				if (posY < message->ymax && bloc->voisinNord != NULL) {
					Scheduler::schedule(new EvenementBlocEnvoieMessage(heureActuelle,
							new MessageDiscoverAreaRequest(masterAgentID, message->agentX, message->agentY, posX,posY+1,message->distance+1,message->xmin,message->ymin,message->xmax,message->ymax),
							bloc->interfaceReseauNord));
				}
				if (posY > 0 && bloc->voisinSud != NULL) {
					Scheduler::schedule(new EvenementBlocEnvoieMessage(heureActuelle,
							new MessageDiscoverAreaRequest(masterAgentID, message->agentX, message->agentY, posX,posY-1,message->distance+1,message->xmin,message->ymin,message->xmax,message->ymax),
							bloc->interfaceReseauSud));
				}
				if (posX < message->xmax && bloc->voisinEst != NULL) {
					Scheduler::schedule(new EvenementBlocEnvoieMessage(heureActuelle,
							new MessageDiscoverAreaRequest(masterAgentID, message->agentX, message->agentY, posX+1,posY,message->distance+1,message->xmin,message->ymin,message->xmax,message->ymax),
							bloc->interfaceReseauEst));
				}
				if (posX > 0 && bloc->voisinOuest != NULL) {
					Scheduler::schedule(new EvenementBlocEnvoieMessage(heureActuelle,
							new MessageDiscoverAreaRequest(masterAgentID, message->agentX, message->agentY, posX-1,posY,message->distance+1,message->xmin,message->ymin,message->xmax,message->ymax),
							bloc->interfaceReseauOuest));
				}
			}
			if (message->distance > 0) bloc->setColor(0,0,255);
			*/
		}
		if (evm->message->typeMessage == TYPE_MSG_DISCOVER_AREA_REPLY) {
			MessageDiscoverAreaReply *message = (MessageDiscoverAreaReply*)evm->message;
/*
			if (agent != NULL) {
				cout << "++" << endl;
			}

			uint64_t heureActuelle = Scheduler::getHeureActuelle();
			map<int, RoutingTableEntry>::iterator it;
			it = localRoutingTable.find(message->agentID);
			if (it == localRoutingTable.end() ) {
				cout << "pas de route trouvée !" << endl;
			} else {
				cout << "forward reply (distance " << it->second.distance << ")" << endl;
				Scheduler::schedule(new EvenementBlocEnvoieMessage(heureActuelle,
										new MessageDiscoverAreaReply(masterAgentID, message->agentX, message->agentY, message->posX,message->posY,message->distance),
										it->second.interface));
			}
			*/
		}
	}
}


void CodeBlocAgents01::broadcastDiscoverAreaRequest(MessageDiscoverAreaRequest *message) {
	uint64_t heureActuelle = Scheduler::getHeureActuelle();
	map <int,RoutingTableEntry>::iterator it1;

	cout << "broadcast Discover" << endl;
	if (message->sourceID == bloc->blocID) {
		cout << "  Je suis la source du discoverRequest" << endl;
	}
	GLfloat globX, globY;
	bloc->get2DPosition(globX, globY);
	cout << "  ma position locale est [" << localX << "," << localY << "] (réelle : [" << globX << "," << globY << "])" << endl;

	if (message->requestID > currentRequestID) {
		northExplored = UNKNOWN;
		southExplored = UNKNOWN;
		eastExplored = UNKNOWN;
		westExplored = UNKNOWN;

		localRoutingTable.clear();
		northRoutingTable.clear();
		southRoutingTable.clear();
		eastRoutingTable.clear();
		westRoutingTable.clear();
	}

	// si au nord je sors de la zone ou que je n'ai pas de voisin
	if (localY >= message->ymax || bloc->voisinNord == NULL) {
		northExplored = EXPLORED;
	}
	// si la requete est arrivée par le nord
	if ( message->interfaceDestination != NULL && message->interfaceDestination->direction == NORD) {
		northExplored = EXPLORED;
		it1 = northRoutingTable.find(message->sourceID);
		if (it1 == northRoutingTable.end()) {
			northRoutingTable.insert(pair<int,RoutingTableEntry>(message->sourceID,RoutingTableEntry(bloc->interfaceReseauNord,message->distance,message->requestID)));
		} else {
			if  (it1->second.distance > message->distance) {
				it1->second.distance = message->distance;
				it1->second.interface = bloc->interfaceReseauNord;
				it1->second.requestID = message->requestID;
			}
		}
	}

	if (northExplored == UNKNOWN) {
		northExplored = WAITING;
		cout << "  exploration du nord" << endl;

		northRoutingTable.insert(pair<int,RoutingTableEntry>(message->sourceID,RoutingTableEntry(bloc->interfaceReseauNord,message->distance+1,message->requestID)));

		Scheduler::schedule(new EvenementBlocEnvoieMessage(heureActuelle,
				new MessageDiscoverAreaRequest(message->sourceID, message->requestID, message->agentID, localX, localY+1,message->distance+1,message->xmin,message->ymin,message->xmax,message->ymax), bloc->interfaceReseauNord));
	}

	if (northExplored == EXPLORED && southExplored == EXPLORED && eastExplored == EXPLORED && westExplored == EXPLORED) {
		cout << "  Plus de voisins à explorer" << endl;
	}
	/*
	if (localY < (controledAreaHeight-1) && bloc->voisinNord != NULL) {
		Scheduler::schedule(new EvenementBlocEnvoieMessage(heureActuelle,
				new MessageDiscoverAreaRequest(agentID, localX, localY, localX,localY+1,1,0,0,controledAreaWidth-1,controledAreaHeight-1), bloc->interfaceReseauNord));
	}
	if (localY > 0 && bloc->voisinSud != NULL) {
		Scheduler::schedule(new EvenementBlocEnvoieMessage(heureActuelle,
				new MessageDiscoverAreaRequest(agentID, localX, localY, localX,localY-1,1,0,0,controledAreaWidth-1,controledAreaHeight-1), bloc->interfaceReseauSud));
	}
	if (localX < (controledAreaWidth-1) && bloc->voisinEst != NULL) {
		Scheduler::schedule(new EvenementBlocEnvoieMessage(heureActuelle,
				new MessageDiscoverAreaRequest(agentID, localX, localY, localX+1,localY,1,0,0,controledAreaWidth-1,controledAreaHeight-1), bloc->interfaceReseauEst));
	}
	if (localX > 0 && bloc->voisinOuest != NULL) {
		Scheduler::schedule(new EvenementBlocEnvoieMessage(heureActuelle,
				new MessageDiscoverAreaRequest(agentID, localX, localY, localX-1,localY,1,0,0,controledAreaWidth-1,controledAreaHeight-1), bloc->interfaceReseauOuest));
	}
	 */


}

