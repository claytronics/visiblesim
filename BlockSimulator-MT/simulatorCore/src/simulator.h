/*
 * simulator.h
 *
 *  Created on: 14 févr. 2013
 *      Author: dom
 */

#ifndef SIMULATOR_H_
#define SIMULATOR_H_

#include "world.h"
#define TIXML_USE_STL	1
#include "TinyXML/tinyxml.h"
#include "scheduler.h"

using namespace std;

class Essai {
public:
	Essai() { };
	~Essai() { };
};

class Simulator {
protected:
	TiXmlDocument *xmlDoc;
	TiXmlNode* xmlWorldNode;
	Scheduler *scheduler;
	static Simulator *simulator;
public:
	static World *world;

	Simulator(int argc, char *argv[]);
	virtual ~Simulator();

	virtual void initWorld() = 0;
	static Simulator *getInstance() { return(simulator); }
};

#endif /* SIMULATOR_H_ */
