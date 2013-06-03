/*
 * Blocs.h
 *
 *  Created on: 29 janv. 2012
 *      Author: dom
 */

#ifndef BLOCS_H_
#define BLOCS_H_

#include <vector>
#include <list>
#include <deque>
#include "Matrice44.h"
#include "glBlock.h"
#include "CodeBloc.h"

class Message;
class Evenement;

using namespace std;

enum direction_E {IMMOBILE, NORD, SUD, EST, OUEST, P2P};

class Bloc;
class Cellule;

class Position {
public:
	uint64_t heure;
	double posX, posY;
};

//===========================================================================================================
//
//          InterfaceReseau  (class)
//
//===========================================================================================================

class InterfaceReseau {
public:
	static int debitInterfaceReseau;

	deque<Message*> fileAttenteMessagesEmission;
	InterfaceReseau *interfaceConnectee;
	Bloc *bloc;
	Message *messageEnCours;
	uint64_t dateDisponibilite;
	direction_E direction;

	InterfaceReseau(Bloc *b, direction_E dir);
	bool ajouteBufferEmission(Message *msg);
	void envoie();
	void connecte(InterfaceReseau *ir);
	void deconnecte();
};

class ConveyorInterface {
public:
	uint64_t beginningTime,endingTime;
	Bloc *bloc;

	ConveyorInterface(Bloc *b);
	void activate(uint64_t begin,uint64_t end);
	Vecteur getForce(const Vecteur &pos);
};

//===========================================================================================================
//
//          Bloc  (class)
//
//===========================================================================================================

class Bloc {
private:
	static unsigned int nextBlocID;
	static vector<Bloc*> blocVector;
	GLuint currentIdTexture;
	GlBlock glBlock;
public:
	unsigned int blocID;
	direction_E etatBloc;
	Position positionDepart, positionArrivee;
	list<Evenement*> listeEvenementsLocaux;
	unsigned int idGroupeBlocs;
	CodeBloc *codeBloc;

	Cellule *CelluleCible;
	Cellule *CelluleSource;

	InterfaceReseau *interfaceReseauNord;
	InterfaceReseau *interfaceReseauSud;
	InterfaceReseau *interfaceReseauEst;
	InterfaceReseau *interfaceReseauOuest;
	// list of P2P network interfaces to connect non-contiguous blocks
	list<InterfaceReseau *> listP2PNetworkInterfaces;

	ConveyorInterface *conveyor;

	Bloc* voisinNord, *voisinSud, *voisinEst, *voisinOuest;

	Bloc(float x, float y, CodeBloc *code);
	~Bloc();

	static Bloc *getBlocByID(unsigned int blocID) {
		vector<Bloc*>::const_iterator it = blocVector.begin();
		while (it != blocVector.end() && (*it)->blocID != blocID) {
			it++;
		}
		return (it==blocVector.end())?NULL:(*it);
	}

	void glDraw();
	void glDrawId();

	void get2DPosition(GLfloat &x,GLfloat &y);
	void getTopPosition(GLfloat &x,GLfloat &y,GLfloat &z) { glBlock.getTopPosition(x,y,z); };
	inline void getLastPosition(GLfloat &x,GLfloat &y)
	{ glBlock.getGridPosition(x,y);
	};
	void scheduleEvenementLocal(Evenement *ev);
	inline void toggleHighlight() { glBlock.toggleHighlight(); };
	void setColor(GLfloat r, GLfloat g, GLfloat b);
	void activeJet(int dt,int delay=0);
	void setSize(double sx,double sy,double sz) { glBlock.setSize(GLfloat(sx),GLfloat(sy),GLfloat(sz)); };
	bool hasMobile(int &id, float &px, float &py, float &vx, float &vy);
	// Management functions of the P2P network interface list
	bool addP2PNetworkInterface(Bloc *destBlock);
	bool addP2PNetworkInterface(unsigned int destBlockID);
	bool removeP2PNetworkInterface(Bloc *destBlock);
	InterfaceReseau *getP2PNetworkInterface(Bloc *destBlock);
	InterfaceReseau *getP2PNetworkInterface(unsigned int destBlockID);
};

//===========================================================================================================
//
//          Cellule  (class)
//
//===========================================================================================================

class Cellule {
public:
	Bloc *blocContenu;
	Bloc *blocQuiVient;
	Bloc *blocQuiPart;

	Cellule();
};


#endif /* BLOCS_H_ */
