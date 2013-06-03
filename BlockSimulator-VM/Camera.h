/*
 * Camera.h
 *
 *  Created on: 29 janv. 2012
 *
 */

#ifndef CAMERA_H_
#define CAMERA_H_

#ifdef _WIN32
#include <windows.h>
#endif

#if defined(_WIN32) || defined(__linux__)
#include <GL/freeglut.h>
#endif

#ifdef __APPLE__
#include <GL/freeglut.h> // modifier avec freeglut et repertoire
#endif

#include <math.h>
#include "Vecteur.h"

#ifndef M_PI
#define M_PI	3.1415926535897932384626433832795
#endif

class Camera
{
  double phi,theta,distance;
  Vecteur Target;
  int mouse[2];
  double sensibiliteX,sensibiliteY;
  Vecteur pos,Xfloor,Yfloor;
  int targetMotion;
public :

  Camera(double t,double p, double d, double SX=0.02, double SY=0.02);
  void mouseDown(int x,int y,bool tm=false);
  void mouseUp(int x,int y);
  void mouseMove(int x,int y);
  void mouseZoom(double pas);
  inline void setTarget(const Vecteur &p) { Target=p; update(); }
  inline void setDistance(double d) { distance=d;  update();}
  void glLookAt();
protected :
  void update();
};


#endif /* CAMERA_H_ */
