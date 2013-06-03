/*
 * CodeBlockDemo2.h
 *
 *  Created on: 5 févr. 2012
 *      Author: dom
 */

#ifndef CODEBLOCDEMO2_H_
#define CODEBLOCDEMO2_H_

#include "CodeBloc.h"
#include "Blocs.h"
#include "Message.h"

#define TYPE_MSG_DIFFUSION_POSITION				1000

class MessageDiffusionPosition : public Message {
public:
	int posX,posY;
	MessageDiffusionPosition(int x, int y);
};

class CodeBlocDemo2 : public CodeBloc {
public:
	bool estPositionne;
	int posX, posY;

	CodeBlocDemo2();
	void demarre();
	void traiteEvenementLocal(Evenement *ev);
};

#endif /* CODEBLOCDEMO2_H_ */
