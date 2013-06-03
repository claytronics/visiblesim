/*
 * Scheduler.cpp
 *
 *  Created on: 29 janv. 2012
 *      Author: dom
 */

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <iomanip>

#ifndef WIN32
#include <unistd.h>
#endif

#include "Scheduler.h"
#include "physics.h"
#include "fcntl.h"

using namespace std;

Scheduler *Scheduler::scheduler = NULL;
uint64_t Scheduler::heureActuelle;
uint64_t Scheduler::heureMaximale;
//list<Evenement*> Scheduler::listeEvenements;
multimap<uint64_t,Evenement*> Scheduler::listeEvenements;
bool Scheduler::simulationEnCours = false;

int Scheduler::tailleMax = 0;
int Scheduler::tailleListe = 0;
int Scheduler::nbreInfos = 0;
bool Scheduler::waitingForStartOrder = true;
list<Info*> Scheduler::lstInfos;
pthread_mutex_t Scheduler::mutex = PTHREAD_MUTEX_INITIALIZER;
int Scheduler::runMode = MODE_SCHEDULER_TEMPS_REEL;
sem_t *Scheduler::sem_schedulerStart;
pthread_t Scheduler::schedulerThread;

Scheduler::Scheduler() {

	if (Scheduler::scheduler == NULL) {
		puts("Création du Scheduler");
		scheduler = this;
	} else {
		puts("ERREUR : Un seul Scheduler peut être créé par instance du programme");
		exit(EXIT_FAILURE);
	}

}

Scheduler::~Scheduler() {
	puts("Destruction du Scheduler");
	list<Info*>::const_iterator ci=lstInfos.begin();
	while (!lstInfos.empty())
	{ Info *ptr = lstInfos.front();
	  delete ptr;
	  lstInfos.pop_front();
	}
}

bool Scheduler::initScheduler() {
	if (sizeof(uint64_t) != 8) {
		puts("ERREUR : En interne le Scheduler a besoin d'entiers sur 64bits qui ne semblent pas disponibles sur ce système");
		exit(EXIT_FAILURE);
	}

	Scheduler::scheduler = new Scheduler();
	Scheduler::heureActuelle = 0;
	Scheduler::heureMaximale = 600000000;//3600000000;

	pthread_create(&schedulerThread,NULL,Scheduler::startPaused,NULL);
	return(true);
}

bool Scheduler::schedule(Evenement *ev) {

  list<Evenement*>::reverse_iterator it;

  //lock();
  if (ev->heureEvenement < Scheduler::heureActuelle) {
    cout << "ERREUR : tentative de programmation d'un événement dans le passé\n";
    cout << "heureActuelle : " << Scheduler::getHeureActuelle() << endl;
    cout << "ev->heureEvenement : " << ev->heureEvenement << endl;
    cout << "ev->typeEvenement : " << ev->typeEvenement << endl;
    return(false);
  }

  if (ev->heureEvenement > heureMaximale) {
	cout << "WARNING : tentative de programmation d'un événement au delà de l'heure de fin\n";
    return(false);
  }

  /*
  if (listeEvenements.empty()) {
    listeEvenements.push_front(ev);
    return(true);
  }

  it = listeEvenements.rbegin();
  while (it != listeEvenements.rend() && ev->heureEvenement < (*it)->heureEvenement) {
    it++;
  }
*/
  	 if (ev->typeEvenement > 17) {
  		 cout << "ERREUR schedule" << endl;
  		 exit(EXIT_FAILURE);
  	 }

  listeEvenements.insert(pair<uint64_t, Evenement*>(ev->heureEvenement,ev));
  //listeEvenements.insert(it.base(),ev);
  tailleListe++;
  ev->nbRef++;


  //if (tailleMax < (int)listeEvenements.size()) tailleMax = (int)listeEvenements.size();
  if (tailleMax < tailleListe) tailleMax = tailleListe;
 // cout << heureActuelle << "  " << listeEvenements.size() << endl;

  //unlock();
  return(true);
}


uint64_t Scheduler::getHeureActuelle() {
	return(heureActuelle);
}

void Scheduler::afficheListeEvenements() {
  //list<Evenement*>::iterator it;
	multimap<uint64_t, Evenement*>::iterator it;

	cout << "--- début de la liste des évenements ---\n";
	for (it=listeEvenements.begin(); it!=listeEvenements.end(); it++) {
		cout << setw(8) << (*it).first << " : Evenement " << (*it).second->id << " type:" << (*it).second->typeEvenement <<"\n";
	}
	cout << "--- fin de la liste des événements ---\n";
}

void Scheduler::run(int pRunMode) {
	cout << "run !!!!" << endl;
	runMode = pRunMode;
	sem_post(sem_schedulerStart);
}

void *Scheduler::startPaused(void *p) {
	Evenement *ev;

	uint64_t heureSystemeDebut;
	uint64_t heureSystemeFin;

	uint64_t heureSystemeActuelle;

	uint64_t heureActuelleMax;
	multimap<uint64_t, Evenement*>::iterator premier;

	sem_schedulerStart = sem_open("/sem_schedulerStart", O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO , 0);
	if (sem_schedulerStart == SEM_FAILED) {
		cout << "ERREUR SEMAPHORE" << endl;
	}
	sem_close(sem_schedulerStart);
	cout << "unlink :" << sem_unlink("/sem_schedulerStart")<< endl;
	sem_schedulerStart = sem_open("/sem_schedulerStart", O_CREAT, S_IRWXU | S_IRWXG | S_IRWXO , 0);
	if (sem_schedulerStart == SEM_FAILED) {
		cout << "ERREUR SEMAPHORE" << endl;
	}

	int i;
	sem_getvalue(sem_schedulerStart,&i);
	cout << "sem_get_value = '" << i << "'" << endl;

	sem_wait(sem_schedulerStart);
	cout << "sem_wait" << endl;
/*
	while (waitingForStartOrder) {

	}
*/
	if (simulationEnCours) {
		return NULL;
	}
	simulationEnCours = true;

	heureActuelle = 0;
	heureActuelleMax =0;

	heureSystemeDebut = ((uint64_t)glutGet(GLUT_ELAPSED_TIME))*1000;
	cout << "debut simu : " << heureSystemeDebut << endl;
	Physics::initPhysics();
	bool doitArreter=false;

	switch (runMode) {
	case MODE_SCHEDULER_VITESSE_MAXIMALE:
	    while (!listeEvenements.empty() && heureActuelle < heureMaximale) {
	    	premier=listeEvenements.begin();
	      //ev = *(listeEvenements.begin());
	    	ev = (*premier).second;
	    	heureActuelle = ev->heureEvenement;
	    	lock();
	    	ev->consomme();
	    	unlock();
	    	ev->nbRef--;
	    	if (ev->nbRef <= 0) {
	    		delete(ev);
	    	}
	    	//listeEvenements.pop_front();
	    	listeEvenements.erase(premier);
	    	tailleListe--;
	    }

		break;
	case MODE_SCHEDULER_TEMPS_REEL:
		cout << "mode temps réel\n";
	    while(!doitArreter && listeEvenements.size() > 0) {
	      //gettimeofday(&heureGlobaleActuelle,NULL);
	      heureSystemeActuelle = ((uint64_t)glutGet(GLUT_ELAPSED_TIME))*1000;
	      heureActuelleMax = heureSystemeActuelle - heureSystemeDebut;
	      //ev = *(listeEvenements.begin());
	      premier=listeEvenements.begin();
	      ev = (*premier).second;
	      while (!listeEvenements.empty() && ev->heureEvenement <= heureActuelleMax) {
			  /* traitement du mouvement des objets physiques*/
			  Physics::update(ev->heureEvenement);
	    	  heureActuelle = ev->heureEvenement;
	    	  lock();
	    	  ev->consomme();
	    	  unlock();
	    	  ev->nbRef--;
		      if (ev->nbRef <= 0) {
		    	  delete(ev);
		      }
	    	  //listeEvenements.pop_front();
		      listeEvenements.erase(premier);
	    	  tailleListe--;
//	    	  ev = *(listeEvenements.begin());
		      premier=listeEvenements.begin();
		      ev = (*premier).second;
	      }
	      heureActuelle = heureActuelleMax;
	      if (listeEvenements.size() > 0) {
	        //ev = *(listeEvenements.begin());
	        premier=listeEvenements.begin();
	        ev = (*premier).second;

	        /*
	        dureeAttente = ev->heureEvenement - heureActuelle;
	        dureeAttenteTimeval.tv_sec = floor(dureeAttente / 1000000);
	        dureeAttenteTimeval.tv_usec = (dureeAttente%1000000);
	        select(0,NULL,NULL,NULL,&dureeAttenteTimeval);
	        */
#ifdef WIN32
			Sleep(5);
#else
	        usleep(5000);
#endif
	      }
	    }
	    break;
	default:
		puts("ERREUR : mode de fonctionnement inconnu pour le Scheduler");
	}

	heureSystemeFin = ((uint64_t)glutGet(GLUT_ELAPSED_TIME))*1000;
	cout << "fin : " << heureSystemeFin << endl;

	cout << "Heure de fin : " << heureActuelle << endl;
	cout << "Temps réel écoulé : " << ((double)(heureSystemeFin-heureSystemeDebut))/1000000 << endl;
	cout << "Nombre d'événements restants en mémoire : " << Evenement::nbEvenements << endl;
	cout << "Nombre de messages restants en mémoire : " << Message::nbMessages << endl;
	cout << "Taille maximale atteinte par la liste des événements : " << Scheduler::tailleMax << endl;
	cout << "Nombre d'événements traités : " << Evenement::nextID << endl;

	return NULL;
}

void Scheduler::trace(string message) {
	/*cout.precision(6);
	cout << fixed << (double)(Scheduler::getHeureActuelle())/1000000 << " " << message << endl;*/
	cout << message << endl;
}

void Scheduler::printf(Bloc*ptr,const char *pszFormat, ...) {
	static char buffer[2000];
	   va_list argList;
	   va_start(argList, pszFormat);
#ifdef WIN32
	   ::vsnprintf_s(buffer, sizeof(buffer)-1, pszFormat, argList);
#else
	   ::vsnprintf(buffer, sizeof(buffer)-1, pszFormat, argList);
#endif
	   buffer[sizeof(buffer)-1] = '\0';

	   lstInfos.push_front(new Info(buffer,getHeureActuelle(),ptr));
	   nbreInfos++;
	   cout << lstInfos.front()->str<< endl;

}

Info::Info(char *_str,uint64_t _heure,Bloc *_ptr)
{ int n = strlen(_str)+1;

  str = new char[n];
  memcpy(str,_str,n);
  ptrBloc = _ptr;
  heure = _heure;
};

void Scheduler::lock() {
	pthread_mutex_lock(&mutex);
}

void Scheduler::unlock() {
	pthread_mutex_unlock(&mutex);
}
