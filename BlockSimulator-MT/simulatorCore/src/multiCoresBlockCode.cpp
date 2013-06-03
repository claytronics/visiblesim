/*
 * multiCoresBlockCode.cpp
 *
 *  Created on: 15 févr. 2013
 *      Author: dom
 */

#include <iostream>
#include "multiCoresBlockCode.h"
#include "multiCoresSimulator.h"

using namespace std;

MultiCoresSimulator *MultiCoresBlockCode::simulator = NULL;

MultiCoresBlockCode::MultiCoresBlockCode(MultiCoresBlock *host):BlockCode(host) {
	cout << "MultiCoresBlockCode constructor" << endl;
	simulator = (MultiCoresSimulator*)Simulator::getInstance();
}

MultiCoresBlockCode::~MultiCoresBlockCode() {
	cout << "MultiCoresBlockCode destructor" << endl;
}
