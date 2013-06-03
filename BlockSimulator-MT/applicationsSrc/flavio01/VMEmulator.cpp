/*
 * VMEmulator.cpp
 *
 *  Created on: 12 mars 2013
 *      Author: dom
 */

#include <iostream>
#include <boost/asio.hpp>

using namespace std;
using boost::asio::ip::udp;

#define VM_MESSAGE_TYPE_START_SIMULATION        1
#define VM_MESSAGE_TYPE_END_SIMULATION          2
#define VM_MESSAGE_TYPE_CREATE_LINK             3
#define VM_MESSAGE_TYPE_COMPUTATION_LOCK        4
#define VM_MESSAGE_TYPE_COMPUTATION_UNLOCK      5
#define VM_MESSAGE_TYPE_SEND_MESSAGE            6
#define VM_MESSAGE_TYPE_RECEIVE_MESSAGE         7
#define VM_MESSAGE_TYPE_WAIT_FOR_MESSAGE		8

typedef struct VMMessage_tt {
        uint64_t messageType;
        uint64_t param1;
        uint64_t param2;
        uint64_t param3;
} VMMessage_t;



int main(int argc, char **argv) {
	cout << "VMEmulator start" << endl;
	cout << "VMEmulator end" << endl;

	boost::asio::io_service ios;
	boost::asio::ip::udp::resolver resolver(ios);

	boost::asio::ip::udp::resolver::query query("localhost", "7800");

	boost::asio::ip::udp::resolver::iterator iter = resolver.resolve(query);
	boost::asio::ip::udp::resolver::iterator end;

	boost::asio::ip::udp::endpoint endpoint;

	string msg ("Bienvenue sur le serveur !");
	if (iter != end) {
		endpoint = *iter;
		std::cout << endpoint << std::endl;
	} else {
		exit(EXIT_FAILURE);
	}

	udp::socket socket(ios);
	socket.open(udp::v4());

	VMMessage_t response;
	boost::asio::ip::udp::endpoint VMEndpoint;

	cout << "press a key to proceed" << endl;

	getchar();
	VMMessage_t m1;
	m1.messageType = VM_MESSAGE_TYPE_START_SIMULATION;
	m1.param1 = 2;
	socket.send_to(boost::asio::buffer((void*)&m1,sizeof(m1)),endpoint);
	cout << "START_SIMULATION sent" << endl;

	getchar();
	m1.messageType = VM_MESSAGE_TYPE_CREATE_LINK;
	m1.param1 = 0;
	m1.param2 = 1;
	socket.send_to(boost::asio::buffer((void*)&m1,sizeof(m1)),endpoint);
	cout << "CREATE_LINK sent" << endl;

	getchar();
	m1.messageType = VM_MESSAGE_TYPE_COMPUTATION_LOCK;
	m1.param1 = 0;
	m1.param2 = 10000000;
	socket.send_to(boost::asio::buffer((void*)&m1,sizeof(m1)),endpoint);
	cout << "COMPUTATION_LOCK for 0 sent (10)" << endl;

	getchar();
	m1.messageType = VM_MESSAGE_TYPE_COMPUTATION_LOCK;
	m1.param1 = 1;
	m1.param2 = 20000000;
	socket.send_to(boost::asio::buffer((void*)&m1,sizeof(m1)),endpoint);
	cout << "COMPUTATION_LOCK for 1 sent (20)" << endl;

	size_t length = socket.receive_from(boost::asio::buffer((void*)&response, sizeof(VMMessage_t)), VMEndpoint);
	if (length > 0) cout << "response : " << response.messageType << " from block " << response.param1 << endl;
	cout << "response received" << endl;

	getchar();
	m1.messageType = VM_MESSAGE_TYPE_COMPUTATION_LOCK;
	m1.param1 = 0;
	m1.param2 = 5000000;
	socket.send_to(boost::asio::buffer((void*)&m1,sizeof(m1)),endpoint);
	cout << "COMPUTATION_LOCK for 0 sent (5)" << endl;

	length = socket.receive_from(boost::asio::buffer((void*)&response, sizeof(VMMessage_t)), VMEndpoint);
	if (length > 0) cout << "response : " << response.messageType << " from block " << response.param1 << endl;
	cout << "response received" << endl;

	/*
	getchar();
	m1.messageType = VM_MESSAGE_TYPE_COMPUTATION_LOCK;
	m1.param1 = 1;
	m1.param2 = 6000000;
	socket.send_to(boost::asio::buffer((void*)&m1,sizeof(m1)),endpoint);
	cout << "COMPUTATION_LOCK for 1 sent (6)" << endl;
*/

	getchar();
	m1.messageType = VM_MESSAGE_TYPE_COMPUTATION_LOCK;
	m1.param1 = 0;
	m1.param2 = 30000000;
	socket.send_to(boost::asio::buffer((void*)&m1,sizeof(m1)),endpoint);
	cout << "COMPUTATION_LOCK for 0 sent (30)" << endl;

	length = socket.receive_from(boost::asio::buffer((void*)&response, sizeof(VMMessage_t)), VMEndpoint);
	if (length > 0) cout << "response : " << response.messageType << " from block " << response.param1 << endl;
	cout << "response received" << endl;

	getchar();
	m1.messageType = VM_MESSAGE_TYPE_WAIT_FOR_MESSAGE;
	m1.param1 = 1;
	socket.send_to(boost::asio::buffer((void*)&m1,sizeof(m1)),endpoint);
	cout << "WAIT_FOR_MESSAGE sent to 1" << endl;

	length = socket.receive_from(boost::asio::buffer((void*)&response, sizeof(VMMessage_t)), VMEndpoint);
	if (length > 0) cout << "response : " << response.messageType << " from block " << response.param1 << endl;
	cout << "response received" << endl;

	/*
	getchar();
	m1.messageType = VM_MESSAGE_TYPE_WAIT_FOR_MESSAGE;
	m1.param1 = 0;
	socket.send_to(boost::asio::buffer((void*)&m1,sizeof(m1)),endpoint);
	cout << "WAIT_FOR_MESSAGE sent to 0" << endl;
*/

	getchar();
	m1.messageType = VM_MESSAGE_TYPE_SEND_MESSAGE;
	m1.param1 = 0;
	m1.param2 = 1;
	m1.param3 = 1500;
	socket.send_to(boost::asio::buffer((void*)&m1,sizeof(m1)),endpoint);
	cout << "SEND_MESSAGE sent to 0" << endl;

	getchar();
	m1.messageType = VM_MESSAGE_TYPE_WAIT_FOR_MESSAGE;
	m1.param1 = 0;
	socket.send_to(boost::asio::buffer((void*)&m1,sizeof(m1)),endpoint);
	cout << "WAIT_FOR_MESSAGE sent to 0" << endl;

	length = socket.receive_from(boost::asio::buffer((void*)&response, sizeof(VMMessage_t)), VMEndpoint);
	if (length > 0) cout << "response : " << response.messageType << " from block " << response.param1 << endl;
	cout << "response received" << endl;

	getchar();
	m1.messageType = VM_MESSAGE_TYPE_WAIT_FOR_MESSAGE;
	m1.param1 = 1;
	socket.send_to(boost::asio::buffer((void*)&m1,sizeof(m1)),endpoint);
	cout << "WAIT_FOR_MESSAGE sent to 0" << endl;

	getchar();
	m1.messageType = VM_MESSAGE_TYPE_END_SIMULATION;
	socket.send_to(boost::asio::buffer((void*)&m1,sizeof(m1)),endpoint);
	cout << "END_SIMULATION sent" << endl;



}
