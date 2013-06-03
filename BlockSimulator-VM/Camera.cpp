/*
 * Camera.cpp
 *
 *  Created on: 29 janv. 2012
 *
 */

/***************************************************************************
  camera.cpp  -  gestion de la camera OpenGL
                             -------------------
    copyright            : (C) 2011 by Benoît Piranda
    email                : benoit.piranda@univ-fcomte.fr
 LASELDI / Universite de Franche-Comté
 ***************************************************************************/

#include "Camera.h"

Camera::Camera(double t, double p, double d, double SX,double SY)
{ phi=p;
  theta=t;
  distance = d;
  sensibiliteX=SX;
  sensibiliteY=SY;
  targetMotion=false;
  update();
}

void Camera::update()
{ double dx = cos(phi)*cos(theta),
         dy = cos(phi)*sin(theta);
  pos.pt[0] = Target.pt[0] + distance*dx;
  pos.pt[1] = Target.pt[1] + distance*dy;
  pos.pt[2] = Target.pt[2] + distance*sin(phi);
  Xfloor.set(dy,-dx,0);
  Xfloor.normer_interne();
  Yfloor.set(dx,dy,0);
  Yfloor.normer_interne();
}

void Camera::mouseDown(int x, int y, bool tm)
{ mouse[0]=x;
  mouse[1]=y;
  targetMotion=tm;
}

void Camera::mouseUp(int x, int y)
{ if (targetMotion)
  { Target+=0.1*(mouse[0]-x)*Xfloor + 0.1*(mouse[1]-y)*Yfloor;
  } else
  { theta += (mouse[0]-x)*sensibiliteX;
    phi -= (mouse[1]-y)*sensibiliteY;
    if (phi>M_PI/2) phi=M_PI/2;
    else if (phi<-M_PI/2) phi=-M_PI/2;
    targetMotion=false;
  }
  update();
}

void Camera::mouseMove(int x, int y)
{ if (targetMotion)
  { Target+=0.1*(mouse[0]-x)*Xfloor + 0.1*(mouse[1]-y)*Yfloor;
  } else
  { theta += (mouse[0]-x)*sensibiliteX;
    phi -= (mouse[1]-y)*sensibiliteY;
    if (phi>M_PI/2) phi=M_PI/2;
    else if (phi<-M_PI/2) phi=-M_PI/2;
  }
  mouse[0]=x;
  mouse[1]=y;
  update(); 
}
void Camera::mouseZoom(double pas)
{ distance+=pas;
  update();
}

void Camera::glLookAt()
{
  gluLookAt(pos[0],pos[1],pos[2],Target[0],Target[1],Target[2], 0.,0.,1.);
}

