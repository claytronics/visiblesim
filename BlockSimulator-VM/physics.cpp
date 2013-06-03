#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <iomanip>

#ifndef WIN32
#include <unistd.h>
#endif

#include "physics.h"

using namespace std;

vector <ConveyorInterface*> Physics::tabConveyors;
vector <Mobile*> Physics::tabMobiles;
uint64_t Physics::previousTime=0;

Physics::Physics()
{ 
};

Physics::~Physics()
{ vector <Mobile*>::const_iterator im=tabMobiles.begin();
  while (im!=tabMobiles.end())
  { delete (*im);
	im++;
  }
  tabMobiles.clear();

  tabConveyors.clear();

};

void Physics::addMobile(Mobile *mob)
{ tabMobiles.push_back(mob);
}

void Physics::addConveyor(ConveyorInterface *ci)
{ // verify that the conveyor is not in the list
	vector <ConveyorInterface*>::const_iterator ic=tabConveyors.begin();
	while (ic!=tabConveyors.end() && (*ic)!=ci)
	{ ic++;
	}
	if (ic==tabConveyors.end()) tabConveyors.push_back(ci);
}

void Physics::removeCoveyor(ConveyorInterface *ci)
{ // find ci in the list
	vector <ConveyorInterface*>::iterator ic=tabConveyors.begin();
	while (ic!=tabConveyors.end() && (*ic)!=ci)
	{ ic++;
	}
	if (ic!=tabConveyors.end()) tabConveyors.erase(ic);
}

void Physics::initPhysics(uint64_t currentTime)
{ previousTime = currentTime;
}

void Physics::update(uint64_t currentTime)
{ 
// many event may have the same date !
  if (currentTime<=previousTime) return;
// calculation of displacements
  double dt = (currentTime-previousTime)/1000000.0;
  //cout << "update (" << currentTime << ")" << endl;

  vector <Mobile*>::const_iterator im=tabMobiles.begin();
  
  Vecteur Force,
	  mobilePos=(*im)->position,
	  mobileVel=(*im)->velocity;

// time integration
  double staticDelay=0.0001; // first method : fixed delay
  int n=int(dt/staticDelay);

  if (n==0)
  { n=1;
  } else
  { dt/=n;
  }

// prise en compte de la dur�e du jet !!!
  // en particulier voir la variable disponibilit�
  for (int i=0; i<n; i++)
  { Force.set(0,0,0);
// looking for the list of airJets
    vector <ConveyorInterface*>::const_iterator ic=tabConveyors.begin();
    while (ic!=tabConveyors.end())
    { Force += (*ic)->getForce(mobilePos);
	  ic++;
    }
    
	// amortissement
	mobileVel=0.9999*mobileVel + (dt/(*im)->mass)*Force;
	
	mobilePos+=dt*mobileVel;
  }
  (*im)->position = mobilePos;
  (*im)->velocity = mobileVel;
  //cout << (*im)->position << ";" << (*im)->velocity << endl;
	
// ready for next call
  previousTime = currentTime;
}

void Physics::glDraw()
{ vector <Mobile*>::const_iterator im=tabMobiles.begin();
  while (im!=tabMobiles.end())
  { (*im)->glDraw();
	im++;
  }
}

Mobile *Physics::getMobileInBox(float x0,float y0,float z0,float x1,float y1,float z1)
{ vector <Mobile*>::const_iterator im=tabMobiles.begin();
  while (im!=tabMobiles.end())
  { if ((*im)->position.pt[0]>=x0 && (*im)->position.pt[0]<=x1 && 
        (*im)->position.pt[1]>=y0 && (*im)->position.pt[1]<=y1 && 
        (*im)->position.pt[2]>=z0 && (*im)->position.pt[2]<=z1)
    { return *im;
    }
	im++;
  }
  return NULL; // no mobile in the box
}

Mobile::Mobile(const Vecteur &pos, const Vecteur &vel,double m,double rad,double thick,const Vecteur &col)
{ position = pos;
  velocity = vel;
  thickness = thick; // mm
  radius = rad; // mm
  color[0] = col[0];
  color[1] = col[1];
  color[2] = col[2];
  color[3] = 1.0f;
  mass = m;

  tabPts = NULL;
  createObject();
}

Mobile::Mobile()
{ color[3] = 1.0f;
  tabPts=NULL;
}

void Mobile::createObject()
{ if (tabPts) delete [] tabPts;
  tabPts = new GLfloat[36];
  double a=0;
  GLfloat *ptr=tabPts;
  for (int i=0; i<12; i++) {
	*ptr++ = radius*cos(a);
    *ptr++ = radius*sin(a);
	*ptr++ = thickness; 
    a+=M_PI/6.0;
  }
}

Mobile::~Mobile()
{ delete [] tabPts;
  while (!tabPositions.empty())
  { delete tabPositions.back();
    tabPositions.pop_back();
  }
}

void Mobile::addPathPosition(PathDatedPosition *pdp)
{ vector<PathDatedPosition*>::iterator ci = tabPositions.begin();
  
  while (ci!=tabPositions.end() && (*ci)->t<pdp->t)
  { ci++; 
  }
  if (ci!=tabPositions.end()) { 
	  tabPositions.insert(ci,pdp);
  } else {
	  tabPositions.push_back(pdp);
  }
}

void Mobile::glDraw()
{ glPushMatrix();
  glTranslatef(position[0],position[1],position[2]);
  glDisable(GL_TEXTURE_2D);
  glMaterialfv(GL_FRONT,GL_DIFFUSE,color);
  glNormal3f(0,0,1);
  glBegin(GL_POLYGON);
  for (int i=0; i<12; i++) {
    glVertex3fv(tabPts+i*3);
  }
  glEnd();
  glBegin(GL_QUAD_STRIP);
    for (int i=0; i<12; i++) {
		glNormal3f(tabPts[i*3],tabPts[i*3+1],0);
		glVertex3f(tabPts[i*3],tabPts[i*3+1],thickness);
		glVertex3f(tabPts[i*3],tabPts[i*3+1],0);
    }
	glVertex3f(tabPts[0],tabPts[1],thickness);
	glVertex3f(tabPts[0],tabPts[1],0);
  
  glEnd();
  glPopMatrix();

  if (!tabPositions.empty()) {
	vector<PathDatedPosition*>::const_iterator ci = tabPositions.begin();
	glDisable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glBegin(GL_LINE_STRIP);
	while (ci!=tabPositions.end()) { 
		glVertex3f((*ci)->x,(*ci)->y,position[2]+thickness);
		ci++;
	}
	glEnd();
	glEnable(GL_LIGHTING);
  }
}
