/*
 * Evenements.h
 *
 *  Created on: 29 janv. 2012
 *      Author: dom
 */

#ifndef EVENEMENTS_H_
#define EVENEMENTS_H_

#include <stdint.h>
#include <stdlib.h>
#include "openglViewer.h"
#include "Blocs.h"
#include "CodeBloc.h"
#include "Message.h"

#define TYPE_EV_GENERIQUE						1
#define TYPE_EV_DEBUT_DEPLACE_BLOC 				2
#define TYPE_EV_FIN_DEPLACE_BLOC 				3
#define TYPE_EV_DEMARRE_CODE_BLOC				4
#define TYPE_EV_CODEBLOC_TRAITE_EV_LOCAL		5
#define TYPE_EV_BLOC_RECOIT_MESSAGE				6
#define TYPE_EV_DEBUT_DEPLACE_GROUPE_BLOCS 		7
#define TYPE_EV_FIN_DEPLACE_GROUPE_BLOCS 		8
#define TYPE_EV_ECHEC_DEBUT_DEPLACE_BLOC		9
#define TYPE_EV_DETECTE_NOUVEAU_VOISIN			10
#define TYPE_EV_PERD_ANCIEN_VOISIN				11
#define TYPE_EV_BLOC_ENVOIE_MESSAGE				12
#define TYPE_EV_INTERFACE_RESEAU_TERMINE_ENVOI	13
#define TYPE_EV_INTERFACE_RESEAU_DEBUTE_ENVOI	14
#define TYPE_EV_CONFIRME_RECEPTION_MESSAGE		15
#define TYPE_EV_BLOC_TIMER						16
#define TYPE_EV_JET_ACTIVATION					17
//===========================================================================================================
//
//          Evenement  (class)
//
//===========================================================================================================

class Evenement {
public:
	static unsigned int nextID;

	unsigned int typeEvenement;
	uint64_t heureEvenement;
	unsigned int id;
	int nbRef;

	static unsigned int nbEvenements;

	static unsigned int nextIDGroupeBlocs;

	Evenement(uint64_t heure);
	virtual ~Evenement();

	virtual void consomme();
};

//===========================================================================================================
//
//          EvenementDebutDeplaceBloc  (class)
//
//===========================================================================================================

class EvenementDebutDeplaceBloc: public Evenement {
private:
	Bloc *blocConcerne;
	direction_E direction;
	uint64_t dureeDeplacement;
public:
	EvenementDebutDeplaceBloc(uint64_t heure, Bloc* bloc, direction_E dir, uint64_t duree);
	void consomme();
};

//===========================================================================================================
//
//          EvenementFinDeplaceBloc  (class)
//
//===========================================================================================================

class EvenementFinDeplaceBloc: public Evenement {
private:
	Bloc *blocConcerne;
public:
	direction_E direction;
	uint64_t dureeDeplacement;
	EvenementFinDeplaceBloc(uint64_t heure, Bloc* bloc, direction_E dir, uint64_t duree);
	void consomme();
};

//===========================================================================================================
//
//          EvenementDebutDeplaceGroupeBlocs  (class)
//
//===========================================================================================================

class EvenementDebutDeplaceGroupeBlocs: public Evenement {
private:
	vector<Bloc*> blocsConcernes;
	direction_E direction;
	uint64_t dureeDeplacement;
public:
	EvenementDebutDeplaceGroupeBlocs(uint64_t heure, vector<Bloc*> *blocs, direction_E dir, uint64_t duree);
	void consomme();
};

//===========================================================================================================
//
//          EvenementFinDeplaceGroupeBlocs  (class)
//
//===========================================================================================================

class EvenementFinDeplaceGroupeBlocs: public Evenement {
private:
	vector<Bloc*> blocsConcernes;

public:
	EvenementFinDeplaceGroupeBlocs(uint64_t heure, vector<Bloc*> *blocs);
	void consomme();
};

//===========================================================================================================
//
//          EvenementDemarreCodeBloc  (class)
//
//===========================================================================================================

class EvenementDemarreCodeBloc: public Evenement {
private:
	Bloc *blocConcerne;
	Message *message;
public:
	EvenementDemarreCodeBloc(uint64_t heure, Bloc* bloc,Message *msg);
	void consomme();
};

//===========================================================================================================
//
//          EvenementCodeBlocTraiteEvenementLocal  (class)
//
//===========================================================================================================

class EvenementCodeBlocTraiteEvenementLocal: public Evenement {
private:
	CodeBloc *codeBlocConcerne;
public:
	EvenementCodeBlocTraiteEvenementLocal(uint64_t heure, CodeBloc* codeBloc);
	void consomme();
};

//===========================================================================================================
//
//          EvenementBlocEnvoieMessage  (class)
//
//===========================================================================================================

class EvenementBlocEnvoieMessage: public Evenement {
public:
	Message *message;
	InterfaceReseau *interfaceConcernee;

	EvenementBlocEnvoieMessage(uint64_t heure, Message *mess, InterfaceReseau *ir);
	void consomme();
};

//===========================================================================================================
//
//          EvenementBlocRecoitMessage  (class)
//
//===========================================================================================================

class EvenementBlocRecoitMessage: public Evenement {
public:
	Message *message;

	EvenementBlocRecoitMessage(uint64_t heure, Message *mess);
	void consomme();
};

//===========================================================================================================
//
//          EvenementEchecDebutDeplaceBloc  (class)
//
//===========================================================================================================

class EvenementEchecDebutDeplaceBloc: public Evenement {
private:
	Bloc *blocConcerne;
	direction_E direction;
	uint64_t dureeDeplacement;
public:
	EvenementEchecDebutDeplaceBloc(uint64_t heure, Bloc* bloc, direction_E dir, uint64_t duree);
	void consomme();
};

//===========================================================================================================
//
//          EvenementDetecteNouveauVoisin  (class)
//
//===========================================================================================================

class EvenementDetecteNouveauVoisin: public Evenement {
private:
	Bloc *blocConcerne;
public:
	direction_E direction;

	EvenementDetecteNouveauVoisin(uint64_t heure, Bloc* bloc, direction_E dir);
	void consomme();
};

//===========================================================================================================
//
//          EvenementPerdAncienVoisin  (class)
//
//===========================================================================================================

class EvenementPerdAncienVoisin: public Evenement {
private:
	Bloc *blocConcerne;
public:
	direction_E direction;

	EvenementPerdAncienVoisin(uint64_t heure, Bloc* bloc, direction_E dir);
	void consomme();
};

//===========================================================================================================
//
//          EvenementInterfaceReseauTermineEnvoi  (class)
//
//===========================================================================================================

class EvenementInterfaceReseauTermineEnvoi: public Evenement {
private:
	InterfaceReseau *interfaceConcernee;
public:
	direction_E direction;
	Message *message;

	EvenementInterfaceReseauTermineEnvoi(uint64_t heure, InterfaceReseau *ir, Message *mess);
	void consomme();
};

//===========================================================================================================
//
//          EvenementInterfaceReseauDebuteEnvoi  (class)
//
//===========================================================================================================

class EvenementInterfaceReseauDebuteEnvoi: public Evenement {
private:
	InterfaceReseau *interfaceConcernee;
public:
	direction_E direction;

	EvenementInterfaceReseauDebuteEnvoi(uint64_t heure, InterfaceReseau *ir);
	void consomme();
};

//===========================================================================================================
//
//          EvenementConfirmeReceptionMessage  (class)
//
//===========================================================================================================

class EvenementConfirmeReceptionMessage: public Evenement {
public:
	Message *message;
	EvenementConfirmeReceptionMessage(uint64_t heure, Message *mess);
	void consomme();
};

//===========================================================================================================
//
//          EvenementBlocTimer  (class)
//
//===========================================================================================================

class EvenementBlocTimer: public Evenement {
private:
	Bloc *blocConcerne;
public:
	unsigned int timerID;

	EvenementBlocTimer(uint64_t heure, Bloc *bloc, unsigned int tID);
	void consomme();
};

//===========================================================================================================
//
//          JetActivationEvent  (class)
//
//===========================================================================================================

class JetActivationEvent: public Evenement {
private:
	ConveyorInterface *conveyor;
public:
	JetActivationEvent(uint64_t heure, ConveyorInterface *cv);
	void consomme();
};

#endif /* EVENEMENTS_H_ */
