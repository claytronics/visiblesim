/*
 * Blocs.cpp
 *
 *  Created on: 29 janv. 2012
 *      Author: dom
 */

#include <sstream>
#include <iostream>
#include "Blocs.h"
#include "Scheduler.h"
#include "CodeBloc.h"
#include "physics.h"

using namespace std;

int InterfaceReseau::debitInterfaceReseau = 100000;
unsigned int Bloc::nextBlocID=0;

//===========================================================================================================
//
//          InterfaceReseau  (class)
//
//===========================================================================================================

InterfaceReseau::InterfaceReseau(Bloc *b, direction_E dir) {
#if DEBUG_LEVEL > 3
	cout << "Constructeur InterfaceReseau" << endl;
#endif
	dateDisponibilite = 0;
	bloc = b;
	interfaceConnectee = NULL;
	messageEnCours = NULL;
	direction = dir;
}


bool InterfaceReseau::ajouteBufferEmission(Message *msg) {
	stringstream info;

	if (interfaceConnectee != NULL) {
		fileAttenteMessagesEmission.push_back(msg);
		if (dateDisponibilite < Scheduler::getHeureActuelle()) dateDisponibilite = Scheduler::getHeureActuelle();
		if (fileAttenteMessagesEmission.size() == 1 && messageEnCours == NULL) {
			Scheduler::schedule(new EvenementInterfaceReseauDebuteEnvoi(dateDisponibilite,this));
		}
		return(true);
	} else {
		info.str("");
		info << "Aucune interface connectée, échec de la mise en attente dans le buffer d'émission";
		info << " ( bloc " << bloc->blocID << ")";
		Scheduler::trace(info.str());
		return(false);
	}
}

void InterfaceReseau::envoie() {
	Message *msg;
	uint64_t dureeTransmission=22;

	if (!interfaceConnectee) {
		cout << "Warning : cette interface n'est plus connectée (envoie() )" << endl;
		return;
	}

	if (fileAttenteMessagesEmission.size()==0) {
		cout << "ERREUR : le buffer d'émission ne devrait pas être vide !" << endl;
		exit(EXIT_FAILURE);
	}
	msg = fileAttenteMessagesEmission.front();
	fileAttenteMessagesEmission.pop_front();

	messageEnCours = msg;
	dateDisponibilite = Scheduler::getHeureActuelle()+dureeTransmission;
	Scheduler::schedule(new EvenementInterfaceReseauTermineEnvoi(Scheduler::getHeureActuelle()+dureeTransmission,this,msg));
}

void InterfaceReseau::connecte(InterfaceReseau *ir) {
	stringstream info;
#if DEBUG_LEVEL > 3
	info << "connection d'une interface " << " ( bloc " << bloc->blocID;

	switch (direction) {
	case NORD:
		info << " Nord";
		break;
	case SUD:
		info << " Sud";
		break;
	case EST:
		info << " Est";
		break;
	case OUEST:
		info << " Ouest";
		break;
	default:
		break;
	}
	info << " à bloc ";
	info << interface->bloc->blocID;
	switch(interface->direction) {
	case NORD:
		info << " Nord";
		break;
	case SUD:
		info << " Sud";
		break;
	case EST:
		info << " Est";
		break;
	case OUEST:
		info << " Ouest";
		break;
	default:
		break;
	}
	info << ")";
	Scheduler::trace(info.str());
#endif
	interfaceConnectee = ir;
	ir->interfaceConnectee = this;
}

void InterfaceReseau::deconnecte() {
#if DEBUG_LEVEL > 3
	stringstream info;
	info << "déconnection d'une interface " << " ( bloc " << bloc->blocID << ")";
	Scheduler::trace(info.str());
#endif

	if (interfaceConnectee) {
		interfaceConnectee->interfaceConnectee = NULL;
		interfaceConnectee->fileAttenteMessagesEmission.clear();
		if (interfaceConnectee->messageEnCours) interfaceConnectee->messageEnCours->echec = true;
		interfaceConnectee = NULL;
	}
	if (messageEnCours) {
		messageEnCours->echec = true;
	}
	fileAttenteMessagesEmission.clear();
	dateDisponibilite = Scheduler::getHeureActuelle();
}
//===========================================================================================================
//
//          Bloc  (class)
//
//===========================================================================================================

vector<Bloc*> Bloc::blocVector;

Bloc::Bloc(float x, float y, CodeBloc *code) {
	blocID = nextBlocID++;
#if DEBUG_LEVEL > 3
	cout << "Constructeur Bloc (" << blocID << ",[" << x << "," << y << "])\n";
#endif
	glBlock.setGridPosition(x,y);
	etatBloc = IMMOBILE;

	positionDepart.posX = x;
	positionDepart.posY = y;

	codeBloc = code;
	code->bloc = this;

	voisinNord = NULL;
	voisinSud = NULL;
	voisinEst = NULL;
	voisinOuest = NULL;

	idGroupeBlocs = 0;

	interfaceReseauNord = new InterfaceReseau(this,NORD);
	interfaceReseauSud = new InterfaceReseau(this,SUD);
	interfaceReseauEst = new InterfaceReseau(this,EST);
	interfaceReseauOuest = new InterfaceReseau(this,OUEST);

	conveyor = NULL;

	blocVector.push_back(this);
}

Bloc::~Bloc()
{ delete conveyor;
}
/*
Bloc* Bloc::getBlocByID(unsigned int blocID) {
	vector<Bloc*>::const_iterator it = blocVector.begin();
	while (it != blocVector.end() && (*it)->blocID != blocID) it++;
	return (it==blocVector.end())?NULL:(*it);
}
*/

void Bloc::glDraw() {
	GLfloat x,y,z,px,py,pz;
	get2DPosition(x,y);
    //glBlock.setColor(1.0*(int(x)%11)/10.0,0.5,0.5+1.0*(int(y)%11)/20.0);
    glBlock.setGridPosition(x,y);
	glBlock.getTopPosition(x,y,z);
    glBlock.glDraw();
    if (GlutContext::showLinks)
    {	glDisable(GL_LIGHTING);
	    glDisable(GL_TEXTURE_2D);
		GLfloat sx,sy,sz;
		glBlock.getSize(sx,sy,sz);
// Draw quads between connected blocks
		if (interfaceReseauNord->interfaceConnectee) {
			glColor3f(0.0,1.0,0.0);
			glPushMatrix();
			glTranslatef(x,y+sy/2.0,z+0.1);
			glScalef(sx/6,sy/6,1.0);
			glBegin(GL_TRIANGLES);
			glVertex3f(0,-1,0);
			glVertex3f(1,-1,0);
			glVertex3f(1,1,0);
			glEnd();
			glPopMatrix();
			
		}
		if (interfaceReseauEst->interfaceConnectee) {
			glColor3f(1.0,0.0,0.0);
			glPushMatrix();
			glTranslatef(x+sx/2,y,z+0.1);
			glScalef(sx/6,sy/6,1.0);
			glBegin(GL_TRIANGLES);
			glVertex3f(-1,0,0);
			glVertex3f(-1,-1,0);
			glVertex3f(1,-1,0);
			glEnd();
			glPopMatrix();
		}
		if (interfaceReseauSud->interfaceConnectee) {
			glColor3f(0.0f,1.0f,1.0f);
			glPushMatrix();
			glTranslatef(x,y-sy/2.0,z+0.1);
			glScalef(sx/6,sy/6,1.0);
			glBegin(GL_TRIANGLES);
			glVertex3f(0,1,0);
			glVertex3f(-1,1,0);
			glVertex3f(-1,-1,0);
			glEnd();
			glPopMatrix();
		}
		if (interfaceReseauOuest->interfaceConnectee) {
			glColor3f(1.0f,0.0f,1.0f);
			glPushMatrix();
			glTranslatef(x-sx/2,y,z+0.1);
			glScalef(sx/6,sy/6,1.0);
			glBegin(GL_TRIANGLES);
			glVertex3f(1,0,0);
			glVertex3f(1,1,0);
			glVertex3f(-1,1,0);
			glEnd();
			glPopMatrix();
		}

		// draw P2P interface connection
		list <InterfaceReseau *>::const_iterator cir = listP2PNetworkInterfaces.begin();
		Bloc *dest;
		while (cir!=listP2PNetworkInterfaces.end()) {
			glColor3f(1.0,0.0,0.0);
			glPushMatrix();
			glBegin(GL_LINES);
			glVertex3f(x,y,z);
			dest = (*cir)->interfaceConnectee->bloc;
			glVertex3fv(dest->glBlock.pos);
			glEnd();
			glPopMatrix();
			//cout << "["<<blocID<<"](" << x << "," << y << "," << z << ")  " << "["<<dest->blocID<<"]-(" << dest->glBlock.pos[0] << "," << dest->glBlock.pos[1] << "," << dest->glBlock.pos[2] << ")" << endl;
			cir++;
		}

		glEnable(GL_LIGHTING);
	    
    }
}

void Bloc::glDrawId() {
	GLfloat x,y;
	get2DPosition(x,y);
    glBlock.setGridPosition(x,y);

    glBlock.glDrawId();
}

void Bloc::get2DPosition(GLfloat &rx,GLfloat &ry) {
	double x,y;
//  x = matricePlacement[3];
//  y = matricePlacement[7];
	if (etatBloc == IMMOBILE ||
		Scheduler::getHeureActuelle() < positionDepart.heure) {
		x = positionDepart.posX;
		y = positionDepart.posY;
	} else {
		if (Scheduler::getHeureActuelle() > positionArrivee.heure) {
			x = positionArrivee.posX;
			y = positionArrivee.posY;
		} else {
			double heureDepart = (double)positionDepart.heure;
			double heureArrivee = (double)positionArrivee.heure;
			double heureActuelle = (double)Scheduler::getHeureActuelle();
			double rapportTemporel;
			rapportTemporel = (heureActuelle-heureDepart)/ (heureArrivee - heureDepart);

			x = positionDepart.posX + (positionArrivee.posX - positionDepart.posX) * rapportTemporel;
			y = positionDepart.posY + (positionArrivee.posY - positionDepart.posY) * rapportTemporel;
		}
	}
	rx = GLfloat(x);
	ry = GLfloat(y);
}


/*void Bloc::magiqueDeplaceXY(double x, double y) {
	matricePlacement.setTranslation(x,y,0.0);
	matricePlacement.transpose(matricePlacementTransposee);
}
*/
void Bloc::scheduleEvenementLocal(Evenement *ev) {
	ev->nbRef++;
	listeEvenementsLocaux.push_back(ev);

	if (listeEvenementsLocaux.size() == 1) {
		uint64_t date;
		date = codeBloc->dateDisponibilite;
		if (date < Scheduler::getHeureActuelle()) date=Scheduler::getHeureActuelle();
		Scheduler::schedule(new EvenementCodeBlocTraiteEvenementLocal(date,codeBloc));

		/*
		if (codeBloc->dateDisponibilite <= Scheduler::getHeureActuelle()) {
			// ATTENTION Ici je ne suis pas sûr que ce soit une bonne idée d'appeler directement traiteEvenementLocal
			// C'est meilleur pour les performances, mais cela permet in fine de réaliser une action dans un
			// bloc voisin PENDANT un traitement dans un autre bloc !
			codeBloc->traiteEvenementLocal();
		} else {
			Scheduler::schedule(new EvenementCodeBlocTraiteEvenementLocal(codeBloc->dateDisponibilite,codeBloc));
		}
		*/
	}
	return;
}

void Bloc::setColor(GLfloat r, GLfloat g, GLfloat b) {
	glBlock.setColor(r,g,b);
}

void Bloc::activeJet(int dt,int delay)
{ if (!conveyor) {
	conveyor = new ConveyorInterface(this);
  } 
  uint64_t debut = Scheduler::getHeureActuelle()+delay;
  conveyor->activate(debut,debut+dt);
  Scheduler::schedule(new JetActivationEvent(debut,conveyor));
  Scheduler::schedule(new JetActivationEvent(debut+dt,conveyor));
}

bool Bloc::hasMobile(int &id, float &px, float &py, float &vx, float &vy)
{ GLfloat x0,y0,z0,x1,y1,z1;
  glBlock.getBoundingBox(x0,y0,z0,x1,y1,z1);

  Mobile *mob=Physics::getMobileInBox(x0,y0,z1-5,x1,y1,z1+25);
  // cout << x0 << "," << y0 << "," << z1-5 << "," << x1 << "," << y1 << "," << z1+25 << endl;
  if (mob)
  { id = mob->id;
    px = mob->position[0];
    py = mob->position[1];
    vx = mob->velocity[0];
    vy = mob->velocity[1];
	//cout << "OK : " << px << "," << py << endl;
	return true;
  }
  return false;
}

// Add an interface into the list of the block if it is not already connected by another interface.
// Return true if ok, false if another interface with this block is found.
bool Bloc::addP2PNetworkInterface(Bloc *destBlock) {
	InterfaceReseau *ir1, *ir2;
	ir1 = NULL;
	ir2 = NULL;

	if (!getP2PNetworkInterface(destBlock)) {
		// creation of the new network interface
		cout << "adding a new interface to block " << destBlock->blocID << endl;
		ir1 = new InterfaceReseau(this,P2P);
		listP2PNetworkInterfaces.push_back(ir1);
	}

	if (!destBlock->getP2PNetworkInterface(this)) {
		// creation of the new network interface
		cout << "adding a new interface to block " << this->blocID << endl;
		ir2 = new InterfaceReseau(destBlock,P2P);
		destBlock->listP2PNetworkInterfaces.push_back(ir2);
	}

	if (ir1!=NULL && ir2!=NULL) {
		ir1->interfaceConnectee = ir2;
		ir2->interfaceConnectee = ir1;
	}


	/*
	// if the link is not in the list
	if (!getP2PNetworkInterface(destBlock)) {
		// creation of the new network interface
		cout << "adding a new interface to block " << destBlock->blocID << endl;
		InterfaceReseau* ir1 = new InterfaceReseau(this,P2P);
		listP2PNetworkInterfaces.push_back(ir1);
		// if the corresponding interface exists in the connected block, we link the two interfaces
		if (destBlock->addP2PNetworkInterface(this)) {
			InterfaceReseau* ir2 = destBlock->getP2PNetworkInterface(this);
			ir1->interfaceConnectee = ir2;
			ir2->interfaceConnectee = ir1;
		}
	}*/
	return false;
}
bool Bloc::addP2PNetworkInterface(unsigned int destBlockID) {
	// if the link is not in the list
	Bloc *destBlock = Bloc::getBlocByID(destBlockID);
	if (!getP2PNetworkInterface(destBlock)) {
		// creation of the new network interface
		InterfaceReseau* ir1 = new InterfaceReseau(this,P2P);
		listP2PNetworkInterfaces.push_back(ir1);
		// if the corresponding interface exists in the connected block, we link the two interfaces
		if (destBlock->addP2PNetworkInterface(this)) {
			InterfaceReseau* ir2 = destBlock->getP2PNetworkInterface(this);
			ir1->interfaceConnectee = ir2;
			ir2->interfaceConnectee = ir1;
		}
	}
	return false;
}

InterfaceReseau*Bloc::getP2PNetworkInterface(Bloc *destBlock) {
	list <InterfaceReseau*>::const_iterator iri=listP2PNetworkInterfaces.begin();
	while (iri!=listP2PNetworkInterfaces.end() && (*iri)->interfaceConnectee->bloc!=destBlock) iri++;
	return (iri==listP2PNetworkInterfaces.end())?NULL:(*iri);
}
InterfaceReseau*Bloc::getP2PNetworkInterface(unsigned int destBlockID) {
	list <InterfaceReseau*>::const_iterator iri=listP2PNetworkInterfaces.begin();
	while (iri!=listP2PNetworkInterfaces.end() && (*iri)->interfaceConnectee->bloc->blocID != destBlockID) iri++;
	return (iri==listP2PNetworkInterfaces.end())?NULL:(*iri);
}

//===========================================================================================================
//
//          Cellule  (class)
//
//===========================================================================================================

Cellule::Cellule() {
	blocContenu = NULL;
	blocQuiVient = NULL;
	blocQuiPart = NULL;
}

//===========================================================================================================
//
//          ConveyorInterface
//
//===========================================================================================================

ConveyorInterface::ConveyorInterface(Bloc *b)
{ bloc = b;
}

void ConveyorInterface::activate(uint64_t begin,uint64_t end)
{ beginningTime = begin;
  endingTime = end;
}

Vecteur ConveyorInterface::getForce(const Vecteur &pos) {
	float x,y,z;
	bloc->getTopPosition(x,y,z);
  
    Vecteur v(x-pos.pt[0],y-pos.pt[1],0);
	return (1000.0/v.norme2())*v;
}

