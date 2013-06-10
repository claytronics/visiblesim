/*
 * blinkyBlocksSimulator.cpp
 *
 *  Created on: 23 mars 2013
 *      Author: dom
 */

#include <iostream>
#include "blinkyBlocksSimulator.h"

using namespace std;

namespace BlinkyBlocks {

BlinkyBlocksBlockCode*(* BlinkyBlocksSimulator::buildNewBlockCode)(BlinkyBlocksBlock*)=NULL;

BlinkyBlocksSimulator::BlinkyBlocksSimulator(int argc, char *argv[], BlinkyBlocksBlockCode *(*blinkyBlocksBlockCodeBuildingFunction)(BlinkyBlocksBlock*)) : BaseSimulator::Simulator(argc, argv) {
	cout << "\033[1;34m" << "BlinkyBlocksSimulator constructor" << "\033[0m" << endl;
	buildNewBlockCode = blinkyBlocksBlockCodeBuildingFunction;

	/* BPI */
	int currentID = 1;
	BlinkBlocksWorld *world=NULL;
	/* reading the xml file */
	TiXmlNode *node = xmlDoc->FirstChild("world");
	if (node) {
		TiXmlElement* worldElement = node->ToElement();
		string str = worldElement->Attribute("gridsize");
		int pos = str.find_first_of(',');
		int largeur = atoi(str.substr(0,pos).c_str());
		int hauteur = atoi(str.substr(pos+1,str.length()-pos-1).c_str());
		cout << "grid size : " << largeur << " x " << hauteur << endl;

		createWorld(largeur,hauteur);
		world = getWorld();
		//world->loadTextures("../../simulatorCore/smartBlocksTextures");
	} else {
		cerr << "ERROR : NO world in XML file" << endl;
		exit(1);
	}

    	TiXmlNode *nodeBlock = node->FirstChild("blockList");
	if (nodeBlock) {
		Vecteur defaultColor(0.8,0.8,0.8);
		TiXmlElement* element = nodeBlock->ToElement();
		const char *attr= element->Attribute("color");
		if (attr) {
			string str(attr);
			int pos1 = str.find_first_of(','),
			pos2 = str.find_last_of(',');
			defaultColor.pt[0] = atof(str.substr(0,pos1).c_str())/255.0;
			defaultColor.pt[1] = atof(str.substr(pos1+1,pos2-pos1-1).c_str())/255.0;
			defaultColor.pt[2] = atof(str.substr(pos2+1,str.length()-pos1-1).c_str())/255.0;
		}

		/* Reading a BlinkyBlock */
		cout << "default color :" << defaultColor << endl;
		nodeBlock = nodeBlock->FirstChild("block");
		Vecteur color,position;
		 while (nodeBlock) {
		   element = nodeBlock->ToElement();
		   color = defaultColor;
		   attr = element->Attribute("color");
		   if (attr) {
			  string str(attr);
		      int pos1 = str.find_first_of(','),
		   		   pos2 = str.find_last_of(',');
			   color.pt[0] = atof(str.substr(0,pos1).c_str())/255.0;
		   	   color.pt[1] = atof(str.substr(pos1+1,pos2-pos1-1).c_str())/255.0;
			   color.pt[2] = atof(str.substr(pos2+1,str.length()-pos1-1).c_str())/255.0;
			   cout << "color :" << defaultColor << endl;
			}
			attr = element->Attribute("position");
			if (attr) {
				string str(attr);
			    int pos = str.find_first_of(',');
			   	position.pt[0] = atoi(str.substr(0,pos).c_str());
			   	position.pt[1] = atoi(str.substr(pos+1,str.length()-pos-1).c_str());
			   	cout << "position : " << position << endl;
			}

			world->addBlock(currentID++,BlinkyBlocksSimulator::buildNewBlockCode,position,color);
			nodeBlock = nodeBlock->NextSibling("block");
		 } // end while (nodeBlock)
		} else // end if(nodeBlock)
		{ cerr << "no Block List" << endl;

		}
	createScheduler(); // launch the scheduler, including server for tcp connections
}

BlinkyBlocksSimulator::~BlinkyBlocksSimulator() {
	cout << "\033[1;34m" << "BlinkyBlocksSimulator destructor" << "\033[0m" <<endl;
}

void BlinkyBlocksSimulator::createSimulator(int argc, char *argv[], BlinkyBlocksBlockCode *(*blinkyBlocksBlockCodeBuildingFunction)(BlinkyBlocksBlock*)) {
	simulator =  new BlinkyBlocksSimulator(argc, argv, blinkyBlocksBlockCodeBuildingFunction);
}

void BlinkyBlocksSimulator::deleteSimulator() {
	delete((BlinkyBlocksSimulator*)simulator);
}

} // BlinkyBlocks namespace
