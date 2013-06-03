/*
 * CodeBlocDemo2.cpp
 *
 *  Created on: 5 févr. 2012
 *      Author: dom
 */

#include <iostream>
#include <sstream>
#include "CodeBlocDemo2.h"
#include "Scheduler.h"

using namespace std;

MessageDiffusionPosition::MessageDiffusionPosition(int x, int y) : Message() {
	typeMessage = TYPE_MSG_DIFFUSION_POSITION;
	posX = x;
	posY = y;
}

CodeBlocDemo2::CodeBlocDemo2() : CodeBloc() {
#if DEBUG_LEVEL > 3
	cout << "Constructeur CodeBlocDemo2\n";
#endif
}

void CodeBlocDemo2::demarre() {

	uint64_t heureActuelle = Scheduler::getHeureActuelle();

	posX = 0;
	posY = 0;
	estPositionne = false;

#if DEBUG_LEVEL > 3
	stringstream info;
	info << "Demarrage du CodeBlocDemo2 dans le bloc " << bloc->blocID;
	Scheduler::trace(info.str());
#endif

	if (bloc->blocID == 0) {

		//Scheduler::schedule(new EvenementDebutDeplaceBloc(heureActuelle+10,bloc,SUD,1000000));

		estPositionne = true;
		if (bloc->voisinNord != NULL) {
			Scheduler::schedule(new EvenementBlocEnvoieMessage(heureActuelle, new MessageDiffusionPosition(posX,posY+1), bloc->interfaceReseauNord));
		}
		if (bloc->voisinSud != NULL) {
			Scheduler::schedule(new EvenementBlocEnvoieMessage(heureActuelle, new MessageDiffusionPosition(posX,posY-1), bloc->interfaceReseauSud));
		}
		if (bloc->voisinEst != NULL) {
			Scheduler::schedule(new EvenementBlocEnvoieMessage(heureActuelle, new MessageDiffusionPosition(posX+1,posY), bloc->interfaceReseauEst));
		}
		if (bloc->voisinOuest != NULL) {
			Scheduler::schedule(new EvenementBlocEnvoieMessage(heureActuelle, new MessageDiffusionPosition(posX-1,posY), bloc->interfaceReseauOuest));
		}
	}

}

void CodeBlocDemo2::traiteEvenementLocal(Evenement *ev) {
	stringstream info;
	uint64_t heureActuelle = Scheduler::getHeureActuelle();

	if (ev->typeEvenement == TYPE_EV_BLOC_RECOIT_MESSAGE) {
		EvenementBlocRecoitMessage *evm = (EvenementBlocRecoitMessage*)ev;

		if (!estPositionne && evm->message->typeMessage == TYPE_MSG_DIFFUSION_POSITION) {
			MessageDiffusionPosition *message = (MessageDiffusionPosition*)evm->message;
			posX = message->posX;
			posY = message->posY;
			estPositionne = true;

#if DEBUG_LEVEL > 3
			info.str("");
			info << "Je suis le bloc " << bloc->blocID << " et je viens de recevoir un message de positionnement. Je suis en " << posX << "," << posY;
#endif			Scheduler::trace(info.str());

			if (bloc->voisinNord != NULL) {
				Scheduler::schedule(new EvenementBlocEnvoieMessage(heureActuelle, new MessageDiffusionPosition(posX,posY+1), bloc->interfaceReseauNord));
			}
			if (bloc->voisinSud != NULL) {
				Scheduler::schedule(new EvenementBlocEnvoieMessage(heureActuelle, new MessageDiffusionPosition(posX,posY-1), bloc->interfaceReseauSud));
			}
			if (bloc->voisinEst != NULL) {
				Scheduler::schedule(new EvenementBlocEnvoieMessage(heureActuelle, new MessageDiffusionPosition(posX+1,posY), bloc->interfaceReseauEst));
			}
			if (bloc->voisinOuest != NULL) {
				Scheduler::schedule(new EvenementBlocEnvoieMessage(heureActuelle, new MessageDiffusionPosition(posX-1,posY), bloc->interfaceReseauOuest));
			}
		}
		dateDisponibilite=Scheduler::getHeureActuelle()+10;
	}

	if (ev->typeEvenement == TYPE_EV_CONFIRME_RECEPTION_MESSAGE) {
		EvenementConfirmeReceptionMessage *evm = (EvenementConfirmeReceptionMessage*)ev;
#if DEBUG_LEVEL > 3
		info.str("");
		info << "Je suis le bloc " << bloc->blocID << " et le bloc " << evm->message->blocDestination->blocID << " a bien reçu le message que je lui ai envoyé";
		Scheduler::trace(info.str());
#endif
		dateDisponibilite=Scheduler::getHeureActuelle()+10;
	}
}
