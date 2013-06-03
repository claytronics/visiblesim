/*
 * glBlock.cpp
 *
 *  Created on: 23/02/2012
 *      Author: ben
 *  OpenGL drawing of a textured block
 *  allowing to colorize and mapping a letter on the top of the block
 */

#include "glBlock.h"

GlBlock::GlBlock()
{ color[0] = 1.0;
  color[1] = 1.0;
  color[2] = 1.0;
  highlighted = false;
}

void GlBlock::glDraw()
{ GLfloat blanc[4],c[4];

  glPushMatrix();
  glTranslated(pos[0],pos[1],pos[2]);
  glEnable(GL_TEXTURE_2D);

  glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

  if (highlighted) 
  { GLfloat n = 0.5+1.5*(1.0-(glutGet(GLUT_ELAPSED_TIME)%1000)/1000.0);
    blanc[0]=blanc[1]=blanc[2]=n;
	c[0]=color[0]*n;
	c[1]=color[1]*n;
	c[2]=color[2]*n;
  } else
  { blanc[0] = blanc[1] = blanc[2] = 1.0;
    memcpy(c,color,3*sizeof(GLfloat));
  }

  glScalef(size[0],size[1],size[2]);

  // face du dessus
  glNormal3f(0,0,1);
  glMaterialfv(GL_FRONT,GL_DIFFUSE,c);
  glBegin(GL_QUADS);
  glTexCoord2f(0.75,0.5);
  glVertex3f(0.5,0.5,1.0);
  glTexCoord2f(0.75,0.75);
  glVertex3f(-0.5,0.5,1.0);
  glTexCoord2f(1.0,0.75);
  glVertex3f(-0.5,-0.5,1.0);
  glTexCoord2f(1.0,0.5);
  glVertex3f(0.5,-0.5,1.0);
  glEnd();
  
  glMaterialfv(GL_FRONT,GL_DIFFUSE,blanc);
  glNormal3f(1,0,0);
  glBegin(GL_QUADS);
    glTexCoord2f(0.75,0);
	glVertex3f(0.5,-0.5,1.0);
	glTexCoord2f(0.5,0);
	glVertex3f(0.5,-0.5,0);
	glTexCoord2f(0.5,0.25);
    glVertex3f(0.5,0.5,0);
	glTexCoord2f(0.75,0.25);
	glVertex3f(0.5,0.5,1.0);
  glEnd();
  glNormal3f(0,1,0);
  glBegin(GL_QUADS);
    glTexCoord2f(0.75,0.25);
	glVertex3f(0.5,0.5,1.0);
    glTexCoord2f(0.5,0.25);
    glVertex3f(0.5,0.5,0);
    glTexCoord2f(0.5,0.5);
    glVertex3f(-0.5,0.5,0);
    glTexCoord2f(0.75,0.5);
    glVertex3f(-0.5,0.5,1.0);
  glEnd();
  glNormal3f(-1,0,0);
  glBegin(GL_QUADS);
    glTexCoord2f(0.75,0.5);
    glVertex3f(-0.5,0.5,1.0);
	glTexCoord2f(0.5,0.5);
    glVertex3f(-0.5,0.5,0);
    glTexCoord2f(0.5,0.75);
    glVertex3f(-0.5,-0.5,0);
    glTexCoord2f(0.75,0.75);
    glVertex3f(-0.5,-0.5,1.0);
  glEnd();
  glNormal3f(0,-1,0);
  glBegin(GL_QUADS);
    glTexCoord2f(0.75,0.75);
    glVertex3f(-0.5,-0.5,1.0);
	glTexCoord2f(0.5,0.75);
    glVertex3f(-0.5,-0.5,0);
    glTexCoord2f(0.5,1.0);
    glVertex3f(0.5,-0.5,0);
    glTexCoord2f(0.75,1.0);
    glVertex3f(0.5,-0.5,1.0);
  glEnd();
  glPopMatrix();
}

void GlBlock::glDrawId()
{ glPushMatrix();
  glTranslated(pos[0],pos[1],pos[2]);
  glScalef(size[0],size[1],size[2]);
  glBegin(GL_QUAD_STRIP);
  glVertex3f(0.5,-0.5,1.0);
  glVertex3f(0.5,-0.5,0);

  glVertex3f(0.5,0.5,1.0);
  glVertex3f(0.5,0.5,0);

  glVertex3f(-0.5,0.5,1.0);
  glVertex3f(-0.5,0.5,0);

  glVertex3f(-0.5,-0.5,1.0);
  glVertex3f(-0.5,-0.5,0);

  glVertex3f(0.5,-0.5,1.0);
  glVertex3f(0.5,-0.5,0);
  glEnd();

  glBegin(GL_QUADS);
  glVertex3f(0.5,0.5,1.0);
  glVertex3f(-0.5,0.5,1.0);
  glVertex3f(-0.5,-0.5,1.0);
  glVertex3f(0.5,-0.5,1.0);
  glEnd();

  glPopMatrix();
}
