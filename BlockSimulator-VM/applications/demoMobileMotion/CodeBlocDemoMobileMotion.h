/*
 * CodeBlocDemoMouvementsGroupes.h
 *
 *  Created on: 29 févr. 2012
 *      Author: dom
 */

#ifndef CODEBLOCDEMOMOBILE_H_
#define CODEBLOCDEMOMOBILE_H_

#include "CodeBloc.h"
#include "Blocs.h"
#include "Message.h"

#define TYPE_MSG_JET				1001
#define TYPE_MSG_TRAJ				1002

class MessageJet : public Message {
public:
	int jetDuration;
	MessageJet(int jd);
};

class Trajectoire;

class MessageTrajectoire : public Message {
public :
	Trajectoire *traj;
	int startTimeMs;
	MessageTrajectoire(Trajectoire *t,int st);
};

class Trajectoire {
public :
	float *tabInfos;
	int nbreInfos;
	
	Trajectoire(int n);
	Trajectoire(MessageTrajectoire *mt);
	~Trajectoire();
	void addInfo(int i,float t,float x,float y); 
	void getInterpolatedPos(float t,float &x,float &y);
};

class CodeBlocDemoMobile : public CodeBloc {
public:
	Trajectoire *traj;
	bool hasMobile;
	int startTimeMs;

	CodeBlocDemoMobile();
	void demarre(Message *msg);
	void traiteEvenementLocal(Evenement *ev);
};

#endif /* CODEBLOCDEMOMOBILE_H_ */
