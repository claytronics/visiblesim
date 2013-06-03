/*
 * CodeBlocMouvementsAleatoires.h
 *
 *  Created on: 16 févr. 2012
 *      Author: dom
 */

#ifndef CODEBLOCMOUVEMENTSALEATOIRES_H_
#define CODEBLOCMOUVEMENTSALEATOIRES_H_

#include "CodeBloc.h"
#include "Blocs.h"
#include "Message.h"

#define TYPE_MSG_DIFFUSION_POSITION				1000

class MessageDiffusionPosition : public Message {
public:
	int posX,posY;
	MessageDiffusionPosition(int x, int y);
};

class CodeBlocMouvementsAleatoires : public CodeBloc {
public:
	bool estPositionne;
	int posX, posY;

	CodeBlocMouvementsAleatoires();
	void demarre();
	void traiteEvenementLocal(Evenement *ev);

	void annonceMaPositionAuxVoisins();
};

#endif /* CODEBLOCMOUVEMENTSALEATOIRES_H_ */
