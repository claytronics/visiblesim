/*
 * CodeBlocAgents01.h
 *
 *  Created on: 24 march. 2012
 *      Author: dom
 */

#ifndef CODEBLOCAGENT01_H_
#define CODEBLOCAGENT01_H_

#include "CodeBloc.h"
#include "Blocs.h"
#include "Message.h"

#define TYPE_MSG_DISCOVER_AREA_REQUEST				2000
#define TYPE_MSG_DISCOVER_AREA_REPLY				2001

enum discoverState_E {UNKNOWN, WAITING, EXPLORED};

class CodeBlocAgents01;

class MessageDiscoverAreaRequest : public Message {
public:
	unsigned int sourceID;
	int requestID;
	int posX,posY;
	int agentID;
	int distance;
	int xmin, ymin, xmax, ymax;
	MessageDiscoverAreaRequest(unsigned int sourceID, int requestID, int agentID, int x, int y, int distance, int xmin, int ymin, int xmax, int ymax);
};

class MessageDiscoverAreaReply : public Message {
public:
	int posX,posY;
	int agentID;
	int agentX, agentY;
	int distance;
	MessageDiscoverAreaReply(int agentID, int agentX, int agentY, int x, int y, int distance);
};

class RoutingTableEntry {
public:
	int distance;
	InterfaceReseau *interface;
	int requestID;

	RoutingTableEntry(InterfaceReseau *interface, int distance, int requestID) {
		this->distance = distance;
		this->interface = interface;
		this->requestID = requestID;
	}
};

//===========================================================================================================
//
//          Agent  (class)
//
//===========================================================================================================

class Agent {
private:
	static int controledAreaWidth;
	static int controledAreaHeight;
	static int nextID;
public:
	int agentID;

	CodeBlocAgents01 *codeBloc;
	int controledAreaXMin, controledAreaYmin;
	int controledAreaXMax, controledAreaYmax;
	int localX, localY;
	bool *localMap;

	Agent(CodeBlocAgents01 *b, int locX, int locY);
	~Agent();

	void takeControlOfArea();
};


//===========================================================================================================
//
//          CodeBlocAgents01  (class)
//
//===========================================================================================================

class CodeBlocAgents01 : public CodeBloc {
public:
	Agent *agent;
	int localX, localY;

	int masterAgentID;

	map<int, RoutingTableEntry> localRoutingTable;
	map<int, RoutingTableEntry> northRoutingTable;
	map<int, RoutingTableEntry> southRoutingTable;
	map<int, RoutingTableEntry> eastRoutingTable;
	map<int, RoutingTableEntry> westRoutingTable;

	int currentRequestID;
	discoverState_E northExplored, southExplored, eastExplored, westExplored;

	CodeBlocAgents01();
	void demarre();
	void traiteEvenementLocal(Evenement *ev);
	void broadcastDiscoverAreaRequest(MessageDiscoverAreaRequest *message);
};


#endif /* CODEBLOCAGENT01_H_ */
