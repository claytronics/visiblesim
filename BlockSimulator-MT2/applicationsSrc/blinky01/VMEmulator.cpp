/*
 * VMEmulator.cpp
 *
 *  Created on: 12 mars 2013
 *      Author: dom
 */

#include <iostream>
#include <boost/asio.hpp>
#include <list>
#include <boost/thread.hpp>

using namespace std;
using boost::asio::ip::tcp;

#define VM_MESSAGE_SET_ID		1
#define VM_MESSAGE_STOP			4
#define VM_MESSAGE_ADD_NEIGHBOR		5
#define VM_MESSAGE_REMOVE_NEIGHBOR	6
#define VM_MESSAGE_TAP			7
#define VM_MESSAGE_SET_COLOR		8
#define VM_MESSAGE_SEND_MESSAGE		9
#define VM_MESSAGE_RECEIVE_MESSAGE	10
#define VM_MESSAGE_ACCEL		11
#define VM_MESSAGE_SHAKE		12

typedef struct VMMessage_tt {
        uint64_t size;
        uint64_t type;
        uint64_t param1;
        uint64_t param2;
	uint64_t param3;
	uint64_t param4;
	uint64_t param5;
} VMMessage_t;

/*
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
list<boost::thread*> threadsList; */

void vm_thread_function(void *data) {	
	boost::asio::io_service ios;
	boost::asio::ip::tcp::resolver resolver(ios);
	boost::asio::ip::tcp::resolver::query query("127.0.0.1", "7800");
	boost::asio::ip::tcp::resolver::iterator iter = resolver.resolve(query);
	boost::asio::ip::tcp::resolver::iterator end;
	boost::asio::ip::tcp::endpoint endpoint;	
	boost::asio::ip::tcp::endpoint VMEndpoint;
	if (iter != end) {
		endpoint = *iter;
		std::cout << endpoint << std::endl;
	} else {
		exit(EXIT_FAILURE);
	}
	tcp::socket socket(ios);
	try {
		socket.connect(*iter);
		cout << "Connected to the Simulator" << endl;
	} catch (std::exception& e) {
		cerr << "Connection to the Simulator failed" << endl;
	}

	VMMessage_t in, out;
	int id;	
	cout << "VMEmulator start" << endl;
	try {
		boost::asio::read(socket,boost::asio::buffer((void*)&in, 3*sizeof(uint64_t)));
		id = in.param1;
		cout << "VM received id: " << id << endl;
	} catch (std::exception& e) {
		cerr << "Connection to the Simulator lost" << endl;
	}

	out.size = 5*sizeof(uint64_t);
	out.type = VM_MESSAGE_SET_COLOR;
	out.param1 = 255; // red
	out.param2 = 0;
	out.param3 = 0;
	out.param4 = 0;
	try {
		boost::asio::write(socket, boost::asio::buffer((void*)&out,6*sizeof(uint64_t)));
		cout << "VM " << id << " sent SET_COLOR(red)" <<  endl;
	} catch (std::exception& e) {
		cerr << "Connection to the Simulator lost" << endl;
	}
	//getchar();
	cout << "VMEmulator end" << endl;

}

/*
void threadFunction(void *data) {
	SimulatedVM *simulatedVM = (SimulatedVM*)data;
	cout << "thread started to handle VM " << simulatedVM->id << endl;
	vm_thread_function(NULL);
	cout << "end of thread handling VM " << simulatedVM->id << endl;
}
*/

int main(int argc, char **argv) {
	vm_thread_function(NULL);
	/*SimulatedVM *sVM = new SimulatedVM();
	simulatedVMList.push_back(sVM);
	threadFunction(sVM);	
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

