/*
 * CodeBloc.cpp
 *
 *  Created on: 1 févr. 2012
 *      Author: dom
 */
#include <sstream>
#include <iostream>
#include "Scheduler.h"
#include "CodeBloc.h"
#include "Blocs.h"

using namespace std;

CodeBloc::CodeBloc() {
	dateDisponibilite=0;
#if DEBUG_LEVEL > 3
	cout << "Constructeur CodeBloc\n";
#endif
}

void CodeBloc::demarre() {
	stringstream message;
	message << "Demarrage du CodeBloc dans le bloc " << bloc->blocID;
	Scheduler::trace(message.str());
	return;
}

void CodeBloc::demarre(Message *msg) {
	demarre();
	return;
}

void CodeBloc::traiteEvenementLocal(Evenement *ev) {

}

Evenement* CodeBloc::getEvenementActuel() {
	Evenement *ev;
	if (bloc->listeEvenementsLocaux.size()==0) {
		cout << "ERREUR : la liste des événements locaux ne devrait pas être vide !" << endl;
		exit(EXIT_FAILURE);
	}
	ev = bloc->listeEvenementsLocaux.front();
	bloc->listeEvenementsLocaux.pop_front();

	return(ev);
}

void CodeBloc::finiTraitementEvenementLocal(Evenement *ev) {
	if (dateDisponibilite < Scheduler::getHeureActuelle()) dateDisponibilite = Scheduler::getHeureActuelle();
	if (bloc->listeEvenementsLocaux.size() > 0) {
		Scheduler::schedule(new EvenementCodeBlocTraiteEvenementLocal(dateDisponibilite,this));
	}

	if (ev->typeEvenement == TYPE_EV_BLOC_RECOIT_MESSAGE) {
		EvenementBlocRecoitMessage *ev2 = (EvenementBlocRecoitMessage*)ev;
		ev2->message->nbRef--;
		if (ev2->message->nbRef <= 0) {
			delete(ev2->message);
		}
	}
	if (ev->typeEvenement == TYPE_EV_CONFIRME_RECEPTION_MESSAGE) {
		EvenementConfirmeReceptionMessage *ev3 = (EvenementConfirmeReceptionMessage*)ev;
		ev3->message->nbRef--;
		if (ev3->message->nbRef <= 0) {
			delete(ev3->message);
		}
	}

	ev->nbRef--;
    if (ev->nbRef <= 0) {
  	  delete(ev);
    }
}

void CodeBloc::traiteEvenementLocal() {
	Evenement *ev = getEvenementActuel();

	traiteEvenementLocal(ev);

	finiTraitementEvenementLocal(ev);
}
