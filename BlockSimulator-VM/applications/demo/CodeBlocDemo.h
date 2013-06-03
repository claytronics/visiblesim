/*
 * CodeBlocDemo.h
 *
 *  Created on: 1 févr. 2012
 *      Author: dom
 */

#ifndef CODEBLOCDEMO_H_
#define CODEBLOCDEMO_H_

#include "CodeBloc.h"
#include "Blocs.h"

class CodeBlocDemo : public CodeBloc {
public:
	CodeBlocDemo();
	void demarre();
	void traiteEvenementLocal(Evenement *ev);
};


#endif /* CODEBLOCDEMO_H_ */
