/*
 * world.cpp
 *
 *  Created on: 16 févr. 2013
 *      Author: dom
 */

#include <iostream>
#include "world.h"

using namespace std;

World::World() {
	cout << "\033[36;1m" << "World constructor" << "\033[0m" << endl;
	blockVect.clear();
}

World::~World() {
	cout << "\033[36;1m" << "World destructor" << "\033[0m" << endl;
}
