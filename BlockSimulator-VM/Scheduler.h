/*
 * Scheduler.h
 *
 *  Created on: 29 janv. 2012
 *      Author: dom
 */

//==============================================================================================
//
// Classe principale pour le Scheduler
//
// Une seule instance du Scheduler peut exister. Pour le garantir, son instanciation se fait par
// la méthode statique initScheduler();
//
// Les méthodes de contrôle et de scheduling sont statiques elles aussi
//
//==============================================================================================

#ifndef SCHEDULER_H_
#define SCHEDULER_H_

#include <stdint.h>
#include <stdarg.h>
#include <list>
#include <map>
#include <string>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>
#include "Evenements.h"

using namespace std;

#define MODE_SCHEDULER_TEMPS_REEL 1
#define MODE_SCHEDULER_VITESSE_MAXIMALE 2

class Info
{ public :
  char *str;
  uint64_t heure;
  Bloc *ptrBloc;

  Info(char *_str,uint64_t _heure,Bloc *_ptr);
  ~Info() { delete [] str; };
};

class Scheduler {
private:
	static Scheduler *scheduler; // DD voir si encore nécessaire ?
	static uint64_t heureActuelle;
	static uint64_t heureMaximale; // condition d'arrêt

//	static list<Evenement*> listeEvenements;
	static multimap<uint64_t,Evenement*> listeEvenements;
	static bool simulationEnCours; 
	static pthread_mutex_t mutex;
	static int runMode;
	static sem_t *sem_schedulerStart;
	static pthread_t schedulerThread;

	Scheduler();
	~Scheduler();

public:
	static int tailleMax;  // stat : taille max atteinte
	static int tailleListe; // stat : taille courante
	static list<Info*> lstInfos;
	static int nbreInfos;
	static bool waitingForStartOrder;

	static bool initScheduler();
	static bool schedule(Evenement *evenement);

	static uint64_t getHeureActuelle();
	static void afficheListeEvenements();
	static void run(int runMode);
	static void *startPaused(void* p);

	static void trace(string message);
	static void printf(Bloc *ptr,const char *pszFormat, ...);

	static void lock();
	static void unlock();
};

#endif /* SCHEDULER_H_ */
