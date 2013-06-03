/*
 * multiCoresSimulator.cpp
 *
 *  Created on: 14 févr. 2013
 *      Author: dom
 */

#include <iostream>
#include <boost/algorithm/string.hpp>

#include "multiCoresSimulator.h"
#include "multiCoresWorld.h"
#include "multiCoresBlock.h"


using namespace std;

MultiCoresBlockCode*(* MultiCoresSimulator::buildNewBlockCode)(MultiCoresBlock*)=NULL;

MultiCoresSimulator::MultiCoresSimulator(int argc, char *argv[], MultiCoresBlockCode *(*multiCoreBlockCodeBuildingFunction)(MultiCoresBlock*)) : Simulator(argc, argv) {
	cout << "\033[1;34m" << "MultiCoresSimulator constructor" << "\033[0m" << endl;

	buildNewBlockCode = multiCoreBlockCodeBuildingFunction;

	world = (World*)new MultiCoresWorld();
	GraphicContext.setWorld(world);

	scheduler = new Scheduler();
	TiXmlElement* element;
	vector<string> tokenizedAttribute;

	float blockX=0, blockY=0, blockZ=0;
	float blockRed=1, blockGreen=1, blockBlue=1;
	const char *attributeValue;
	unsigned int blockId=0;

	if (xmlWorldNode != NULL) {
		TiXmlNode *nodeBlocks = xmlWorldNode->FirstChild("blockList");
		if (nodeBlocks) {
			TiXmlNode *nodeBlock = nodeBlocks->FirstChild("block");
			while (nodeBlock != NULL) {
				cout << "  block  ";
				element = nodeBlock->ToElement();

				attributeValue= element->Attribute("position");
				if (attributeValue) {
					boost::split(tokenizedAttribute, attributeValue, boost::is_any_of(",;"));
					if (tokenizedAttribute.size() > 0) blockX = atof(tokenizedAttribute[0].c_str());
					if (tokenizedAttribute.size() > 1) blockY = atof(tokenizedAttribute[1].c_str());
					if (tokenizedAttribute.size() > 2) blockZ = atof(tokenizedAttribute[2].c_str());
					cout << "[" << blockX << "," << blockY << "," << blockZ << "]";
				}
				attributeValue= element->Attribute("color");
				if (attributeValue) {
					boost::split(tokenizedAttribute, attributeValue, boost::is_any_of(",;"));
					if (tokenizedAttribute.size() > 0) blockRed = atof(tokenizedAttribute[0].c_str());
					if (tokenizedAttribute.size() > 1) blockGreen = atof(tokenizedAttribute[1].c_str());
					if (tokenizedAttribute.size() > 2) blockBlue = atof(tokenizedAttribute[2].c_str());
					cout << "[" << blockRed << "," << blockGreen << "," << blockBlue << "]";
				}
				attributeValue= element->Attribute("id");
				if (attributeValue) {
					blockId = atoi(attributeValue);
				}
				cout << endl;
				((MultiCoresWorld*)world)->addBlock(blockId,buildNewBlockCode,blockX, blockY, blockZ, blockRed, blockGreen, blockBlue);
				nodeBlock = nodeBlock->NextSibling("block");
			}
		}
	}

/*
	BuildingBlock *b0, *b1, *b2, *b3;
	b0 = BuildingBlock::getBlocByID(0);
	b1 = BuildingBlock::getBlocByID(1);
	b2 = BuildingBlock::getBlocByID(2);
	b3 = BuildingBlock::getBlocByID(3);

	b0->addP2PNetworkInterface(b1);
*/
}

MultiCoresSimulator::~MultiCoresSimulator() {
	cout << "\033[1;34m" << "MultiCoresSimulator destructor" << "\033[0m" <<endl;
}

void MultiCoresSimulator::initWorld() {

}
