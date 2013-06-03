/*
 * CodeBlocDemoMotion.cpp
 *
 *  Created on: 08 may 2012
 *      Author: ben
 */

#include <iostream>
#include <sstream>
#include "CodeBlocDemoMobileMotion.h"
#include "Scheduler.h"

using namespace std;

MessageJet::MessageJet(int jd) : Message() {
	typeMessage = TYPE_MSG_JET;
	jetDuration = jd;
}

MessageTrajectoire::MessageTrajectoire(Trajectoire *t,int st)
{ typeMessage = TYPE_MSG_TRAJ;
  traj = t;
  startTimeMs = st;
}

Trajectoire::Trajectoire(int n)
{ nbreInfos=n;
  tabInfos = new float[3*n];
}

Trajectoire::Trajectoire(MessageTrajectoire *mt)
{ nbreInfos=mt->traj->nbreInfos;
  tabInfos = new float[3*nbreInfos];
  memcpy(tabInfos,mt->traj->tabInfos,nbreInfos*3*sizeof(float));
}

Trajectoire::~Trajectoire()
{ delete [] tabInfos;
}

void Trajectoire::addInfo(int i,float t,float x,float y)
{ tabInfos[i*3]=t;
  tabInfos[i*3+1]=x;
  tabInfos[i*3+2]=y;
}

void Trajectoire::getInterpolatedPos(float t, float &x,float &y)
{ int i=nbreInfos-1;
  float *ptrInfos=tabInfos;
  
  while (i && *ptrInfos<=t)
  { i--;
    ptrInfos+=3;
  }

  if (i==0)
  { x = ptrInfos[1];
    y = ptrInfos[2];
  } else
  { float ti = (t-ptrInfos[0])/(ptrInfos[3]-ptrInfos[0]);
	x = (1-ti)*ptrInfos[1]+ti*ptrInfos[4];
	y = (1-ti)*ptrInfos[2]+ti*ptrInfos[5];  
  }
}

CodeBlocDemoMobile::CodeBlocDemoMobile() : CodeBloc() {
	hasMobile=false;
	traj=NULL;
}

void CodeBlocDemoMobile::demarre(Message *msg) {
	float x, y, vx, vy;
	int id;

	hasMobile = bloc->hasMobile(id,x,y,vx,vy); // voir pour l'info surface/plusieurs objets couverts

	if (msg->typeMessage==TYPE_MSG_TRAJ) { 
		MessageTrajectoire *mt = (MessageTrajectoire*)msg;
	  // copie la trajectoire
		traj = new Trajectoire(mt);
		startTimeMs=mt->startTimeMs;
	}
	
	if (hasMobile)
	{ Scheduler::schedule(new EvenementBlocTimer(Scheduler::getHeureActuelle()+10000,bloc,1));
	}
}

void CodeBlocDemoMobile::traiteEvenementLocal(Evenement *ev) {
	EvenementBlocRecoitMessage *evm;
	
	switch (ev->typeEvenement) {
	case TYPE_EV_BLOC_RECOIT_MESSAGE:
		evm = (EvenementBlocRecoitMessage*)ev;
		if (evm->message->typeMessage == TYPE_MSG_JET) {
			MessageJet *mj = (MessageJet*)evm->message;
			uint64_t instant = mj->jetDuration;
			if (mj->jetDuration>0)
			{ bloc->activeJet(mj->jetDuration);
			  bloc->setColor(255,0,0);
			} else bloc->setColor(0,255,0);
		}
	break;
	case TYPE_EV_BLOC_TIMER:
		{	float x, y, vx, vy,gx,gy;
			int id;
			if (bloc->hasMobile(id,x,y,vx,vy)) {
				float t = (Scheduler::getHeureActuelle()-startTimeMs)/1000000.0;
				cout << Scheduler::getHeureActuelle() << endl;
				if (t>0)
				{ cout << "pos =" << t << " =" << x << "," << y << endl;
				  traj->getInterpolatedPos(t,gx,gy);
				  cout << "but " << t << " =" << gx << "," << gy << endl;
					vx = gx-x;
				  vy = gy-y;
				  int currentTime = Scheduler::getHeureActuelle()+100;
				  if (vx>0 && bloc->voisinEst != NULL) {
					  Scheduler::schedule(new EvenementBlocEnvoieMessage(currentTime, new MessageJet(1000), bloc->interfaceReseauEst));
				  } 
				  if (vx<0 && bloc->voisinOuest != NULL) {
					  Scheduler::schedule(new EvenementBlocEnvoieMessage(currentTime, new MessageJet(1000), bloc->interfaceReseauOuest));
				  }
				  if (vy<0 && bloc->voisinNord != NULL) {
					  Scheduler::schedule(new EvenementBlocEnvoieMessage(currentTime, new MessageJet(1000), bloc->interfaceReseauNord));
				  } 
				  if (vy>0 && bloc->voisinSud != NULL) {
					  Scheduler::schedule(new EvenementBlocEnvoieMessage(currentTime, new MessageJet(1000), bloc->interfaceReseauSud));
				  }
				  bloc->setColor(255,0,255);
				}
			} 
			Scheduler::schedule(new EvenementBlocTimer(Scheduler::getHeureActuelle()+10000,bloc,1));
		}
	break;
	default:
		break;
	}
}
