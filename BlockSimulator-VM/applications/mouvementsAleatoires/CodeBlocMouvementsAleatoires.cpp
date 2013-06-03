/*
 * CodeBlocMouvementsAleatoires.c
 *
 *  Created on: 16 févr. 2012
 *      Author: dom
 */

#include <iostream>
#include <sstream>
#include "CodeBlocMouvementsAleatoires.h"
#include "Scheduler.h"

using namespace std;

MessageDiffusionPosition::MessageDiffusionPosition(int x, int y) : Message() {
	typeMessage = TYPE_MSG_DIFFUSION_POSITION;
	posX = x;
	posY = y;
}

CodeBlocMouvementsAleatoires::CodeBlocMouvementsAleatoires() : CodeBloc() {
#if DEBUG_LEVEL > 3
	cout << "Constructeur CodeBlocMouvementsAleatoires\n";
#endif
}

void CodeBlocMouvementsAleatoires::demarre() {

	uint64_t heureActuelle = Scheduler::getHeureActuelle();

	posX = 0;
	posY = 0;
	estPositionne = false;

	direction_E direction;

#if DEBUG_LEVEL > 3
	stringstream info;
	info << "Demarrage du CodeBlocMouvementsAleatoires dans le bloc " << bloc->blocID;
	Scheduler::trace(info.str());
#endif

	double alea = ((double)random()/RAND_MAX)*4;
	if (alea < 1) direction = NORD;
	if (alea >= 1 && alea < 2) direction = SUD;
	if (alea >= 2 && alea < 3) direction = EST;
	if (alea >= 3 && alea < 4) direction = OUEST;

	double dureeAlea = ((double)random()/RAND_MAX)*1000000;

	Scheduler::schedule(new EvenementDebutDeplaceBloc(heureActuelle+dureeAlea,bloc,direction,1000000));

	bloc->setColor(0.8,0.8,0.8);
	if (bloc->blocID == 0 && bloc->etatBloc == IMMOBILE) {
		bloc->setColor(1,0,0);
		estPositionne = true;
		annonceMaPositionAuxVoisins();
	}
}

void CodeBlocMouvementsAleatoires::traiteEvenementLocal(Evenement *ev) {
	stringstream info;
	uint64_t heureActuelle = Scheduler::getHeureActuelle();
	direction_E direction;

	if (ev->typeEvenement == TYPE_EV_FIN_DEPLACE_BLOC && estPositionne) {
		EvenementFinDeplaceBloc *efdb = (EvenementFinDeplaceBloc*)ev;
		switch (efdb->direction) {
		case NORD:
				posY += 1;
				break;
		case SUD:
				posY -= 1;
				break;
		case EST:
				posX += 1;
				break;
		case OUEST: posX -= 1;
				break;
		default:
			break;
		}
		annonceMaPositionAuxVoisins();
	}
	if (ev->typeEvenement == TYPE_EV_FIN_DEPLACE_BLOC || ev->typeEvenement == TYPE_EV_ECHEC_DEBUT_DEPLACE_BLOC) {
		double alea = ((double)random()/RAND_MAX)*4;
		if (alea < 1) direction = NORD;
		if (alea >= 1 && alea < 2) direction = SUD;
		if (alea >= 2 && alea < 3) direction = EST;
		if (alea >= 3 && alea < 4) direction = OUEST;

		double dureeAlea = ((double)random()/RAND_MAX)*1000000;

		Scheduler::schedule(new EvenementDebutDeplaceBloc(heureActuelle+dureeAlea,bloc,direction,1000000));
	}

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
			Scheduler::trace(info.str());
#endif
			annonceMaPositionAuxVoisins();
			bloc->setColor(1,0,1);
		}
		dateDisponibilite=Scheduler::getHeureActuelle()+10;

	}

}

void CodeBlocMouvementsAleatoires::annonceMaPositionAuxVoisins() {
//	stringstream info;
//	info << "Bloc " << bloc->blocID;
//	info << "  J'annonce ma position aux voisins" << endl;
//	Scheduler::trace(info.str());

	Scheduler::printf(bloc,"%d annonce sa position",bloc->blocID);

	uint64_t heureActuelle = Scheduler::getHeureActuelle();
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
