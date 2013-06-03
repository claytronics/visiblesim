/*
 * Evenements.cpp
 *
 *  Created on: 29 janv. 2012
 *      Author: dom
 */

#include <iostream>
#include <sstream>

#include "Scheduler.h"
#include "Evenements.h"
#include "physics.h"

using namespace std;

unsigned int Evenement::nextID = 0;
unsigned int Evenement::nextIDGroupeBlocs = 0;

unsigned int Evenement::nbEvenements = 0;

//===========================================================================================================
//
//          Evenement  (class)
//
//===========================================================================================================

Evenement::Evenement(uint64_t heure) {
  id = nextID++;
  nbRef = 0;
  typeEvenement = TYPE_EV_GENERIQUE;
  nbEvenements++;
//  cout.precision(6);
//  cout << fixed << (double)(Scheduler::getHeureActuelle())/1000000 << " Création Evenement (" << id << ")\n";

  heureEvenement = heure;
}

Evenement::~Evenement() {
  nbEvenements--;
  //cout.precision(6);
  //cout << fixed << (double)(Scheduler::getHeureActuelle())/1000000 << "  Destruction Evenement (" << id << ")\n";
}

void Evenement::consomme() {
//  cout.precision(6);
//  cout << fixed << double(Scheduler::getHeureActuelle())/1000000 << "  Consomme l'événement (" << id << ")\n";
}

//===========================================================================================================
//
//          EvenementDebutDeplaceBloc  (class)
//
//===========================================================================================================

EvenementDebutDeplaceBloc::EvenementDebutDeplaceBloc(uint64_t heure, Bloc* bloc, direction_E dir, uint64_t duree) : Evenement(heure) {
	blocConcerne = bloc;
	dureeDeplacement = duree;
	direction = dir;
	typeEvenement = TYPE_EV_DEBUT_DEPLACE_BLOC;
}

void EvenementDebutDeplaceBloc::consomme() {
	GLfloat xd,yd;
	uint64_t heureActuelle;
	bool echec = false;
	int x,y;
	int xdest, ydest;
	Cellule *celluleActuelle;
	Cellule *celluleDest;

	if (blocConcerne->etatBloc != IMMOBILE) {
		cout << "WARNING : un mouvement est déjà en cours" << endl;
		echec = true;
	}

	if (!echec) {
		heureActuelle = Scheduler::getHeureActuelle();
		blocConcerne->get2DPosition(xd, yd);

		x = (int)floor(xd);
		y = (int)floor(yd);

		xdest = x;
		ydest = y;

		switch (direction) {
		case NORD:
			ydest += 1;
			break;
		case SUD:
			ydest -= 1;
			break;
		case EST:
			xdest += 1;
			break;
		case OUEST:
			xdest -= 1;
			break;
		default:
			cout << "ERREUR : direction non valide !" << endl;
			exit(EXIT_FAILURE);
		}

		celluleActuelle = GrilleSupport::getCellule(x,y);
		celluleDest = GrilleSupport::getCellule(xdest,ydest);

		if (celluleDest == NULL) {
			echec = true;
		}
	}

	if (!echec) {
		if (celluleDest->blocContenu != NULL || celluleDest->blocQuiVient != NULL || celluleDest->blocQuiPart != NULL) {
#if DEBUG_LEVEL > 3
			stringstream info;
			info << "EvenementDebutDeplaceBloc ma destination est occupée, je n'y vais pas (bloc " << blocConcerne->blocID << ")";
			Scheduler::trace(info.str());
#endif
			echec = true;
		}
	}

	if (!echec) {
#if DEBUG_LEVEL > 3
		stringstream info;
		info << "Je commence à bouger (bloc " << blocConcerne->blocID << ")";
		Scheduler::trace(info.str());
#endif

		blocConcerne->CelluleSource = celluleActuelle;
		blocConcerne->CelluleCible = celluleDest;

		celluleDest->blocQuiVient = blocConcerne;
		celluleActuelle->blocContenu = NULL;
		celluleActuelle->blocQuiPart = blocConcerne;

		blocConcerne->etatBloc = direction;
		blocConcerne->positionDepart.posX = xd;
		blocConcerne->positionDepart.posY = yd;
		blocConcerne->positionDepart.heure = heureActuelle;

		blocConcerne->positionArrivee.posX = xdest;
		blocConcerne->positionArrivee.posY = ydest;

		blocConcerne->positionArrivee.heure = heureActuelle+dureeDeplacement;

		blocConcerne->voisinNord = NULL;
		blocConcerne->voisinSud = NULL;
		blocConcerne->voisinEst = NULL;
		blocConcerne->voisinOuest = NULL;

		Cellule *cellNord = GrilleSupport::getCellule(x,y+1);
		Cellule *cellSud = GrilleSupport::getCellule(x,y-1);
		Cellule *cellEst = GrilleSupport::getCellule(x+1,y);
		Cellule *cellOuest = GrilleSupport::getCellule(x-1,y);
		if (cellNord != NULL && cellNord->blocContenu != NULL) {
			cellNord->blocContenu->voisinSud = NULL;
			cellNord->blocContenu->scheduleEvenementLocal(new EvenementPerdAncienVoisin(Scheduler::getHeureActuelle(),cellNord->blocContenu,SUD));
		}
		if (cellSud != NULL && cellSud->blocContenu != NULL) {
			cellSud->blocContenu->voisinNord = NULL;
			cellSud->blocContenu->scheduleEvenementLocal(new EvenementPerdAncienVoisin(Scheduler::getHeureActuelle(),cellSud->blocContenu,NORD));
		}
		if (cellEst != NULL && cellEst->blocContenu != NULL) {
			cellEst->blocContenu->voisinOuest = NULL;
			cellEst->blocContenu->scheduleEvenementLocal(new EvenementPerdAncienVoisin(Scheduler::getHeureActuelle(),cellEst->blocContenu,OUEST));
		}
		if (cellOuest != NULL && cellOuest->blocContenu != NULL) {
			cellOuest->blocContenu->voisinEst = NULL;
			cellOuest->blocContenu->scheduleEvenementLocal(new EvenementPerdAncienVoisin(Scheduler::getHeureActuelle(),cellOuest->blocContenu,EST));
		}

		blocConcerne->interfaceReseauNord->deconnecte();
		blocConcerne->interfaceReseauSud->deconnecte();
		blocConcerne->interfaceReseauEst->deconnecte();
		blocConcerne->interfaceReseauOuest->deconnecte();

		blocConcerne->scheduleEvenementLocal(this);
		Scheduler::schedule(new EvenementFinDeplaceBloc(heureActuelle+dureeDeplacement, blocConcerne, direction, dureeDeplacement));
	}

	if (echec) {
		Scheduler::schedule(new EvenementEchecDebutDeplaceBloc(Scheduler::getHeureActuelle(),blocConcerne,direction,dureeDeplacement));
	}
}

//===========================================================================================================
//
//          EvenementFinDeplaceBloc  (class)
//
//===========================================================================================================

EvenementFinDeplaceBloc::EvenementFinDeplaceBloc(uint64_t heure, Bloc* bloc, direction_E dir, uint64_t duree) : Evenement(heure) {
	blocConcerne = bloc;
	typeEvenement = TYPE_EV_FIN_DEPLACE_BLOC;
	direction = dir;
	dureeDeplacement = duree;
}

void EvenementFinDeplaceBloc::consomme() {
	GLfloat xd,yd;
	blocConcerne->get2DPosition(xd, yd);

	int x,y;
	x = (int)floor(xd);
	y = (int)floor(yd);

	blocConcerne->etatBloc = IMMOBILE;
	blocConcerne->positionDepart.posX = blocConcerne->positionArrivee.posX;
	blocConcerne->positionDepart.posY = blocConcerne->positionArrivee.posY;

	blocConcerne->CelluleSource->blocQuiPart = NULL;
	blocConcerne->CelluleCible->blocQuiVient = NULL;
	blocConcerne->CelluleCible->blocContenu = blocConcerne;

	Cellule *cellNord = GrilleSupport::getCellule(x,y+1);
	Cellule *cellSud = GrilleSupport::getCellule(x,y-1);
	Cellule *cellEst = GrilleSupport::getCellule(x+1,y);
	Cellule *cellOuest = GrilleSupport::getCellule(x-1,y);

	if (cellNord != NULL && cellNord->blocContenu != NULL) {
		blocConcerne->voisinNord = cellNord->blocContenu;
		cellNord->blocContenu->voisinSud = blocConcerne;
		cellNord->blocContenu->scheduleEvenementLocal(new EvenementDetecteNouveauVoisin(Scheduler::getHeureActuelle(),cellNord->blocContenu,SUD));
		blocConcerne->interfaceReseauNord->connecte(cellNord->blocContenu->interfaceReseauSud);
	}
	if (cellSud != NULL && cellSud->blocContenu != NULL) {
		blocConcerne->voisinSud = cellSud->blocContenu;
		cellSud->blocContenu->voisinNord = blocConcerne;
		cellSud->blocContenu->scheduleEvenementLocal(new EvenementDetecteNouveauVoisin(Scheduler::getHeureActuelle(),cellSud->blocContenu,NORD));
		blocConcerne->interfaceReseauSud->connecte(cellSud->blocContenu->interfaceReseauNord);
	}
	if (cellEst != NULL && cellEst->blocContenu != NULL) {
		blocConcerne->voisinEst = cellEst->blocContenu;
		cellEst->blocContenu->voisinOuest = blocConcerne;
		cellEst->blocContenu->scheduleEvenementLocal(new EvenementDetecteNouveauVoisin(Scheduler::getHeureActuelle(),cellEst->blocContenu,OUEST));
		blocConcerne->interfaceReseauEst->connecte(cellEst->blocContenu->interfaceReseauOuest);
	}
	if (cellOuest != NULL && cellOuest->blocContenu != NULL) {
		blocConcerne->voisinOuest = cellOuest->blocContenu;
		cellOuest->blocContenu->voisinEst = blocConcerne;
		cellOuest->blocContenu->scheduleEvenementLocal(new EvenementDetecteNouveauVoisin(Scheduler::getHeureActuelle(),cellOuest->blocContenu,EST));
		blocConcerne->interfaceReseauOuest->connecte(cellOuest->blocContenu->interfaceReseauEst);
	}

	blocConcerne->scheduleEvenementLocal(this);
	return;
}

//===========================================================================================================
//
//          EvenementDebutDeplaceGroupeBlocs  (class)
//
//===========================================================================================================

EvenementDebutDeplaceGroupeBlocs::EvenementDebutDeplaceGroupeBlocs(uint64_t heure, vector<Bloc*> *blocs, direction_E dir, uint64_t duree) : Evenement(heure) {
	blocsConcernes = vector<Bloc*>(*blocs);
	dureeDeplacement = duree;
	direction = dir;
	typeEvenement = TYPE_EV_DEBUT_DEPLACE_GROUPE_BLOCS;
}

void EvenementDebutDeplaceGroupeBlocs::consomme() {
	vector<Bloc*>::iterator it;

	bool tousImmobiles = true;
	bool destinationOk = true;

	GLfloat xd,yd;
	int x,y;
	int xdest, ydest;

	Bloc *bloc;

	Cellule *celluleActuelle;
	Cellule *celluleDest;

	uint64_t heureActuelle;
	unsigned int idGroupeBlocs;

	heureActuelle = Scheduler::getHeureActuelle();
	idGroupeBlocs = ++Evenement::nextIDGroupeBlocs;

	for (it=blocsConcernes.begin(); it != blocsConcernes.end(); it++) {
		bloc = (*it);
		if (bloc->etatBloc != IMMOBILE) {
			tousImmobiles = false;
		}
		bloc->idGroupeBlocs = idGroupeBlocs;
	}

	for (it=blocsConcernes.begin(); it != blocsConcernes.end(); it++) {
		bloc = (*it);
		bloc->get2DPosition(xd, yd);
		x = (int)floor(xd);
		y = (int)floor(yd);
		xdest = x;
		ydest = y;
		switch (direction) {
		case NORD:
			ydest += 1;
			break;
		case SUD:
			ydest -= 1;
			break;
		case EST:
			xdest += 1;
			break;
		case OUEST:
			xdest -= 1;
			break;
		default:
			cout << "ERREUR : direction non valide !" << endl;
			exit(EXIT_FAILURE);
		}

		celluleDest = GrilleSupport::getCellule(xdest,ydest);
		if (celluleDest == NULL) {
			 destinationOk = false;
		} else {
			if ((celluleDest->blocContenu != NULL && celluleDest->blocContenu->idGroupeBlocs != idGroupeBlocs) ||
				(celluleDest->blocQuiPart != NULL && celluleDest->blocQuiPart->idGroupeBlocs != idGroupeBlocs) ||
				(celluleDest->blocQuiVient != NULL && celluleDest->blocQuiVient->idGroupeBlocs != idGroupeBlocs)) {
				destinationOk = false;
			}
		}
	}
	if (!tousImmobiles) {
		stringstream info;
		info << "WARNING : un mouvement est déjà en cours" << endl;
		Scheduler::trace(info.str());
		return;
	}
	if (!destinationOk) {
		stringstream info;
		info << "WARNING : Un des bocs cherche à aller sur une case occupée ou interdite";
		Scheduler::trace(info.str());
		return;

	}

	for (it=blocsConcernes.begin(); it != blocsConcernes.end(); it++) {
		bloc = (*it);
		bloc->get2DPosition(xd, yd);
		x = (int)floor(xd);
		y = (int)floor(yd);
		xdest = x;
		ydest = y;
		switch (direction) {
		case NORD:
			ydest += 1;
			break;
		case SUD:
			ydest -= 1;
			break;
		case EST:
			xdest += 1;
			break;
		case OUEST:
			xdest -= 1;
			break;
		default:
			cout << "ERREUR : direction non valide !" << endl;
			exit(EXIT_FAILURE);
		}

		celluleActuelle = GrilleSupport::getCellule(x,y);
		celluleDest = GrilleSupport::getCellule(xdest,ydest);

		celluleActuelle->blocContenu = NULL;
		celluleActuelle->blocQuiPart = bloc;
		celluleDest->blocQuiVient = bloc;

		bloc->CelluleSource = celluleActuelle;
		bloc->CelluleCible = celluleDest;

		bloc->etatBloc = direction;
		bloc->positionDepart.posX = xd;
		bloc->positionDepart.posY = yd;
		bloc->positionDepart.heure = heureActuelle;

		bloc->positionArrivee.posX = xdest;
		bloc->positionArrivee.posY = ydest;

		bloc->positionArrivee.heure = heureActuelle+dureeDeplacement;

		Cellule *cellNord = GrilleSupport::getCellule(x,y+1);
		Cellule *cellSud = GrilleSupport::getCellule(x,y-1);
		Cellule *cellEst = GrilleSupport::getCellule(x+1,y);
		Cellule *cellOuest = GrilleSupport::getCellule(x-1,y);

		if (bloc->voisinNord != NULL && bloc->voisinNord->idGroupeBlocs != idGroupeBlocs) {
			bloc->voisinNord = NULL;
			if (cellNord != NULL && cellNord->blocContenu != NULL) {
				cellNord->blocContenu->voisinSud = NULL;
				cellNord->blocContenu->scheduleEvenementLocal(new EvenementPerdAncienVoisin(Scheduler::getHeureActuelle(),cellNord->blocContenu,SUD));
				bloc->interfaceReseauNord->deconnecte();
			}
		}
		if (bloc->voisinSud != NULL && bloc->voisinSud->idGroupeBlocs != idGroupeBlocs) {
			bloc->voisinSud = NULL;
			if (cellSud != NULL && cellSud->blocContenu != NULL) {
				cellSud->blocContenu->voisinNord = NULL;
				cellSud->blocContenu->scheduleEvenementLocal(new EvenementPerdAncienVoisin(Scheduler::getHeureActuelle(),cellSud->blocContenu,NORD));
				bloc->interfaceReseauSud->deconnecte();
			}
		}
		if (bloc->voisinEst != NULL && bloc->voisinEst->idGroupeBlocs != idGroupeBlocs) {
			bloc->voisinEst = NULL;
			if (cellEst != NULL && cellEst->blocContenu != NULL) {
				cellEst->blocContenu->voisinOuest = NULL;
				cellEst->blocContenu->scheduleEvenementLocal(new EvenementPerdAncienVoisin(Scheduler::getHeureActuelle(),cellEst->blocContenu,OUEST));
				bloc->interfaceReseauEst->deconnecte();
			}
		}
		if (bloc->voisinOuest != NULL && bloc->voisinOuest->idGroupeBlocs != idGroupeBlocs) {
			bloc->voisinOuest = NULL;
			if (cellOuest != NULL && cellOuest->blocContenu != NULL) {
				cellOuest->blocContenu->voisinEst = NULL;
				cellOuest->blocContenu->scheduleEvenementLocal(new EvenementPerdAncienVoisin(Scheduler::getHeureActuelle(),cellOuest->blocContenu,EST));
				bloc->interfaceReseauOuest->deconnecte();
			}
		}
		bloc->scheduleEvenementLocal(this);
	}

	Scheduler::schedule(new EvenementFinDeplaceGroupeBlocs(heureActuelle+dureeDeplacement, &blocsConcernes));

	stringstream info;
	info << "Commence à bouger en groupe - idGroupeBlocs : " << idGroupeBlocs << " (" << id << ")";
	Scheduler::trace(info.str());

}

//===========================================================================================================
//
//          EvenementFinDeplaceGroupeBlocs  (class)
//
//===========================================================================================================

EvenementFinDeplaceGroupeBlocs::EvenementFinDeplaceGroupeBlocs(uint64_t heure, vector<Bloc*> *blocs) : Evenement(heure) {
	blocsConcernes = vector<Bloc*>(*blocs);
	typeEvenement = TYPE_EV_FIN_DEPLACE_GROUPE_BLOCS;
}

void EvenementFinDeplaceGroupeBlocs::consomme() {
	unsigned int idGroupeBlocs;
	vector<Bloc*>::iterator it;
	Bloc *bloc;

	GLfloat xd,yd;
	int x,y;

	if (blocsConcernes.size() > 0) {
		idGroupeBlocs = (*blocsConcernes.begin())->idGroupeBlocs;
	} else {
		cerr << "ERREUR : un groupe de blocs est vide" << endl;
		exit(EXIT_FAILURE);
	}

	for (it=blocsConcernes.begin(); it != blocsConcernes.end(); it++) {
		bloc = (*it);
		bloc->etatBloc = IMMOBILE;
		bloc->positionDepart.posX = bloc->positionArrivee.posX;
		bloc->positionDepart.posY = bloc->positionArrivee.posY;

		bloc->CelluleSource->blocQuiPart = NULL;
		bloc->CelluleCible->blocQuiVient = NULL;
		bloc->CelluleCible->blocContenu = bloc;

		bloc->get2DPosition(xd, yd);
		x = (int)floor(xd);
		y = (int)floor(yd);

		Cellule *cellNord = GrilleSupport::getCellule(x,y+1);
		Cellule *cellSud = GrilleSupport::getCellule(x,y-1);
		Cellule *cellEst = GrilleSupport::getCellule(x+1,y);
		Cellule *cellOuest = GrilleSupport::getCellule(x-1,y);

		if (cellNord != NULL && cellNord->blocContenu != NULL && cellNord->blocContenu->idGroupeBlocs != idGroupeBlocs) {
			bloc->voisinNord = cellNord->blocContenu;
			cellNord->blocContenu->voisinSud = bloc;
			cellNord->blocContenu->scheduleEvenementLocal(new EvenementDetecteNouveauVoisin(Scheduler::getHeureActuelle(),cellNord->blocContenu,SUD));
			bloc->interfaceReseauNord->connecte(cellNord->blocContenu->interfaceReseauSud);
		}
		if (cellSud != NULL && cellSud->blocContenu != NULL && cellSud->blocContenu->idGroupeBlocs != idGroupeBlocs) {
			bloc->voisinSud = cellSud->blocContenu;
			cellSud->blocContenu->voisinNord = bloc;
			cellSud->blocContenu->scheduleEvenementLocal(new EvenementDetecteNouveauVoisin(Scheduler::getHeureActuelle(),cellSud->blocContenu,NORD));
			bloc->interfaceReseauSud->connecte(cellSud->blocContenu->interfaceReseauNord);
		}
		if (cellEst != NULL && cellEst->blocContenu != NULL && cellEst->blocContenu->idGroupeBlocs != idGroupeBlocs) {
			bloc->voisinEst = cellEst->blocContenu;
			cellEst->blocContenu->voisinOuest = bloc;
			cellEst->blocContenu->scheduleEvenementLocal(new EvenementDetecteNouveauVoisin(Scheduler::getHeureActuelle(),cellEst->blocContenu,OUEST));
			bloc->interfaceReseauEst->connecte(cellEst->blocContenu->interfaceReseauOuest);
		}
		if (cellOuest != NULL && cellOuest->blocContenu != NULL && cellOuest->blocContenu->idGroupeBlocs != idGroupeBlocs) {
			bloc->voisinOuest = cellOuest->blocContenu;
			cellOuest->blocContenu->voisinEst = bloc;
			cellOuest->blocContenu->scheduleEvenementLocal(new EvenementDetecteNouveauVoisin(Scheduler::getHeureActuelle(),cellOuest->blocContenu,EST));
			bloc->interfaceReseauOuest->connecte(cellOuest->blocContenu->interfaceReseauEst);
		}

		bloc->scheduleEvenementLocal(this);
	}

	stringstream info;
	info << "Fin déplacement de groupe - idGroupeBlocs : "  << idGroupeBlocs << " (" << id << ")";
	Scheduler::trace(info.str());

}

//===========================================================================================================
//
//          EvenementDemarreCodeBloc  (class)
//
//===========================================================================================================

EvenementDemarreCodeBloc::EvenementDemarreCodeBloc(uint64_t heure, Bloc* bloc,Message *msg) : Evenement(heure) {
	blocConcerne = bloc;
	message = msg;
	typeEvenement = TYPE_EV_DEMARRE_CODE_BLOC;
}

void EvenementDemarreCodeBloc::consomme() {
	if (message) 
	{ blocConcerne->codeBloc->demarre(message);
	} else
	{ blocConcerne->codeBloc->demarre();
	}
}

//===========================================================================================================
//
//          EvenementCodeBlocTraiteEvenementLocal  (class)
//
//===========================================================================================================

EvenementCodeBlocTraiteEvenementLocal::EvenementCodeBlocTraiteEvenementLocal(uint64_t heure, CodeBloc* bloc) : Evenement(heure) {
	codeBlocConcerne = bloc;
	typeEvenement = TYPE_EV_CODEBLOC_TRAITE_EV_LOCAL;
}

void EvenementCodeBlocTraiteEvenementLocal::consomme() {
	codeBlocConcerne->traiteEvenementLocal();
	return;
}

//===========================================================================================================
//
//          EvenementBlocEnvoieMessage  (class)
//
//===========================================================================================================

EvenementBlocEnvoieMessage::EvenementBlocEnvoieMessage(uint64_t heure, Message *mess, InterfaceReseau *ir) : Evenement(heure) {
	message = mess;
	interfaceConcernee = ir;
	typeEvenement = TYPE_EV_BLOC_ENVOIE_MESSAGE;

	stringstream info;
	info << "EvenementBlocEnvoieMessage : scheduling de l'envoi d'un message par le bloc " << message->blocSource->blocID;
	Scheduler::trace(info.str());
}

void EvenementBlocEnvoieMessage::consomme() {
	stringstream info;

	if ( !interfaceConcernee->interfaceConnectee ) {
		info << "WARNING tentative d'envoyer un message sur une interface actuellement non connectée";
		Scheduler::trace(info.str());
		delete(message);
		return;
	}

	message->blocSource = interfaceConcernee->bloc;
	message->blocDestination = interfaceConcernee->interfaceConnectee->bloc;
	message->interfaceSource = interfaceConcernee;
	message->interfaceDestination = interfaceConcernee->interfaceConnectee;

	info << "EvenementBlocEnvoieMessage : début de l'envoi d'un message par le bloc " << message->blocSource->blocID;
	Scheduler::trace(info.str());

	interfaceConcernee->ajouteBufferEmission(message);

	return;
}

//===========================================================================================================
//
//          EvenementBlocRecoitMessage  (class)
//
//===========================================================================================================

EvenementBlocRecoitMessage::EvenementBlocRecoitMessage(uint64_t heure, Message *mess) : Evenement(heure) {
	message = mess;
	typeEvenement = TYPE_EV_BLOC_RECOIT_MESSAGE;
}

void EvenementBlocRecoitMessage::consomme() {
	stringstream info;
	info << "EvenementBlocRecoitMessage : fin de la réception d'un message par le bloc " << message->blocDestination->blocID;
	Scheduler::trace(info.str());

	message->blocDestination->scheduleEvenementLocal(this);
	return;
}

//===========================================================================================================
//
//          EvenementEchecDebutDeplaceBloc  (class)
//
//===========================================================================================================

EvenementEchecDebutDeplaceBloc::EvenementEchecDebutDeplaceBloc(uint64_t heure, Bloc* bloc, direction_E dir, uint64_t duree) : Evenement(heure) {
	blocConcerne = bloc;
	dureeDeplacement = duree;
	direction = dir;
	typeEvenement = TYPE_EV_ECHEC_DEBUT_DEPLACE_BLOC;
}

void EvenementEchecDebutDeplaceBloc::consomme() {
	blocConcerne->scheduleEvenementLocal(this);
	return;
}

//===========================================================================================================
//
//          EvenementDetecteNouveauVoisin  (class)
//
//===========================================================================================================

EvenementDetecteNouveauVoisin ::EvenementDetecteNouveauVoisin (uint64_t heure, Bloc* bloc, direction_E dir) : Evenement(heure) {
	blocConcerne = bloc;
	direction = dir;
	typeEvenement = TYPE_EV_DETECTE_NOUVEAU_VOISIN;
}

void EvenementDetecteNouveauVoisin ::consomme() {
	blocConcerne->scheduleEvenementLocal(this);
	return;
}

//===========================================================================================================
//
//          EvenementPerdAncienVoisin  (class)
//
//===========================================================================================================

EvenementPerdAncienVoisin ::EvenementPerdAncienVoisin (uint64_t heure, Bloc* bloc, direction_E dir) : Evenement(heure) {
	blocConcerne = bloc;
	direction = dir;
	typeEvenement = TYPE_EV_PERD_ANCIEN_VOISIN;
}

void EvenementPerdAncienVoisin ::consomme() {
	blocConcerne->scheduleEvenementLocal(this);
	return;
}

//===========================================================================================================
//
//          EvenementInterfaceReseauTermineEnvoi  (class)
//
//===========================================================================================================

EvenementInterfaceReseauTermineEnvoi::EvenementInterfaceReseauTermineEnvoi (uint64_t heure, InterfaceReseau *ir, Message *mess) : Evenement(heure) {
	interfaceConcernee = ir;
	typeEvenement = TYPE_EV_INTERFACE_RESEAU_TERMINE_ENVOI;
	message = mess;
}

void EvenementInterfaceReseauTermineEnvoi ::consomme() {
	if (message->echec) {
		Scheduler::trace("WARNING Un message détruit suite a une interruption de liaison");
		interfaceConcernee->messageEnCours = NULL;
		delete(message);
	} else {
		interfaceConcernee->messageEnCours->nbRef = 2;
		interfaceConcernee->bloc->scheduleEvenementLocal(new EvenementConfirmeReceptionMessage(Scheduler::getHeureActuelle(),interfaceConcernee->messageEnCours));
		interfaceConcernee->interfaceConnectee->bloc->scheduleEvenementLocal(new EvenementBlocRecoitMessage(Scheduler::getHeureActuelle(),interfaceConcernee->messageEnCours));

		interfaceConcernee->dateDisponibilite = Scheduler::getHeureActuelle();
		if (interfaceConcernee->fileAttenteMessagesEmission.size() > 0) {
			Scheduler::schedule(new EvenementInterfaceReseauDebuteEnvoi(interfaceConcernee->dateDisponibilite,interfaceConcernee));
		}
		interfaceConcernee->messageEnCours = NULL;
	}
	return;
}

//===========================================================================================================
//
//          EvenementInterfaceReseauDebuteEnvoi  (class)
//
//===========================================================================================================

EvenementInterfaceReseauDebuteEnvoi::EvenementInterfaceReseauDebuteEnvoi (uint64_t heure, InterfaceReseau *ir) : Evenement(heure) {
	interfaceConcernee = ir;
	typeEvenement = TYPE_EV_INTERFACE_RESEAU_DEBUTE_ENVOI;
}

void EvenementInterfaceReseauDebuteEnvoi ::consomme() {
	interfaceConcernee->envoie();
	return;
}

//===========================================================================================================
//
//          EvenementConfirmeReceptionMessage  (class)
//
//===========================================================================================================

EvenementConfirmeReceptionMessage ::EvenementConfirmeReceptionMessage (uint64_t heure, Message *mess) : Evenement(heure) {
	message = mess;
	typeEvenement = TYPE_EV_CONFIRME_RECEPTION_MESSAGE;
}

void EvenementConfirmeReceptionMessage ::consomme() {
	return;
}

//===========================================================================================================
//
//          EvenementBlocTimer  (class)
//
//===========================================================================================================

EvenementBlocTimer::EvenementBlocTimer(uint64_t heure, Bloc *bloc, unsigned int tID) : Evenement(heure) {
	blocConcerne = bloc;
	timerID = tID;
	typeEvenement = TYPE_EV_BLOC_TIMER;
}

void EvenementBlocTimer::consomme() {
	blocConcerne->scheduleEvenementLocal(this);
	return;
}

//===========================================================================================================
//
//          JetActivationEvent
//
//===========================================================================================================

JetActivationEvent::JetActivationEvent(uint64_t heure, ConveyorInterface *cv) : Evenement(heure) {
	conveyor = cv;
	typeEvenement = TYPE_EV_JET_ACTIVATION;
}

void JetActivationEvent::consomme() {
	if (heureEvenement>=conveyor->endingTime) {
			Physics::removeCoveyor(conveyor);
			cout << "remove :" << heureEvenement << " >=" << conveyor->endingTime << endl;
	} else {
		cout << "add" << endl;
		Physics::addConveyor(conveyor);
	}
	return;
}
