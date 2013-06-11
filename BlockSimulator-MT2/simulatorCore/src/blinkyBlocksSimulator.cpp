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
    createScheduler();

    int currentID=1;
    BlinkyBlocksWorld *world=NULL;
    /* reading the xml file */
    TiXmlNode *node = xmlDoc->FirstChild("world");
    if (node) {
      TiXmlElement* worldElement = node->ToElement();
      string str = worldElement->Attribute("gridsize");
      int pos1 = str.find_first_of(','),
	pos2 = str.find_last_of(',');
      int lx = atoi(str.substr(0,pos1).c_str());
      int ly = atoi(str.substr(pos1+1,pos2-pos1-1).c_str());
      int lz = atoi(str.substr(pos2+1,str.length()-pos1-1).c_str());

      cout << "grid size : " << lx << " x " << ly << " x " << lz << endl;

      createWorld(lx,ly,lz,argc,argv);
      world = getWorld();
      world->loadTextures("../../simulatorCore/blinkyBlocksTextures");
    } else {
      cerr << "ERROR : NO world in XML file" << endl;
      exit(1);
    }

    // loading the camera parameters
    TiXmlNode *nodeConfig = node->FirstChild("camera");
    if (nodeConfig) {
      TiXmlElement* cameraElement = nodeConfig->ToElement();
      const char *attr=cameraElement->Attribute("target");
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
      }
      attr=cameraElement->Attribute("angle");
      if (attr) {
	float angle = atof(attr);
	world->getCamera()->setAngle(angle);
      }
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

      attr= element->Attribute("blocksize");
      if (attr) {
	string str(attr);
	int pos1 = str.find_first_of(','),
	  pos2 = str.find_last_of(',');
	float siz[3];
	siz[0] = atof(str.substr(0,pos1).c_str());
	siz[1] = atof(str.substr(pos1+1,pos2-pos1-1).c_str());
	siz[2] = atof(str.substr(pos2+1,str.length()-pos1-1).c_str());
	cout << "blocksize =" << siz[0] <<"," << siz[1] <<"," << siz[2]<< endl;
	world->setBlocksSize(siz);
      }

      /* Reading a blinkyblock */
      cout << "default color :" << defaultColor << endl;
      nodeBlock = nodeBlock->FirstChild("block");
      Vecteur color,position;

      while (nodeBlock) {
	element = nodeBlock->ToElement();
	color=defaultColor;
	attr = element->Attribute("color");
	if (attr) {
	  string str(attr);
	  int pos1 = str.find_first_of(','),
	    pos2 = str.find_last_of(',');
	  color.pt[0] = atof(str.substr(0,pos1).c_str())/255.0;
	  color.pt[1] = atof(str.substr(pos1+1,pos2-pos1-1).c_str())/255.0;
	  color.pt[2] = atof(str.substr(pos2+1,str.length()-pos1-1).c_str())/255.0;
	  cout << "color :" << color << endl;
	}
	attr = element->Attribute("position");
	if (attr) {
	  string str(attr);
	  int pos1 = str.find_first_of(','),
	    pos2 = str.find_last_of(',');
	  position.pt[0] = atoi(str.substr(0,pos1).c_str());
	  position.pt[1] = atoi(str.substr(pos1+1,pos2-pos1-1).c_str());
	  position.pt[2] = atoi(str.substr(pos2+1,str.length()-pos1-1).c_str());
	  cout << "position : " << position << endl;
	}

	world->addBlock(currentID++,BlinkyBlocksSimulator::buildNewBlockCode,position,color);
	nodeBlock = nodeBlock->NextSibling("block");
      } // end while (nodeBlock)

    } else // end if(nodeBlock)
      { cerr << "no Block List" << endl;

      }
    world->linkBlocks();

    GlutContext::mainLoop();
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
