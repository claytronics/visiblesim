/*
 * blinkyBlocksVM.h
 *
 *  Created on: 24 juin 2013
 *      Author: Andre
 */

#ifndef BLINKYBLOCKSVM_H_
#define BLINKYBLOCKSVM_H_

#include <iostream>
#include <boost/asio.hpp>
#include <boost/shared_ptr.hpp>
#include <inttypes.h>
#include <boost/interprocess/sync/interprocess_mutex.hpp>

using namespace std;
using boost::asio::ip::tcp;

namespace BlinkyBlocks {

#define VM_MESSAGE_MAXLENGHT 512
typedef uint64_t MessageUnit;

class BlinkyBlocksBlock;

class VMMessage {
public:
	int size;
	uint64_t message[VM_MESSAGE_MAXLENGHT];
	
	VMMessage() { };
	VMMessage(int size) { };
	~VMMessage() { };
};

/*
typedef struct VMMessage_tt {
        uint64_t size;
        uint64_t *message;
} VMMessage_t; */

class BlinkyBlocksVM {

protected:
	/* associated Block */
	BlinkyBlocksBlock* hostBlock;
	/* associated VM program pid */
	pid_t pid;
	/* socket connected to the associated VM program */
	boost::shared_ptr<tcp::socket> socket;
	/* buffer used to receive tcp message */
	VMMessage inBuffer;
	/* Mutex used when sending message */
	boost::interprocess::interprocess_mutex mutex_send;
	
	tcp::socket& getSocket() { assert(socket != NULL); return *(socket.get()); };
	
	//static int port;
	static boost::asio::io_service *ios;
	static tcp::acceptor *acceptor;
	static string vmPath;
	static string programPath;
	static bool debugging;

public:
	
	BlinkyBlocksVM(BlinkyBlocksBlock* bb);
	~BlinkyBlocksVM();

	VMMessage*  getBufferPtr() { return &inBuffer;};

	/*static void setVmPath(string p) { vmPath = p; };
	static void setProgramPath(string p) { programPath = p; };
	static void setDebugging(bool d) { debugging = d;};*/
	
	static void setConfiguration(string v, string p, bool d) {
		vmPath = v;
		programPath = p;
		debugging = d;
	};
	
	static void createServer(int p) {
		assert(ios == NULL);
		ios = new boost::asio::io_service();
		acceptor =  new tcp::acceptor(*ios, tcp::endpoint(tcp::v4(), p));
	};
	
	static void deleteServer() {
		ios->stop();
		delete acceptor;
		delete ios;
		ios = NULL; acceptor = NULL;
	};
	
	/* wait for the termination of the associated VM program */
	void waitEnd();
	/* kill the associated VM program */
	void terminate();
	/* close the socket associated to the VM program */
	void closeSocket();
	/* close the socket, and then kill the vm program */
	void stop();
	
	/* send and receive message from the associated VM program */
	//void sendMessage(VMMessage &m);
	void sendMessage(uint64_t size, uint64_t* m);
	void asyncReadMessage();
	
	static void checkForReceivedMessages() {
		if (ios != NULL) {
			ios->poll();
			ios->reset();
		}
	};

	static void waitForOneMessage() {
		if (ios != NULL) {
			ios->run_one();
			ios->reset();
		}
	};
	
	void asyncReadMessageHandler(const boost::system::error_code& error, std::size_t bytes_transferred);

};

	inline void createVMServer(int p) { BlinkyBlocksVM::createServer(p); };
	inline void deleteVMServer() { BlinkyBlocksVM::deleteServer(); };
	inline void setVMConfiguration(string v, string p, bool d) { BlinkyBlocksVM::setConfiguration(v,p,d); };
	inline void checkForReceivedVMMessages() { BlinkyBlocksVM::checkForReceivedMessages(); };
	inline void waitForOneVMMessage() { BlinkyBlocksVM::waitForOneMessage(); };
}
#endif /* BLINKYBLOCKSVM_H_ */
