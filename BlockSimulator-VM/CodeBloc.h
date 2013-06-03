/*
 * CodeBloc.h
 *
 *  Created on: 1 févr. 2012
 *      Author: dom
 */

#ifndef CODEBLOC_H_
#define CODEBLOC_H_

#include <stdint.h>

class Evenement;
class Message;
class Bloc;

class CodeBloc {
private:

public:
	Bloc *bloc;
	CodeBloc();
	uint64_t dateDisponibilite;

	virtual void demarre();
	virtual void demarre(Message *msg);
	virtual void traiteEvenementLocal();
	virtual void traiteEvenementLocal(Evenement *ev);

	Evenement *getEvenementActuel();
	void finiTraitementEvenementLocal(Evenement *ev);
};


#endif /* CODEBLOC_H_ */
