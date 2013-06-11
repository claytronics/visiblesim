/*
 * VMEmulator.cpp
 *
 *  Created on: 12 mars 2013
 *      Author: dom
 */

#include <iostream>
#include <boost/asio.hpp>
#include "VMEmulator.h"
#include <list>
#include <boost/thread.hpp>

using namespace std;
using boost::asio::ip::tcp;

#define VM_MESSAGE_TYPE_START_SIMULATION        1
#define VM_MESSAGE_TYPE_END_SIMULATION          2
#define VM_MESSAGE_TYPE_CREATE_LINK             3
#define VM_MESSAGE_TYPE_COMPUTATION_LOCK        4
#define VM_MESSAGE_TYPE_COMPUTATION_UNLOCK      5
#define VM_MESSAGE_TYPE_SEND_MESSAGE            6
#define VM_MESSAGE_TYPE_RECEIVE_MESSAGE         7
#define VM_MESSAGE_TYPE_WAIT_FOR_MESSAGE	8

class SimulatedVM {
protected:
//	static int nextId;
public:
	static int nextId;
	int id;
	SimulatedVM() {
		id = nextId;
		nextId++;
		cout << "created VM " << id << endl;
	}
	~SimulatedVM() {

	}
};

int SimulatedVM::nextId = 0;
list<SimulatedVM*> simulatedVMList;
list<boost::thread*> threadsList;

void vm_thread_function(void *data) {
	cout << "VMEmulator start" << endl;
	
	boost::asio::io_service ios;
	boost::asio::ip::tcp::resolver resolver(ios);

	boost::asio::ip::tcp::resolver::query query("127.0.0.1", "7800");

	boost::asio::ip::tcp::resolver::iterator iter = resolver.resolve(query);
	boost::asio::ip::tcp::resolver::iterator end;

	boost::asio::ip::tcp::endpoint endpoint;

	string msg ("Bienvenue sur le serveur !");
	cout << msg << endl;
	if (iter != end) {
		endpoint = *iter;
		std::cout << endpoint << std::endl;
	} else {
		exit(EXIT_FAILURE);
	}

	tcp::socket socket(ios);
	//socket.open(tcp::v4());
	//boost::asio::connect(socket, iter);
	try {
	socket.connect(*iter);
	} catch (std::exception& e) { cerr << "Connection to the Simulator failed" << endl; }

	VMMessage_t response;
	boost::asio::ip::tcp::endpoint VMEndpoint;
	size_t length;

	/*try {
		length = boost::asio::read(socket,boost::asio::buffer((void*)&response, sizeof(response)) );
		if (length > 0) cout << "response : " << response.messageType << " from block " << response.param1 << endl;
		cout << "response received" << endl;
	} catch (std::exception& e) {
		cout << "VMEmulator" << endl;	
	}*/
	
	cout << "VMEmulator start" << endl;
	/*VMMessage_t m1;
	m1.messageType = VM_MESSAGE_TYPE_START_SIMULATION;
	m1.param1 = 2;
	boost::asio::write(socket, boost::asio::buffer((void*)&m1,sizeof(m1)));
	getchar();*/
	VMMessage_t mes;
	boost::asio::read(socket,boost::asio::buffer((void*)&mes, sizeof(VMMessage_t)) );
	cout << "VM id received: " << mes.param1 << endl;
	
	mes.messageType = VM_MESSAGE_TYPE_START_SIMULATION; //1
	mes.param1 = 2;
	boost::asio::write(socket, boost::asio::buffer((void*)&mes,sizeof(VMMessage_t)));
	cout << "VM sends a message: " << mes.param1 << endl;
	cout << "VMEmulator end" << endl;
	/*
	cout << "press a key to proceed" << endl;

	getchar();
	VMMessage_t m1;
	m1.messageType = VM_MESSAGE_TYPE_START_SIMULATION;
	m1.param1 = 2;
	//socket.send_to(boost::asio::buffer((void*)&m1,sizeof(m1)),endpoint);
	boost::asio::write(socket, boost::asio::buffer((void*)&m1,sizeof(m1)));
	cout << "START_SIMULATION sent" << endl;

	getchar();

	m1.messageType = VM_MESSAGE_TYPE_CREATE_LINK;
	m1.param1 = 0;
	m1.param2 = 1;
	//socket.send_to(boost::asio::buffer((void*)&m1,sizeof(m1)),endpoint);
	boost::asio::write(socket, boost::asio::buffer((void*)&m1,sizeof(m1)));
	cout << "CREATE_LINK sent" << endl;

	getchar();
	m1.messageType = VM_MESSAGE_TYPE_COMPUTATION_LOCK;
	m1.param1 = 0;
	m1.param2 = 10000000;
	//socket.send_to(boost::asio::buffer((void*)&m1,sizeof(m1)),endpoint);
	boost::asio::write(socket, boost::asio::buffer((void*)&m1,sizeof(m1)));
	cout << "COMPUTATION_LOCK for 0 sent (10)" << endl;

	getchar();
	m1.messageType = VM_MESSAGE_TYPE_COMPUTATION_LOCK;
	m1.param1 = 1;
	m1.param2 = 20000000;
	//socket.send_to(boost::asio::buffer((void*)&m1,sizeof(m1)),endpoint);
	boost::asio::write(socket, boost::asio::buffer((void*)&m1,sizeof(m1)));
	cout << "COMPUTATION_LOCK for 1 sent (20)" << endl;

//	size_t length = socket.receive_from(boost::asio::buffer((void*)&response, sizeof(VMMessage_t)), VMEndpoint);
	length = boost::asio::read(socket,boost::asio::buffer((void*)&response, sizeof(response)) );
	if (length > 0) cout << "response : " << response.messageType << " from block " << response.param1 << endl;
	cout << "response received" << endl;

	getchar();
	m1.messageType = VM_MESSAGE_TYPE_COMPUTATION_LOCK;
	m1.param1 = 0;
	m1.param2 = 5000000;
	//socket.send_to(boost::asio::buffer((void*)&m1,sizeof(m1)),endpoint);
	boost::asio::write(socket, boost::asio::buffer((void*)&m1,sizeof(m1)));
	cout << "COMPUTATION_LOCK for 0 sent (5)" << endl;

//	length = socket.receive_from(boost::asio::buffer((void*)&response, sizeof(VMMessage_t)), VMEndpoint);
	length = boost::asio::read(socket,boost::asio::buffer((void*)&response, sizeof(response)) );
	if (length > 0) cout << "response : " << response.messageType << " from block " << response.param1 << endl;
	cout << "response received" << endl;

	*/
	/*
	getchar();
	m1.messageType = VM_MESSAGE_TYPE_COMPUTATION_LOCK;
	m1.param1 = 1;
	m1.param2 = 6000000;
	socket.send_to(boost::asio::buffer((void*)&m1,sizeof(m1)),endpoint);
	cout << "COMPUTATION_LOCK for 1 sent (6)" << endl;
*/

/*
	getchar();
	m1.messageType = VM_MESSAGE_TYPE_COMPUTATION_LOCK;
	m1.param1 = 0;
	m1.param2 = 30000000;
	//socket.send_to(boost::asio::buffer((void*)&m1,sizeof(m1)),endpoint);
	boost::asio::write(socket, boost::asio::buffer((void*)&m1,sizeof(m1)));
	cout << "COMPUTATION_LOCK for 0 sent (30)" << endl;

	//length = socket.receive_from(boost::asio::buffer((void*)&response, sizeof(VMMessage_t)), VMEndpoint);
	length = boost::asio::read(socket,boost::asio::buffer((void*)&response, sizeof(response)) );
	if (length > 0) cout << "response : " << response.messageType << " from block " << response.param1 << endl;
	cout << "response received" << endl;

	getchar();
	m1.messageType = VM_MESSAGE_TYPE_WAIT_FOR_MESSAGE;
	m1.param1 = 1;
	//socket.send_to(boost::asio::buffer((void*)&m1,sizeof(m1)),endpoint);
	boost::asio::write(socket, boost::asio::buffer((void*)&m1,sizeof(m1)));
	cout << "WAIT_FOR_MESSAGE sent to 1" << endl;

	//length = socket.receive_from(boost::asio::buffer((void*)&response, sizeof(VMMessage_t)), VMEndpoint);
	length = boost::asio::read(socket,boost::asio::buffer((void*)&response, sizeof(response)) );
	if (length > 0) cout << "response : " << response.messageType << " from block " << response.param1 << endl;
	cout << "response received" << endl;
*/
	/*
	getchar();
	m1.messageType = VM_MESSAGE_TYPE_WAIT_FOR_MESSAGE;
	m1.param1 = 0;
	socket.send_to(boost::asio::buffer((void*)&m1,sizeof(m1)),endpoint);
	cout << "WAIT_FOR_MESSAGE sent to 0" << endl;
*/

/*
	getchar();
	m1.messageType = VM_MESSAGE_TYPE_SEND_MESSAGE;
	m1.param1 = 0;
	m1.param2 = 1;
	m1.param3 = 1500;
	//socket.send_to(boost::asio::buffer((void*)&m1,sizeof(m1)),endpoint);
	boost::asio::write(socket, boost::asio::buffer((void*)&m1,sizeof(m1)));
	cout << "SEND_MESSAGE sent to 0" << endl;

	getchar();
	m1.messageType = VM_MESSAGE_TYPE_WAIT_FOR_MESSAGE;
	m1.param1 = 0;
	//socket.send_to(boost::asio::buffer((void*)&m1,sizeof(m1)),endpoint);
	boost::asio::write(socket, boost::asio::buffer((void*)&m1,sizeof(m1)));
	cout << "WAIT_FOR_MESSAGE sent to 0" << endl;

	//length = socket.receive_from(boost::asio::buffer((void*)&response, sizeof(VMMessage_t)), VMEndpoint);
	length = boost::asio::read(socket,boost::asio::buffer((void*)&response, sizeof(response)) );
	if (length > 0) cout << "response : " << response.messageType << " from block " << response.param1 << endl;
	cout << "response received" << endl;

	getchar();
	m1.messageType = VM_MESSAGE_TYPE_WAIT_FOR_MESSAGE;
	m1.param1 = 1;
	//socket.send_to(boost::asio::buffer((void*)&m1,sizeof(m1)),endpoint);
	boost::asio::write(socket, boost::asio::buffer((void*)&m1,sizeof(m1)));
	cout << "WAIT_FOR_MESSAGE sent to 0" << endl;

	getchar();
	m1.messageType = VM_MESSAGE_TYPE_END_SIMULATION;
	//socket.send_to(boost::asio::buffer((void*)&m1,sizeof(m1)),endpoint);
	boost::asio::write(socket, boost::asio::buffer((void*)&m1,sizeof(m1)));
	cout << "END_SIMULATION sent" << endl;

	cout << "VMEmulator end" << endl;
*/
}

void threadFunction(void *data) {
	SimulatedVM *simulatedVM = (SimulatedVM*)data;
	cout << "thread started to handle VM " << simulatedVM->id << endl;
	vm_thread_function(NULL);
	cout << "end of thread handling VM " << simulatedVM->id << endl;
}


int main(int argc, char **argv) {
	SimulatedVM *sVM = new SimulatedVM();
	simulatedVMList.push_back(sVM);
	threadFunction(sVM);
	/*	
	cout << "VMEmulator start" << endl;

	SimulatedVM *sVM;
	for (int i = 0; i < 2; i++) {
		sVM = new SimulatedVM();
		simulatedVMList.push_back(sVM);
		threadsList.push_back(new boost::thread(threadFunction, sVM));
	}
	list<boost::thread*>::iterator it;
	for (it=threadsList.begin(); it != threadsList.end(); it++) {
		(*it)->join();
	}

	cout << "VMEmulator end" << endl; */
}

