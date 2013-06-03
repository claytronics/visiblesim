/*
 * simulator.cpp
 *
 *  Created on: 14 févr. 2013
 *      Author: dom
 */

#include <iostream>
#include "simulator.h"
#include <GL/freeglut.h>

using namespace std;

World *Simulator::world=NULL;
Simulator *Simulator::simulator=NULL;

Simulator::Simulator(int argc, char *argv[]) {
	xmlWorldNode = NULL;
	simulator = this;

	cout << "\033[1;34m" << "Simulator constructor" << "\033[0m" << endl;

	string confFileName = "config.xml";

	if (argc>=2) {
		confFileName= argv[1];
	}

	xmlDoc = new TiXmlDocument(confFileName.c_str());

	bool isLoaded = xmlDoc->LoadFile();

	if ( !isLoaded) {
		cerr << "\033[1;31m" << "Could not load configuration file :" << confFileName << "\033[0m" << endl;
		exit(1);
	} else {
		xmlWorldNode = xmlDoc->FirstChild("world");
		if (xmlWorldNode) {
			cout << "\033[1;34m  " << confFileName << " successfully loaded "<< "\033[0m" << endl;
		} else {
			cerr << "\033[1;31m" << "Could not find root 'world' element in configuration file" << "\033[0m" << endl;
			exit(1);
		}
	}

	GraphicContext::init(&argc, argv);
//	scheduler = new Scheduler();

}

Simulator::~Simulator() {
	cout << "\033[1;34m"  << "Simulator destructor" << "\033[0m" << endl;

	if (world != NULL) delete(world);

	if (scheduler != NULL) delete(scheduler);

	cout << "Events(s) left in memory : " << Event::getNbEvents() << endl;
	cout << "Message(s) left in memory : " << Message::getNbMessages() << endl;
}
