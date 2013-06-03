/*
 * physics.h
 *
 *  Created on: 11 april 2012
 *      Author: ben
 */

//==============================================================================================
//
// Main class for physical mation simulation
//
// Only one instance of physics is possible. 
// uses initPhysics(); method.
//
//==============================================================================================

#ifndef PHYSICS_H_
#define PHYSICS_H_

#include <vector>
#include "Vecteur.h"
#include "Blocs.h"

using namespace std;

class PathDatedPosition
{ 
public :
  float t;
  float x,y;
  PathDatedPosition(float _t,float _x,float _y) { t=_t; x=_x; y=_y; };
};

class Mobile
{ 
public :
  GLfloat thickness,radius,*tabPts;
  GLfloat color[4];
  int id;
  Vecteur position,velocity; 
  double mass;
  vector <PathDatedPosition*>tabPositions;

  Mobile();
  Mobile(const Vecteur &pos,const Vecteur &vel,double m,double rad,double thick,const Vecteur &col);
  ~Mobile();
  void addPathPosition(PathDatedPosition*);
  void createObject();
  void glDraw();
  void update(double dt);
};

class Physics
{ 
private :
  static vector <ConveyorInterface*> tabConveyors; 
  static vector <Mobile*> tabMobiles;
  static uint64_t previousTime;

  Physics();
  ~Physics();
public :
	static void initPhysics(uint64_t currentTime=0);
	static void addMobile(Mobile *mob);
	static void addConveyor(ConveyorInterface *ci);
	static void removeCoveyor(ConveyorInterface *ci);
	static void update(uint64_t currentTime);
	static void glDraw();
	static Mobile* getMobileInBox(float x0,float y0,float z0,float x1,float y1,float z1);
	static Mobile* getMobileByID(int id) { return tabMobiles[id]; };
};

#endif
