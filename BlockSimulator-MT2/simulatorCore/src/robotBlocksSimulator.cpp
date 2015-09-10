/*
* robotBlocksSimulator.cpp
*
*  Created on: 12 janvier 2014
*      Author: Benoît
*/

#include <iostream>
#include "robotBlocksSimulator.h"
#include <string.h>
#include "trace.h"

using namespace std;

namespace RobotBlocks {

RobotBlocksBlockCode*(* RobotBlocksSimulator::buildNewBlockCode)(RobotBlocksBlock*)=NULL;

void RobotBlocksSimulator::help() {
   cerr << "VisibleSim:" << endl;
	cerr << "Robot01" << endl;
   exit(EXIT_SUCCESS);
}

RobotBlocksSimulator::RobotBlocksSimulator(int argc, char *argv[], RobotBlocksBlockCode *(*robotBlocksBlockCodeBuildingFunction)(RobotBlocksBlock*)) : BaseSimulator::Simulator(argc, argv) {
	OUTPUT << "\033[1;34m" << "RobotBlocksSimulator constructor" << "\033[0m" << endl;

	int currentID = 1;
	RobotBlocksWorld *world = NULL;
	buildNewBlockCode = robotBlocksBlockCodeBuildingFunction;

	testMode = false;

	/* reading the xml file */
	TiXmlNode *node = xmlDoc->FirstChild("world");
	if (node) {
		TiXmlElement* worldElement = node->ToElement();
		const char *attr= worldElement->Attribute("gridsize");
		int lx,ly,lz;
		if (attr) {
			string str=attr;
			int pos1 = str.find_first_of(','),
				pos2 = str.find_last_of(',');
			lx = atoi(str.substr(0,pos1).c_str());
			ly = atoi(str.substr(pos1+1,pos2-pos1-1).c_str());
			lz = atoi(str.substr(pos2+1,str.length()-pos1-1).c_str());
			OUTPUT << "grid size : " << lx << " x " << ly << " x " << lz << endl;
		} else {
			OUTPUT << "WARNING No grid size in XML file" << endl;
		}
		attr=worldElement->Attribute("windowSize");
		if (attr) {
			string str=attr;
	 		int pos = str.find_first_of(',');
			GlutContext::initialScreenWidth = atoi(str.substr(0,pos).c_str());
			GlutContext::initialScreenHeight = atoi(str.substr(pos+1,str.length()-pos-1).c_str());
			GlutContext::screenWidth = GlutContext::initialScreenWidth;
			GlutContext::screenHeight = GlutContext::initialScreenHeight;
		}

		createWorld(lx, ly, lz, argc, argv);
		world = getWorld();
		world->loadTextures("../../simulatorCore/robotBlocksTextures");

	} else {
		ERRPUT << "ERROR : NO world in XML file" << endl;
		exit(1);
	}

	createScheduler();

	// loading the camera parameters
	TiXmlNode *nodeConfig = node->FirstChild("camera");
	if (nodeConfig) {
		TiXmlElement* cameraElement = nodeConfig->ToElement();
		const char *attr=cameraElement->Attribute("target");
		double def_near=1,def_far=1500;
		float angle=45.0;
		if (attr) {
			string str(attr);
			int pos1 = str.find_first_of(','),
			pos2 = str.find_last_of(',');
			Vecteur target;
			target.pt[0] = atof(str.substr(0,pos1).c_str());
			target.pt[1] = atof(str.substr(pos1+1,pos2-pos1-1).c_str());
			target.pt[2] = atof(str.substr(pos2+1,str.length()-pos1-1).c_str());
			world->getCamera()->setTarget(target);
		}
		attr=cameraElement->Attribute("angle");
		if (attr) {
			angle = atof(attr);
			world->getCamera()->setAngle(angle);
		}
		attr=cameraElement->Attribute("directionSpherical");
		if (attr) {
			string str(attr);
			int pos1 = str.find_first_of(','),
			pos2 = str.find_last_of(',');
			float az,ele,dist;
			az = -90.0+atof(str.substr(0,pos1).c_str());
			ele = atof(str.substr(pos1+1,pos2-pos1-1).c_str());
			dist = atof(str.substr(pos2+1,str.length()-pos1-1).c_str());
			world->getCamera()->setDirection(az,ele);
			world->getCamera()->setDistance(dist);
			az = dist*sin(angle*M_PI/180.0);
			def_near = dist-az;
			def_far = dist+az;
		}
		attr=cameraElement->Attribute("near");
		if (attr) {
			def_near = atof(attr);
		}
		attr=cameraElement->Attribute("far");
		if (attr) {
			def_far = atof(attr);
		}
		world->getCamera()->setNearFar(def_near,def_far);
	}

	// loading the spotlight parameters
	nodeConfig = node->FirstChild("spotlight");
	if (nodeConfig) {
		Vecteur target;
		float az=0,ele=60,dist=1000,angle=50;
		TiXmlElement* lightElement = nodeConfig->ToElement();
		const char *attr=lightElement->Attribute("target");
		if (attr) {
			string str(attr);
			int pos1 = str.find_first_of(','),
			pos2 = str.find_last_of(',');
			target.pt[0] = atof(str.substr(0,pos1).c_str());
			target.pt[1] = atof(str.substr(pos1+1,pos2-pos1-1).c_str());
			target.pt[2] = atof(str.substr(pos2+1,str.length()-pos1-1).c_str());
		}
		attr=lightElement->Attribute("directionSpherical");
		if (attr) {
			string str(attr);
			int pos1 = str.find_first_of(','),
			pos2 = str.find_last_of(',');
			az = -90.0+atof(str.substr(0,pos1).c_str());
			ele = atof(str.substr(pos1+1,pos2-pos1-1).c_str());
			dist = atof(str.substr(pos2+1,str.length()-pos1-1).c_str());
		}
		attr=lightElement->Attribute("angle");
		if (attr) {
			angle = atof(attr);
		}
		float farplane=2.0*dist*tan(angle*M_PI/180.0);
		world->getCamera()->setLightParameters(target,az,ele,dist,angle,10.0,farplane);

	}

	TiXmlNode *nodeBlock = node->FirstChild("blockList");
	if (nodeBlock) {
		Color defaultColor=DARKGREY;
		TiXmlElement* element = nodeBlock->ToElement();
		const char *attr= element->Attribute("color");
		if (attr) {
			string str(attr);
			int pos1 = str.find_first_of(','),
			pos2 = str.find_last_of(',');
			defaultColor.rgba[0] = atof(str.substr(0,pos1).c_str())/255.0;
			defaultColor.rgba[1] = atof(str.substr(pos1+1,pos2-pos1-1).c_str())/255.0;
			defaultColor.rgba[2] = atof(str.substr(pos2+1,str.length()-pos1-1).c_str())/255.0;
			OUTPUT << "new default color :" << defaultColor << endl;
		}
		attr= element->Attribute("blocksize");
    if (attr) {
		 string str(attr);
		 int pos1 = str.find_first_of(','),
		  pos2 = str.find_last_of(',');
		 float siz[3];
		 siz[0] = atof(str.substr(0,pos1).c_str());
		 siz[1] = atof(str.substr(pos1+1,pos2-pos1-1).c_str());
		 siz[2] = atof(str.substr(pos2+1,str.length()-pos1-1).c_str());
		 OUTPUT << "blocksize =" << siz[0] << "," << siz[1] << "," << siz[2] << endl;
		 world->setBlocksSize(siz);
		}

	/* Reading a robotblock */
		TiXmlNode *block = nodeBlock->FirstChild("block");
		Vecteur position;
		Color color;
		bool master;
		while (block) {
		   element = block->ToElement();
		   color=defaultColor;
		   master=false;
		   attr = element->Attribute("color");
		   if (attr) {
			   string str(attr);
			   int pos1 = str.find_first_of(','),
		   		   pos2 = str.find_last_of(',');
			   color.set(atof(str.substr(0,pos1).c_str())/255.0,
                         atof(str.substr(pos1+1,pos2-pos1-1).c_str())/255.0,
                         atof(str.substr(pos2+1,str.length()-pos1-1).c_str())/255.0);
			   OUTPUT << "new color :" << defaultColor << endl;
			}
			attr = element->Attribute("position");
			if (attr) {
        string str(attr);
        int pos1 = str.find_first_of(','),
				pos2 = str.find_last_of(',');
				position.pt[0] = atof(str.substr(0,pos1).c_str());
				position.pt[1] = atof(str.substr(pos1+1,pos2-pos1-1).c_str());
				position.pt[2] = atof(str.substr(pos2+1,str.length()-pos1-1).c_str());
				OUTPUT << "position : " << position << endl;
			}
			attr = element->Attribute("master");
			if (attr) {
        string str(attr);
        if (str.compare("true")==0 || str.compare("1")==0) {
					master=true;
        }
        OUTPUT << "master : " << master << endl;
			}
			world->addBlock(currentID++,RobotBlocksSimulator::buildNewBlockCode,position,color,master);
			block = block->NextSibling("block");
		} // end while (block)

		block = nodeBlock->FirstChild("blocksLine");
		int line,plane;
		while (block) {
			element = block->ToElement();
			color=defaultColor;
			attr = element->Attribute("color");
			if (attr) {
				string str(attr);
				int pos1 = str.find_first_of(','),
					pos2 = str.find_last_of(',');
				color.rgba[0] = atof(str.substr(0,pos1).c_str())/255.0;
				color.rgba[1] = atof(str.substr(pos1+1,pos2-pos1-1).c_str())/255.0;
				color.rgba[2] = atof(str.substr(pos2+1,str.length()-pos1-1).c_str())/255.0;
				OUTPUT << "line color :" << color << endl;

			}
			attr = element->Attribute("line");
			if (attr) {
				line = atoi(attr);
			}
			attr = element->Attribute("plane");
			if (attr) {
				plane = atoi(attr);
			}
			attr = element->Attribute("values");
			if (attr) {
				string str(attr);
				position.pt[2] = plane;
				position.pt[1] = line;
				int n = str.length();
				for(int i=0; i<n; i++) {
			    	if  (str[i]=='1') {
			    		position.pt[0]=i;
			    		world->addBlock(currentID++,RobotBlocksSimulator::buildNewBlockCode,position,color);
			    	}
			    }
			}
			block = block->NextSibling("blocksLine");
		} // end while (nodeBlock)
	} else // end if(nodeBlock)
	{ cerr << "no Block List" << endl;
	}
	TiXmlNode *nodeGrid = node->FirstChild("targetGrid");
	if (nodeGrid) {
        world->initTargetGrid();
		TiXmlNode *block = nodeGrid->FirstChild("targetLine");
		int line,plane;
		while (block) {
			TiXmlElement* element = block->ToElement();
			const char *attr = element->Attribute("line");
	 		if (attr) {
	 			line = atoi(attr);
	 		}
	 		attr = element->Attribute("plane");
	 		if (attr) {
	 			plane = atoi(attr);
	 		}
	 		attr = element->Attribute("values");
	 		if (attr) {
	 			string str(attr);
	 			int n = str.length();
	 			for(int i=0; i<n; i++) {
	 		    	world->setTargetGrid((str[i]=='1')?fullCell:emptyCell,i,line,plane);
	 		    }
	 		}
	 		block = block->NextSibling("targetLine");
	 	}
	 } else {
	 	ERRPUT << "No target grid" << endl;
	 }

    TiXmlNode *nodeCapa = node->FirstChild("capabilities");
	if (nodeCapa) {
        world->setCapabilities(new RobotBlocksCapabilities(nodeCapa));
    }

    world->linkBlocks();

//	getScheduler()->sem_schedulerStart->post();
//	getScheduler()->setState(Scheduler::NOTSTARTED);

	if (!testMode) {
      GlutContext::mainLoop();
   }
}

RobotBlocksSimulator::~RobotBlocksSimulator() {
	OUTPUT << "\033[1;34m" << "RobotBlocksSimulator destructor" << "\033[0m" <<endl;
}

void RobotBlocksSimulator::createSimulator(int argc, char *argv[], RobotBlocksBlockCode *(*robotBlocksBlockCodeBuildingFunction)(RobotBlocksBlock*)) {
	simulator =  new RobotBlocksSimulator(argc, argv, robotBlocksBlockCodeBuildingFunction);
}

void RobotBlocksSimulator::deleteSimulator() {
	delete((RobotBlocksSimulator*)simulator);
}

} // RobotBlocks namespace
