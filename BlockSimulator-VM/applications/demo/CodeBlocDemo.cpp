/*
 * CodeBlocDemo.cpp
 *
 *  Created on: 1 févr. 2012
 *      Author: dom
 */

#include <iostream>
#include <sstream>
#include "CodeBlocDemo.h"
#include "Scheduler.h"

using namespace std;

CodeBlocDemo::CodeBlocDemo() : CodeBloc() {
#if DEBUG_LEVEL > 3
	cout << "Constructeur CodeBlocDemo\n";
#endif
}

void CodeBlocDemo::demarre() {

#if DEBUG_LEVEL > 3
	stringstream info;
	info << "Demarrage du CodeBlocDemo dans le bloc " << bloc->blocID;
	Scheduler::trace(info.str());
#endif

	if (bloc->blocID == 4) {
		Scheduler::schedule(new EvenementDebutDeplaceBloc(Scheduler::getHeureActuelle()+50,bloc,EST,1000000));
	}

	if (bloc->blocID == 0) {
		Scheduler::schedule(new EvenementBlocEnvoieMessage(1000000,new Message(),bloc->interfaceReseauNord));

		/*
		for (int i=0; i<10; i++) {
			if ( !bloc->interfaceReseauNord->ajouteBufferEmission(new Message(bloc,bloc->voisinNord))) {
				Scheduler::trace("J'ai essayé d'emettre mais mon interface n'est pas connectée");
			}
		}
		*/
	}
}

void CodeBlocDemo::traiteEvenementLocal(Evenement *ev) {
	static int etat = 0;

#if DEBUG_LEVEL > 1
	stringstream info;

	if (ev->typeEvenement == TYPE_EV_DEBUT_DEPLACE_GROUPE_BLOCS) {
		info << "Je suis le bloc " << bloc->blocID << " et je viens de commencer un déplacement de groupe";
		Scheduler::trace(info.str());
	}
	if (ev->typeEvenement == TYPE_EV_FIN_DEPLACE_GROUPE_BLOCS) {
		info << "Je suis le bloc " << bloc->blocID << " et je viens de terminer un déplacement de groupe";
		Scheduler::trace(info.str());
	}
	if (ev->typeEvenement == TYPE_EV_DEBUT_DEPLACE_BLOC) {
		info << "Je suis le bloc " << bloc->blocID << " et je viens de commencer un déplacement";
		Scheduler::trace(info.str());
	}
	if (ev->typeEvenement == TYPE_EV_FIN_DEPLACE_BLOC) {
		info << "Je suis le bloc " << bloc->blocID << " et je viens de terminer un déplacement";
		Scheduler::trace(info.str());
	}
	if (ev->typeEvenement == TYPE_EV_ECHEC_DEBUT_DEPLACE_BLOC) {
		info << "Je suis le bloc " << bloc->blocID << " et je viens de tenter un déplacement qui a échoué";
		Scheduler::trace(info.str());
	}
	if (ev->typeEvenement == TYPE_EV_DETECTE_NOUVEAU_VOISIN) {
		info << "Je suis le bloc " << bloc->blocID << " et je viens de détecter un nouveau voisin";
		EvenementDetecteNouveauVoisin *ev2 = (EvenementDetecteNouveauVoisin*)ev;
		switch (ev2->direction) {
		case NORD:
			if (bloc->voisinNord) { info << " (le bloc " << bloc->voisinNord->blocID << ")"; }
			info << " au Nord";
			break;
		case SUD:
			if (bloc->voisinSud) { info << " (le bloc " << bloc->voisinSud->blocID << ")"; }
			info << " au Sud";
			break;
		case EST:
			if (bloc->voisinEst) { info << " (le bloc " << bloc->voisinEst->blocID << ")"; }
			info << " à l'Est";
			break;
		case OUEST:
			if (bloc->voisinOuest) { info << " (le bloc " << bloc->voisinOuest->blocID << ")"; }
			info << " à l'Ouest";
			break;
		default:
			break;
		}
		Scheduler::trace(info.str());
	}
	if (ev->typeEvenement == TYPE_EV_PERD_ANCIEN_VOISIN) {
		info << "Je suis le bloc " << bloc->blocID << " et je viens de perdre mon voisin";
		EvenementPerdAncienVoisin *ev3 = (EvenementPerdAncienVoisin*)ev;
		switch (ev3->direction) {
		case NORD:
			if (bloc->voisinNord) { info << " (le bloc " << bloc->voisinNord->blocID << ")"; }
			info << " du Nord";
			break;
		case SUD:
			if (bloc->voisinSud) { info << " (le bloc " << bloc->voisinSud->blocID << ")"; }
			info << " du Sud";
			break;
		case EST:
			if (bloc->voisinEst) { info << " (le bloc " << bloc->voisinEst->blocID << ")"; }
			info << " de l'Est";
			break;
		case OUEST:
			if (bloc->voisinOuest) { info << " (le bloc " << bloc->voisinOuest->blocID << ")"; }
			info << " de l'Ouest";
			break;
		default:
			break;
		}
		Scheduler::trace(info.str());
	}

	info.str("");
#endif

	if (bloc->blocID == 0 && ev->typeEvenement == TYPE_EV_FIN_DEPLACE_BLOC) {

#if DEBUG_LEVEL > 1
		info << "Je viens de terminer un déplacement et je détermine quoi faire ensuite (bloc " << bloc->blocID << ") ";
		Scheduler::trace(info.str());
		info.str("");
#endif

		switch (etat) {
		case 0:
#if DEBUG_LEVEL > 1
			info << "etat 1";
			Scheduler::trace(info.str());
#endif
			Scheduler::schedule(new EvenementDebutDeplaceBloc(Scheduler::getHeureActuelle()+10,bloc,EST,1000000));
			etat++;
			break;
		case 1:
#if DEBUG_LEVEL > 1
			info << "etat 2";
			Scheduler::trace(info.str());
#endif
			Scheduler::schedule(new EvenementDebutDeplaceBloc(Scheduler::getHeureActuelle()+10,bloc,EST,1000000));
			etat++;
			break;
		case 2:
#if DEBUG_LEVEL > 1
			info << "etat 3";
			Scheduler::trace(info.str());
#endif
			Scheduler::schedule(new EvenementDebutDeplaceBloc(Scheduler::getHeureActuelle()+10,bloc,EST,1000000));
			etat++;
			break;
		case 3:
#if DEBUG_LEVEL > 1
			info << "etat 4";
			Scheduler::trace(info.str());
#endif
			Scheduler::schedule(new EvenementDebutDeplaceBloc(Scheduler::getHeureActuelle()+10,bloc,SUD,1000000));
			etat++;
			break;
		default:
			break;
		}
		dateDisponibilite=Scheduler::getHeureActuelle()+10000;
	}


	if (ev->typeEvenement == TYPE_EV_BLOC_ENVOIE_MESSAGE) {
#if DEBUG_LEVEL > 1
		info.str("");
		info << "Je viens de commencer à envoyer un message (bloc " << bloc->blocID << ") ";
		Scheduler::trace(info.str());
#endif
	}
	if (ev->typeEvenement == TYPE_EV_BLOC_RECOIT_MESSAGE) {
//		EvenementBlocRecoitMessage *evm = (EvenementBlocRecoitMessage*)ev;
#if DEBUG_LEVEL > 1
		info.str("");
		info << "Je viens de recevoir un message (bloc " << bloc->blocID << ") ";
		Scheduler::trace(info.str());
#endif
		dateDisponibilite=Scheduler::getHeureActuelle()+10;
	}
	if (ev->typeEvenement == TYPE_EV_CONFIRME_RECEPTION_MESSAGE) {
//		EvenementBlocRecoitMessage *evm = (EvenementBlocRecoitMessage*)ev;
#if DEBUG_LEVEL > 1
		info.str("");
		info << "Le message que j'ai envoyé a été acquitté (bloc " << bloc->blocID << ") ";
		Scheduler::trace(info.str());
#endif
		dateDisponibilite=Scheduler::getHeureActuelle()+10;
	}
}
