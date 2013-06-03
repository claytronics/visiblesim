/*
 * CodeBlocDemoMouvementsGroupes.cpp
 *
 *  Created on: 29 févr. 2012
 *      Author: dom
 */

#include <iostream>
#include <sstream>
#include "CodeBlocDemoMouvementsGroupes.h"
#include "Scheduler.h"

using namespace std;

MessageConstitutionGroupe::MessageConstitutionGroupe(vector<Bloc*> vectBloc, float r, float g, float b) : Message() {
	typeMessage = TYPE_MSG_CONSTITUTION_GROUPE;
	vecteurBlocs = vector<Bloc*>(vectBloc);
	rCol = r;
	gCol = g;
	bCol = b;
}

CodeBlocDemoMouvementsGroupes::CodeBlocDemoMouvementsGroupes() : CodeBloc() {
#if DEBUG_LEVEL > 3
	cout << "Constructeur CodeBlocDemoMouvementsGroupes\n";
#endif
	queueDeTrain = false;
	locomotive = false;
}

void CodeBlocDemoMouvementsGroupes::demarre() {
	GLfloat x, y;
	int yi;
	bloc->get2DPosition(x,y);
	yi=y;
	if (yi%2 == 0) return;
	// si je n'ai pas de voisin à l'ouest, je suis en queue de train
	// dans ce cas j'envoie un message à mon voisin est pour construire la liste des membres du groupe
	if (bloc->voisinOuest == NULL) {
		queueDeTrain = true;
		bloc->setColor(1,0,0);

		vector<Bloc*> listeBlocs;
		listeBlocs.push_back(bloc);

		Scheduler::schedule(new EvenementBlocEnvoieMessage(Scheduler::getHeureActuelle(), new MessageConstitutionGroupe(listeBlocs,1,1,0),bloc->interfaceReseauEst));
	}

	// si je n'ai pas de voisin à l'est, je suis en tête du train
	// dans ce cas j'attend de recevoir la liste de groupe et je ferai bouger tout le monde d'un coup
	if (bloc->voisinEst == NULL) {
		locomotive = true;
		bloc->setColor(0,1,0);
	}

	if (!locomotive && !queueDeTrain) {
		bloc->setColor(0.5,0.5,0.5);
	}
}

void CodeBlocDemoMouvementsGroupes::traiteEvenementLocal(Evenement *ev) {
	EvenementBlocRecoitMessage *evm;
	MessageConstitutionGroupe *messageConstitutionGroupe;

	float r,g,b;

	vector<Bloc *>vecteurBlocs;

	switch (ev->typeEvenement) {
	case TYPE_EV_BLOC_RECOIT_MESSAGE:
		evm = (EvenementBlocRecoitMessage*)ev;
		if (evm->message->typeMessage == TYPE_MSG_CONSTITUTION_GROUPE) {
			messageConstitutionGroupe = (MessageConstitutionGroupe*)evm->message;
			vecteurBlocs = vector<Bloc*>(messageConstitutionGroupe->vecteurBlocs);
			vecteurBlocs.push_back(bloc);
			r = messageConstitutionGroupe->rCol;
			g = messageConstitutionGroupe->gCol;
			b = messageConstitutionGroupe->bCol;

			if (!queueDeTrain && !locomotive) {
				bloc->setColor(r,g,b);
				Scheduler::schedule(new EvenementBlocEnvoieMessage(Scheduler::getHeureActuelle(), new MessageConstitutionGroupe(vecteurBlocs,r,g,b),bloc->interfaceReseauEst));
			}

			if (locomotive) {
				cout << "locomotive " <<vecteurBlocs.size() << endl;
				Scheduler::printf(bloc,"loco %d",vecteurBlocs.size());
				Scheduler::schedule(new EvenementDebutDeplaceGroupeBlocs(Scheduler::getHeureActuelle(),&vecteurBlocs,EST,1000000));
			}

		}
		break;
	default:
		break;
	}
}
