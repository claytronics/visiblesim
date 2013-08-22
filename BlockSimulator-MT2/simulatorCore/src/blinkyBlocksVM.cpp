
#include "blinkyBlocksVM.h"
#include "blinkyBlocksBlock.h"
#include "blinkyBlocksBlockCode.h"
#include <sys/wait.h>
#include <stdio.h>
#include <boost/bind.hpp>
#include "trace.h"
#include <stdexcept>
#include <string.h>
#include "events.h"
#include "blinkyBlocksEvents.h"

using namespace boost;
using asio::ip::tcp;

namespace BlinkyBlocks {

boost::asio::io_service *BlinkyBlocksVM::ios = NULL;
tcp::acceptor *BlinkyBlocksVM::acceptor = NULL;
string BlinkyBlocksVM::vmPath;
string BlinkyBlocksVM::programPath;
bool BlinkyBlocksVM::debugging = false;

BlinkyBlocksVM::BlinkyBlocksVM(BlinkyBlocksBlock* bb){
	assert(ios != NULL && acceptor != NULL);
	hostBlock = bb;
	OUTPUT << "VM "<< hostBlock->blockId << " constructor" << endl;
	socket = boost::shared_ptr<tcp::socket>(new tcp::socket(*ios));
	// Start the VM
	pid = 0;
	pid = fork();
	if(pid < 0) {ERRPUT << "Error when starting the VM" << endl;}
    if(pid == 0) {
        stringstream output;
		output << "VM" << hostBlock->blockId << ".log";
		int fd = open(output.str().c_str(), O_RDWR | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
		dup2(fd, 1);
		dup2(fd, 2);
		close(fd);
		if (debugging) {
			//./meld -f  /home/ubuntu/Bureau/CMU/meld/examples/ends.m -c sl -D SIM
			char* cmd[] = {(char*)vmPath.c_str(), (char*)"-f", (char*)programPath.c_str(), (char*)"-c", (char*) "sl", (char*) "-D", (char*) "SIM", NULL };
			execv(vmPath.c_str(), const_cast<char**>(cmd));
		} else {
			//./meld -f  /home/ubuntu/Bureau/CMU/meld/examples/ends.m -c sl
			char* cmd[] = {(char*)vmPath.c_str(), (char*)"-f", (char*)programPath.c_str(), (char*)"-c", (char*) "sl", NULL };
			execv(vmPath.c_str(), const_cast<char**>(cmd));
		}
	}
	acceptor->accept(*(socket.get()));	
	idSent = false;
	nbSentCommands = 0;
	asyncReadCommand();
}

BlinkyBlocksVM::~BlinkyBlocksVM() {
	closeSocket();
	terminate();
}

void BlinkyBlocksVM::terminate() {
	waitpid(pid, NULL, 0);
}

void BlinkyBlocksVM::closeSocket() {
	if (socket != NULL) {
		socket->cancel();
		socket->close();
		socket.reset();
	}
}

void BlinkyBlocksVM::asyncReadCommandHandler(const boost::system::error_code& error, std::size_t bytes_transferred) {
	if(error) {
		ERRPUT << "An error occurred while receiving a tcp command from VM " << hostBlock->blockId << " (socket closed ?) " <<endl;
		return;
	}
    try {
		memset(inBuffer+1, 0, inBuffer[0]);
		boost::asio::read(getSocket(),boost::asio::buffer((void*)(inBuffer + 1), inBuffer[0]) );
	} catch (std::exception& e) {
		ERRPUT << "Connection to the VM "<< hostBlock->blockId << " lost" << endl;
	}
	handleInBuffer();
	while (socket->available()) {
		try {
			boost::asio::read(getSocket(), boost::asio::buffer(inBuffer, sizeof(commandType))); 
			boost::asio::read(getSocket(),boost::asio::buffer((void*)(inBuffer + 1), inBuffer[0]));
		}  catch (std::exception& e) {
			ERRPUT << "Connection to the VM "<< hostBlock->blockId << " lost" << endl;
		}
		handleInBuffer();
	}
    this->asyncReadCommand();
}

void BlinkyBlocksVM::handleInBuffer() {
	BlinkyBlocksBlockCode *bbc = (BlinkyBlocksBlockCode*)hostBlock->blockCode;
	VMCommand command(inBuffer);
	
	if (bbc->mustBeQueued(command)) {
		command.copyData();
		inQueue.push(command);
	} else {
		bbc->handleCommand(command);
	}
}

void BlinkyBlocksVM::asyncReadCommand() {
	if (socket == NULL) {
		ERRPUT << "Simulator is not connected to the VM "<< hostBlock->blockId << endl;
		return;
	}
	try {
	inBuffer[0] = 0;
	boost::asio::async_read(getSocket(),
		boost::asio::buffer(inBuffer, sizeof(commandType)),
		boost::bind(&BlinkyBlocksVM::asyncReadCommandHandler, this, boost::asio::placeholders::error,
		boost::asio::placeholders::bytes_transferred));
	} catch (std::exception& e) {
		ERRPUT << "Connection to the VM "<< hostBlock->blockId << " lost" << endl;
	}
}

void BlinkyBlocksVM::sendCommand(VMCommand &command){
	if (socket == NULL) {
		ERRPUT << "Simulator is not connected to the VM "<< hostBlock->blockId << endl;
		return;
	}
	if (!idSent) {
		idSent = true;
		hostBlock->blockCode->processLocalEvent(EventPtr (new VMSetIdEvent(BaseSimulator::getScheduler()->now(), hostBlock)));
	}
	if (command.getType() != VM_COMMAND_DEBUG) {
		nbSentCommands++;
	}
	((BlinkyBlocksBlockCode*)hostBlock->blockCode)->handleDeterministicMode(command);
	mutex_send.lock();
	try {
		boost::asio::write(getSocket(), boost::asio::buffer(command.getData(), command.getSize()));
	} catch (std::exception& e) {
		ERRPUT << "Connection to the VM "<< hostBlock->blockId << " lost" << endl;
	}
	mutex_send.unlock();
}

void BlinkyBlocksVM::handleQueuedCommands() {
	BlinkyBlocksBlockCode *bbc = (BlinkyBlocksBlockCode*)hostBlock->blockCode;
	while (!inQueue.empty()) {
		VMCommand &command = inQueue.front();
		bbc->handleCommand(command);
		delete[] command.getData();
		inQueue.pop();
	}
}


void BlinkyBlocksVM::checkForReceivedCommands() {
	if (ios != NULL) {
		ios->poll();
		ios->reset();
	}
}

void BlinkyBlocksVM::waitForOneCommand() {
	if (ios != NULL) {
		ios->run_one();
		ios->reset();
	}
	checkForReceivedCommands();
}

void BlinkyBlocksVM::setConfiguration(string v, string p, bool d) {
	vmPath = v;
	programPath = p;
	debugging = d;
}

void BlinkyBlocksVM::createServer(int p) {
	assert(ios == NULL);
	ios = new boost::asio::io_service();
	acceptor =  new tcp::acceptor(*ios, tcp::endpoint(tcp::v4(), p));
}

void BlinkyBlocksVM::deleteServer() {
	ios->stop();
	delete acceptor;
	delete ios;
	ios = NULL; acceptor = NULL;
}


}
