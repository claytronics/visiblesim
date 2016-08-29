/*
 * blinkyBlocksSimulator.cpp
 *
 *  Created on: 23 mars 2013
 *      Author: dom
 */

#include <iostream>
#include <string.h>

#include "blinkyBlocksSimulator.h"
#include "trace.h"

using namespace std;

namespace BlinkyBlocks {

BlinkyBlocksSimulator::BlinkyBlocksSimulator(int argc, char *argv[], BlockCodeBuilder bcb)
	: BaseSimulator::Simulator(argc, argv, bcb) {
	OUTPUT << "\033[1;34m" << "BlinkyBlocksSimulator constructor" << "\033[0m" << endl;
}

// PTHY: TODO: Refactor / Genericize
void BlinkyBlocksSimulator::parseScenario() {
	// // loading the scenario
	// TiXmlNode *nodeScenario = xmlWorldNode->FirstChild("scenario");
	// if (nodeScenario) {
	// 	bool autostart=false;
	// 	TiXmlElement* element = nodeScenario->ToElement();
	// 	const char *attr= element->Attribute("autostart");
	// 	if (attr) {
	// 		string str(attr);
	// 		autostart=(str=="True" || str=="true");
	// 	}
	// 	OUTPUT << "SCENARIO: Autostart=" << autostart << endl;
	// 	/* Reading an event */
	// 	nodeScenario = nodeScenario->FirstChild("event");
	// 	float eventTime =0.0;
	// 	int eventBlockId=-1;
	// 	while (nodeScenario) {
	// 		element = nodeScenario->ToElement();
	// 		attr = element->Attribute("time");
	// 		if (attr) {
	// 			eventTime = atof(attr);
	// 		}
	// 		attr = element->Attribute("type");
	// 		if (attr) {
	// 			string strAttr(attr);
	// 			if (strAttr=="tap") {
	// 				attr = element->Attribute("id");
	// 				eventBlockId=-1;
	// 				if (attr) {
	// 					eventBlockId=atoi(attr);
	// 				}
	// 				if (eventBlockId==-1) {
	// 					ERRPUT << "SCENARIO:No id for tap event" << endl;
	// 				} else {
	// 					OUTPUT << "SCENARIO: tap(" << eventTime << "," << eventBlockId << ")" << endl;
	// 					((BlinkyBlocksWorld*)world)->addScenarioEvent(new ScenarioTapEvent(eventTime,eventBlockId));
	// 				}
	// 			} else if (strAttr=="debug") {
	// 				attr = element->Attribute("id");
	// 				bool open=true;
	// 				if (attr) {
	// 					string str(attr);
	// 					open = (str=="true" || str=="True");
	// 				}
	// 				OUTPUT << "SCENARIO: debug(" << eventTime << "," << open << ")" << endl;
	// 				((BlinkyBlocksWorld*)world)->addScenarioEvent(new ScenarioDebugEvent(eventTime,open));
	// 			} else if (strAttr=="selectBlock") {
	// 				attr = element->Attribute("id");
	// 				eventBlockId=-1;
	// 				if (attr) {
	// 					eventBlockId=atoi(attr);
	// 				}
	// 				OUTPUT << "SCENARIO: selectBlock(" << eventTime << "," << eventBlockId << ")" << endl;
	// 				((BlinkyBlocksWorld*)world)->addScenarioEvent(new ScenarioSelectBlockEvent(eventTime,eventBlockId));
	// 			} else if (strAttr=="addBlock") {
	// 				attr = element->Attribute("position");
	// 				if (attr) {
	// 					string str(attr);
	// 					int pos1 = str.find_first_of(','),
	// 						pos2 = str.find_last_of(',');
	// 					Vector3D position;
	// 					position.pt[0] = atoi(str.substr(0,pos1).c_str());
	// 					position.pt[1] = atoi(str.substr(pos1+1,pos2-pos1-1).c_str());
	// 					position.pt[2] = atoi(str.substr(pos2+1,str.length()-pos1-1).c_str());
	// 					OUTPUT << "SCENARIO: addBlock(" << eventTime << "," << position << ")" << endl;
	// 					((BlinkyBlocksWorld*)world)->addScenarioEvent(new ScenarioAddBlockEvent(eventTime,position));
	// 				} else {
	// 					ERRPUT << "SCENARIO: No position for addBlock event" << endl;
	// 				}

	// 			} else {
	// 				ERRPUT << "SCENARIO: event '" << attr << "': unknown !" << endl;
	// 			}

	// 		} else {
	// 			ERRPUT << "SCENARIO: no Event type " << endl;
	// 		}
	// 		nodeScenario = nodeScenario->NextSibling("event");
	// 	} // while(nodeScenario)
	// }
}

BlinkyBlocksSimulator::~BlinkyBlocksSimulator() {
	OUTPUT << "\033[1;34m" << "BlinkyBlocksSimulator destructor" << "\033[0m" <<endl;
}

void BlinkyBlocksSimulator::createSimulator(int argc, char *argv[], BlockCodeBuilder bcb) {
	simulator =  new BlinkyBlocksSimulator(argc, argv, bcb);
	simulator->parseConfiguration(argc, argv);
	simulator->startSimulation();
}

void BlinkyBlocksSimulator::loadWorld(const Cell3DPosition &gridSize, const Vector3D &gridScale,
									  int argc, char *argv[]) {
	world = new BlinkyBlocksWorld(gridSize, gridScale, argc,argv);

	if (GlutContext::GUIisEnabled)
		world->loadTextures("../../simulatorCore/resources/textures/latticeTextures");

	World::setWorld(world);
}

void BlinkyBlocksSimulator::loadBlock(TiXmlElement *blockElt, bID blockId, BlockCodeBuilder bcb,
									  const Cell3DPosition &pos, const Color &color, bool master) {

	// Any additional configuration file parsing exclusive to this type of block should be performed
	//  here, using the blockElt TiXmlElement.

	// ...Parsing code...

	// Finally, add block to the world
	((BlinkyBlocksWorld*)world)->addBlock(blockId, bcb, pos, color, 0, master);
}

} // BlinkyBlocks namespace
