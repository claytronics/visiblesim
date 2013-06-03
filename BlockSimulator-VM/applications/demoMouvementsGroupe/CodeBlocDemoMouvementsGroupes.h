/*
 * CodeBlocDemoMouvementsGroupes.h
 *
 *  Created on: 29 févr. 2012
 *      Author: dom
 */

#ifndef CODEBLOCDEMOMOUVEMENTSGROUPES_H_
#define CODEBLOCDEMOMOUVEMENTSGROUPES_H_

#include "CodeBloc.h"
#include "Blocs.h"
#include "Message.h"

#define TYPE_MSG_CONSTITUTION_GROUPE				1001

class MessageConstitutionGroupe : public Message {
public:
	vector<Bloc*> vecteurBlocs;
	float rCol, gCol, bCol;
	MessageConstitutionGroupe(vector<Bloc*> vectBloc,float r, float g, float b);
};


class CodeBlocDemoMouvementsGroupes : public CodeBloc {
public:
	bool queueDeTrain;
	bool locomotive;

	CodeBlocDemoMouvementsGroupes();
	void demarre();
	void traiteEvenementLocal(Evenement *ev);
};

#endif /* CODEBLOCDEMOMOUVEMENTSGROUPES_H_ */
