
#include "blinkyBlocksVM.h"
#include "blinkyBlocksBlock.h"
#include <sys/wait.h>
#include <stdio.h>
#include <boost/bind.hpp>
#include "trace.h"
#include <stdexcept>

using namespace boost;
using asio::ip::tcp;

namespace BlinkyBlocks {

//int BlinkyBlocksVM::port=5000; // default
boost::asio::io_service *BlinkyBlocksVM::ios = NULL;
tcp::acceptor *BlinkyBlocksVM::acceptor = NULL;
string BlinkyBlocksVM::vmPath;
string BlinkyBlocksVM::programPath;
bool BlinkyBlocksVM::debugging = false;

BlinkyBlocksVM::BlinkyBlocksVM(BlinkyBlocksBlock* bb){
	assert(ios != NULL && acceptor != NULL);
	hostBlock = bb;
	OUTPUT << "VM "<< hostBlock->blockId << " constructor" << endl;
	// Start the VM
	pid = 0;
	pid = fork();	
	if(pid < 0) {ERRPUT << "Error when starting the VM" << endl;}
    if(pid == 0) {
		stringstream output;
		output << "VM" << hostBlock->blockId << ".log";
		int fd = open(output.str().c_str(), O_RDWR | O_CREAT, S_IRUSR | S_IWUSR);
		dup2(fd, 1);
		dup2(fd, 2);
		close(fd);
		if (debugging) {
			char* cmd[] = {(char*)vmPath.c_str(), (char*)"-f", (char*)programPath.c_str(), (char*)"-S", NULL };
			OUTPUT << "debugging mode!" << endl;
			execv(vmPath.c_str(), const_cast<char**>(cmd));
		} else {
			char* cmd[] = {(char*)vmPath.c_str(), (char*)"-f", (char*)programPath.c_str(), (char*)"-c", (char*) "sl", NULL };
			OUTPUT << "no debugging mode!" << endl;	
			execv(vmPath.c_str(), const_cast<char**>(cmd));
		}
	}

	// Wait for an incoming connection	
	socket = boost::shared_ptr<tcp::socket>(new tcp::socket(*ios));	
	acceptor->accept(*(socket.get()));
	OUTPUT << "VM "<< hostBlock->blockId << " connected" << endl;
}

BlinkyBlocksVM::~BlinkyBlocksVM() {
	OUTPUT << "VM "<< hostBlock->blockId << " destroyed" << endl;
	closeSocket();
	terminate();
}

void BlinkyBlocksVM::stop() {
	closeSocket();
	terminate();
}

void BlinkyBlocksVM::waitEnd() {	
	waitpid(pid, NULL, 0);
}

void BlinkyBlocksVM::terminate() {
	if (hostBlock->state == Alive) {
		kill(pid, SIGTERM);
		waitEnd();
	}
}

void BlinkyBlocksVM::closeSocket() {
	OUTPUT << "VM "<< hostBlock->blockId << " close socket" << endl;
	if (socket != NULL) {
		socket->cancel();
		OUTPUT << "VM "<< hostBlock->blockId << " cancel socket" << endl;
		socket->close();
		socket.reset();
	}
}

void BlinkyBlocksVM::asyncReadMessageHandler(const boost::system::error_code& error, std::size_t bytes_transferred) {
	OUTPUT << "handler called" << endl;
	if(error) {
		ERRPUT << "an error occurred while receiving a tcp message from VM " << hostBlock->blockId << " (socket closed ?) " <<endl;
		return;
	}
    try {
		boost::asio::read(getSocket(),boost::asio::buffer((void*)(inBuffer.message + 1), inBuffer.message[0]) );
	} catch (std::exception& e) {
		ERRPUT << "connection to the VM "<< hostBlock->blockId << " lost" << endl;
	}
    ((BlinkyBlocksBlockCode*)hostBlock->blockCode)->handleNewMessage();
    this->asyncReadMessage();
}
  
void BlinkyBlocksVM::asyncReadMessage() {
	if (socket == NULL) {
		ERRPUT << "the simulator is not connected to the VM "<< hostBlock->blockId << endl;
		return;
	}
	try {
	boost::asio::async_read(getSocket(), 
		boost::asio::buffer(inBuffer.message, sizeof(uint64_t)),
		boost::bind(&BlinkyBlocksVM::asyncReadMessageHandler, this, boost::asio::placeholders::error,
		boost::asio::placeholders::bytes_transferred));
	} catch (std::exception& e) {
		ERRPUT << "connection to the VM "<< hostBlock->blockId << " lost" << endl;
	}
}
  
void BlinkyBlocksVM::sendMessage(uint64_t size, uint64_t* message){
	if (socket == NULL) {
		ERRPUT << "the simulator is not connected to the VM "<< hostBlock->blockId << endl;
		return;
	}
	try {
		boost::asio::write(getSocket(), boost::asio::buffer((void*)message,size));
	} catch (std::exception& e) {
		ERRPUT << "connection to the VM "<< hostBlock->blockId << " lost" << endl;
	}
}

}
