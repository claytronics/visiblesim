/*
 * blinkyThreadsManager.cpp
 *
 *  Created on: 27 mars 2013
 *      Author: dom
 */


#include <iostream>
#include <list>
#include <boost/thread.hpp>
#include <boost/asio.hpp>
#include "VMEmulator.h"

using namespace std;
using boost::asio::ip::tcp;
using boost::shared_ptr;

list<boost::thread*> threadsList;

void threadVMConnection (boost::shared_ptr<tcp::socket> socket2) {	
	cout << "called!" << endl;
	VMMessage_t mes;
	tcp::socket *s = (tcp::socket*) socket2.get();
	cout << "One connection start" << endl;
	while (true) {
		try {
		boost::asio::read(*s,boost::asio::buffer((void*)&mes, sizeof(VMMessage_t)) );
		cout << mes.messageType << "," << mes.param1 << endl;
		} catch (std::exception& e) {
			cout << "ERROR connection closed ?" << endl;
			break;
		}
	}
}

int main(int argc, char **argv) {
	cout << "VMTCPManager start" << endl;
	cout << "Server start" << endl;
	boost::asio::io_service ios;
	tcp::acceptor acceptor(ios, tcp::endpoint(tcp::v4(), 7900));
	while (true) {		
		boost::shared_ptr<tcp::socket> socket2(new tcp::socket(ios));
		acceptor.accept(*socket2.get());
		threadsList.push_back(new boost::thread(threadVMConnection, socket2));
		cout << "connected" << endl;
	}
	cout << "VMTCPManager end" << endl;
}

