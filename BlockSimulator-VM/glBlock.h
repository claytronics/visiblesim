/*
 * glBlock.h
 *
 *  Created on: 23/02/2012
 *      Author: ben
 *  OpenGL drawing of a textured block
 *  allowing to colorize and mapping a letter on the top of the block
 */

#ifndef GLBLOCK_H_
#define GLBLOCK_H_

#ifdef _WIN32
#include <windows.h>
#endif

#if defined(_WIN32) || defined(__linux__)
#include <GL/freeglut.h>
#endif

#ifdef __APPLE__
#include <GL/freeglut.h> // modifier avec freeglut et repertoire
#endif

#include "Vecteur.h"
class Bloc;

class GlBlock
{ GLfloat pos[3];
  GLfloat color[3];
  GLfloat size[3];
  bool highlighted;
  public :
    GlBlock();
    inline void setColor(GLfloat r,GLfloat g,GLfloat b) { color[0]=r; color[1]=g; color[2]=b; };
    inline void setGridPosition(float x=0.0f,float y=0.0f,float z=0.0f) { pos[0]=x*size[0]; pos[1]=y*size[1]; pos[2]=z*size[2];  };
    inline void getGridPosition(GLfloat &x,GLfloat &y) { x = pos[0]/size[0]; y = pos[1]/size[1]; };
	inline void getTopPosition(GLfloat &x,GLfloat &y,GLfloat &z) { x = pos[0]; y = pos[1]; z=pos[2]+size[2]; };
	inline void setSize(GLfloat sx,GLfloat sy,GLfloat sz) { size[0]=sx; size[1]=sy; size[2]=sz; }; 
	inline void getSize(GLfloat &sx,GLfloat &sy,GLfloat &sz) { sx=size[0]; sy=size[1]; sz=size[2]; }; 
	inline void getBoundingBox(GLfloat &x0,GLfloat &y0,GLfloat &z0,GLfloat &x1,GLfloat &y1,GLfloat &z1) { x0=pos[0]-0.5f*size[0]; y0=pos[1]-0.5f*size[1]; z0=pos[2]; x1=pos[0]+0.5f*size[0]; y1=pos[1]+0.5f*size[1]; z1=pos[2]+size[2]; };
	void glDraw();
    void glDrawId();
    void toggleHighlight() { highlighted = !highlighted; };

    friend class Bloc;
};


#endif

