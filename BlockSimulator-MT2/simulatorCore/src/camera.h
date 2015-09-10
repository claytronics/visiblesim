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
#include <GL/glew.h>
#include <GL/freeglut.h>
#endif

#ifdef __APPLE__
#include <GL/glew.h>
#include <GL/freeglut.h> // modifier avec freeglut et repertoire
#endif

#include <math.h>
#include "vecteur.h"
#include "matrice44.h"

#ifndef M_PI
#define M_PI	3.1415926535897932384626433832795
#endif

class LightSource {
public :
	GLfloat pos[4],target[3],dir[3];
	GLfloat color;
	GLfloat falloffAngle,near_plane,far_plane;
	GLfloat matMV[16],matMV_1[16],matP[16];
	GLfloat theta,phi,distance;

	LightSource();
	void calcMatrices();
	void draw();
};

class Camera {
  double phi,theta,distance;
  Vecteur position,target,Xcam,Ycam;
  int mouse[2];
  double sensibilityX,sensibilityY;
  double w_h,near_plane,far_plane,angle;
  bool targetMotion;
public :
  LightSource ls;

  Camera(double t,double p, double d, double SX=0.02, double SY=0.02);
  void mouseDown(int x,int y,bool tm=false);
  void mouseUp(int x,int y);
  void mouseMove(int x,int y);
  void mouseZoom(double pas);
  void mouseLightDown(int x,int y);
  void mouseLightUp(int x,int y);
  void mouseLightMove(int x,int y);
  void mouseLightZoom(double pas);
  void updateIntrinsics(double a,double rwh,double np,double fp);
  void setW_H(double r) { w_h=r; };
  void setNearFar(double n,double f) { near_plane=n; far_plane=f; };
  void setAngle(double a) { angle=a; };
  inline void setTarget(const Vecteur &p) { target=p; updatePositionFromAngles(); }
  inline void setDirection(double az,double ele) { theta=az*M_PI/180.0; phi=ele*M_PI/180.0; updatePositionFromAngles(); }
  inline void setDistance(double d) { distance=d; updatePositionFromAngles(); }
  void glLookAt();
  void setLightParameters(const Vecteur &t,double th,double ph, double d,double angle,double nearplane,double farplane);
  void glProjection();

	friend ostream& operator<<(ostream& f,const Camera &c);
protected :
  void updatePositionFromAngles();
};


#endif /* CAMERA_H_ */
