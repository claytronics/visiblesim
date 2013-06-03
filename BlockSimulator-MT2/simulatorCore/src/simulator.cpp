/*
 * simulator.cpp
 *
 *  Created on: 22 mars 2013
 *      Author: dom
 */

#include "simulator.h"

using namespace std;

namespace BaseSimulator {

Simulator* simulator = NULL;

Simulator* Simulator::simulator = NULL;

Simulator::Simulator(int argc, char *argv[]) {
	if (simulator == NULL) {
		simulator = this;
		BaseSimulator::simulator = simulator;
	} else {
		cerr << "\033[1;31m" << "Only one Simulator instance can be created, aborting !" << "\033[0m" << endl;
		exit(EXIT_FAILURE);
	}

	xmlWorldNode = NULL;

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
}

Simulator::~Simulator() {
	cout << "\033[1;34m"  << "Simulator destructor" << "\033[0m" << endl;
	//MODIF NICO : le mieux serait de faire ce delete juste après avoir fini de lire le fichier xml
	delete xmlDoc;
	//FIN MODIF NICO
	deleteScheduler();
	deleteWorld();
}

} // Simulator namespace
