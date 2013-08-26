/*
 * blinkyBlocksVM.h
 *
 *  Created on: 24 juin 2013
 *      Author: Andre
 */

#ifndef BLINKYBLOCKSVM_H_
#define BLINKYBLOCKSVM_H_

#include <iostream>
#include <queue>
#include <inttypes.h>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/interprocess/sync/interprocess_mutex.hpp>
#include <boost/interprocess/sync/interprocess_semaphore.hpp>
#include "blinkyBlocksVMCommands.h"

using namespace std;
using boost::asio::ip::tcp;

namespace BlinkyBlocks {

class BlinkyBlocksBlock;

class BlinkyBlocksVM {

protected:
	/* associated Block */
	BlinkyBlocksBlock* hostBlock;
	/* socket connected to the associated VM program */
	boost::shared_ptr<tcp::socket> socket;
		
	/* True if the id was sent */
	bool idSent;
	
	inline tcp::socket& getSocket() { 
		assert(socket != NULL);
		return *(socket.get()); };
	/* kill the associated VM program (and wait for the effective end) */
	void terminate();
	/* close the socket associated to the VM program */
	void closeSocket();
	
	//static int port;
	static boost::asio::io_service *ios;
	static tcp::acceptor *acceptor;
	static string vmPath;
	static string programPath;
	static bool debugging;

public:
	
	BlinkyBlocksVM(BlinkyBlocksBlock* bb);
	~BlinkyBlocksVM();

	/* associated VM program pid */
	pid_t pid;
	/* buffer used to receive tcp message */
	commandType inBuffer[VM_COMMAND_MAX_LENGHT];	
	commandType outBuffer[VM_COMMAND_MAX_LENGHT];	
	commandType nbSentCommands; // mode fastest 1
			
	/* send and receive message from the associated VM program */
	int sendCommand(VMCommand &command);
	void asyncReadCommand();	
	void asyncReadCommandHandler(const boost::system::error_code& error, std::size_t bytes_transferred);
	void handleInBuffer();
	
	/* kill the VM process */
	void killProcess();
	
	inline static bool isInDebuggingMode() { return debugging; };
	static void setConfiguration(string v, string p, bool d);
	static void createServer(int p);
	static void deleteServer();
	static void checkForReceivedCommands();
	static void waitForOneCommand();
};

	inline void createVMServer(int p) { BlinkyBlocksVM::createServer(p); };
	inline void deleteVMServer() { BlinkyBlocksVM::deleteServer(); };
	inline void setVMConfiguration(string v, string p, bool d) { BlinkyBlocksVM::setConfiguration(v,p,d); };
	inline void checkForReceivedVMCommands() { BlinkyBlocksVM::checkForReceivedCommands(); };
	inline void waitForOneVMCommand() { BlinkyBlocksVM::waitForOneCommand(); };
}
#endif /* BLINKYBLOCKSVM_H_ */
