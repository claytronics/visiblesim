//============================================================================
// Name        : BlockSimulator.cpp
// Author      : Dominique Dhoutaut, Benoît Piranda
// Version     :
// Copyright   : Copyright Dominique Dhoutaut et Benoît Piranda, Université de Franche-Comté - 2012
// Description : BlockSimulator, simulateur de SmartBlocks en C++
//============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <cmath>

using namespace std;

#include <sstream>
#include <string>
#include "Scheduler.h"
#include "openglViewer.h"
#include "Blocs.h"
#include "CodeBlocFlavio01.h"
//#include "physics.h"
//#include "CodeBlocMouvementsAleatoires.h"
//#include "CodeBlocAgents01.h"
#include "Matrice44.h"

#ifndef WIN32
#define TIXML_USE_STL	1
#endif

#include "TinyXML/tinyxml.h"

int main(int argc, char **argv) {
	puts("Démarrage de BlockSimulator v1.00\n");
	string confFileName = "configuration.xml";

	if (argc>=2) {
		confFileName= argv[1];
	}

	cout << "Open configuration file : "<< confFileName << endl;
	TiXmlDocument doc(confFileName.c_str());
	bool isLoaded = doc.LoadFile();

	if ( !isLoaded) {
		cerr << "Could not load configuration file '"<<  confFileName
				<< "'.\nUsage ./Visibl conf.xml\n" << endl;
		exit( 1 );
	}

	int largeur=20;
	int hauteur=20;
	double sx,sy,sz;

	// loading environment parameters
	TiXmlNode* node = 0;

	node = doc.FirstChild("world");
	if (node) {
		TiXmlElement* worldElement = node->ToElement();
		string str = worldElement->Attribute("gridsize");
		int pos = str.find_first_of(',');
		largeur = atoi(str.substr(0,pos).c_str());
		hauteur = atoi(str.substr(pos+1,str.length()-pos-1).c_str());
	}
	cout << "grid size : " << largeur << " x " << hauteur << endl;

	OpenglViewer *oglViewer = new OpenglViewer(argc, argv);
    oglViewer->camera->setDistance(sqrt(double(largeur*largeur+hauteur*hauteur)));

	GrilleSupport::initGrilleSupport(largeur, hauteur);

	CodeBloc **tabCodeBlocs = new CodeBloc*[GrilleSupport::tailleX*GrilleSupport::tailleY];
	Bloc **tabBlocs = new Bloc*[GrilleSupport::tailleX*GrilleSupport::tailleY];

	memset(tabCodeBlocs,0,GrilleSupport::tailleX*GrilleSupport::tailleY*sizeof(CodeBloc*));
	memset(tabBlocs,0,GrilleSupport::tailleX*GrilleSupport::tailleY*sizeof(Bloc*));

	// filling tabs using XML file
   TiXmlNode *nodeBlock = node->FirstChild("blocks");
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
	   attr= element->Attribute("size");
	   if (attr) {
		   string str(attr);
		   int pos1 = str.find_first_of(','),
			   pos2 = str.find_last_of(',');
		   sx = atof(str.substr(0,pos1).c_str());
		   sy = atof(str.substr(pos1+1,pos2-pos1-1).c_str());
		   sz = atof(str.substr(pos2+1,str.length()-pos1-1).c_str());
		   GrilleSupport::tailleBlockX=sx;
		   GrilleSupport::tailleBlockY=sy;
		   GrilleSupport::tailleBlockZ=sz;
	   }
	   cout << "default color :" << defaultColor << endl;
	   nodeBlock = nodeBlock->FirstChild("block");
	   Vecteur color;
	   int bx,by;
	   int i=0;
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
		   	   cout << "color :" << defaultColor << endl;
		   }
		   attr = element->Attribute("position");
		   bx = 0;
		   by = 0;
		   if (attr) {
		       string str(attr);
		       int pos = str.find_first_of(',');
		   	   bx = atoi(str.substr(0,pos).c_str());
		   	   by = atoi(str.substr(pos+1,str.length()-pos-1).c_str());
		   	   cout << "position : " << bx << "," << by << endl;
		   }

		   int ind = bx+by*GrilleSupport::tailleX;
		   tabCodeBlocs[ind] = new CodeBlocFlavio01();
		   tabBlocs[ind] = new Bloc(bx,by,tabCodeBlocs[ind]);
		   tabBlocs[ind]->setColor(color[0],color[1],color[2]);
		   tabBlocs[ind]->setSize(sx,sy,sz);
		   cout << "block " << i++ << ": " << bx << "," << by << endl;
		   if ( !GrilleSupport::ajouteBloc(tabBlocs[ind]) ) {
			   cout << "echec ajouteBloc";
			   exit(EXIT_FAILURE);
		   }

		   nodeBlock = nodeBlock->NextSibling("block");
	   } // end while (nodeBlock)

	} // end if(nodeBlock)


	Scheduler::initScheduler();

	GrilleSupport::demarreCodeBloc();

	oglViewer->visualise();

	delete oglViewer;
	return(EXIT_SUCCESS);
}
