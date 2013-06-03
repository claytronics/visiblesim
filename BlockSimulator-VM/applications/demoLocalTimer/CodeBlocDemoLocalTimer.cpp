/*
 * CodeBlocDemoLocalTimer.cpp
 *
 *  Created on: 29 févr. 2012
 *      Author: dom
 */

#include <iostream>
#include <sstream>
#include "CodeBlocDemoLocalTimer.h"
#include "Scheduler.h"

using namespace std;

CodeBlocDemoLocalTimer::CodeBlocDemoLocalTimer() : CodeBloc() {
#if DEBUG_LEVEL > 3
	cout << "Constructeur CodeBlocDemoLocalTimer\n";
#endif
}

void CodeBlocDemoLocalTimer::demarre() {
	if (bloc->blocID % 2 == 0) {
		bloc->setColor(1,0,0);
		rouge = true;
	} else {
		bloc->setColor(0,0,1);
		rouge = false;
	}
	Scheduler::schedule(new EvenementBlocTimer(Scheduler::getHeureActuelle()+1000000,bloc,1));
}

void CodeBlocDemoLocalTimer::traiteEvenementLocal(Evenement *ev) {
	if (ev->typeEvenement == TYPE_EV_BLOC_TIMER) {
		if (rouge) {
			bloc->setColor(0,0,1);
			rouge = false;
		} else {
			bloc->setColor(1,0,0);
			rouge = true;
		}
		Scheduler::schedule(new EvenementBlocTimer(Scheduler::getHeureActuelle()+1000000,bloc,1));
	}

}
