/*
 * CodeBlocDemoLocalTimer.h
 *
 *  Created on: 29 févr. 2012
 *      Author: dom
 */

#ifndef CODEBLOCDEMOLOCALTIMER_H_
#define CODEBLOCDEMOLOCALTIMER_H_

#include "CodeBloc.h"
#include "Blocs.h"
#include "Message.h"

class CodeBlocDemoLocalTimer : public CodeBloc {
public:
	bool rouge;

	CodeBlocDemoLocalTimer();
	void demarre();
	void traiteEvenementLocal(Evenement *ev);
};

#endif /* CODEBLOCDEMOLOCALTIMER_H_ */
