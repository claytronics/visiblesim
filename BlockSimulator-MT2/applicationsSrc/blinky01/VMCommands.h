/*
 * VMCommands.h
 *
 *  Created on: 11 aout 2013
 *      Author: Andre
 */

#ifndef VMCOMMANDS_H_
#define VMCOMMANDS_H_

#include "network.h"
#include "vecteur.h"

#define VM_MESSAGE_MAXLENGHT 544 // debugger

#define VM_MESSAGE_SET_ID						1
#define VM_MESSAGE_STOP							4
#define VM_MESSAGE_ADD_NEIGHBOR					5
#define VM_MESSAGE_REMOVE_NEIGHBOR				6
#define VM_MESSAGE_TAP							7
#define VM_MESSAGE_SET_COLOR					8
#define VM_MESSAGE_SEND_MESSAGE					12
#define VM_MESSAGE_RECEIVE_MESSAGE				13
#define VM_MESSAGE_ACCEL						14
#define VM_MESSAGE_SHAKE						15
#define VM_MESSAGE_DEBUG						16

#define VM_SET_DETERMINISTIC_MODE				20
#define VM_MESSAGE_START_COMPUTATION 			21
#define VM_MESSAGE_END_COMPUTATION 				22

#define VM_MESSAGE_TIME_INFO					23


typedef uint64_t commandType;


//===========================================================================================================
//
//          VMCommand  (class)
//
//===========================================================================================================

class VMCommand {

protected:
	commandType *command;
	VMCommand(commandType *b, commandType s, commandType t, commandType ts, commandType src);
	VMCommand(commandType *c);
public:
	//commandType *command;
	/*commandType size;
	commandType type;
	commandType	timestamp;
	commandType source;*/
	//IncomingVMCommand(commandType *c);
	
	//static commandType getContentSize(commandType *c);
	//static commandType getTotalSize(commandType *c);
	static commandType getType(commandType *c);
	static commandType getTimestamp(commandType *c);
	//static commandType getSource(commandType *c);
	
	commandType getContentSize();
	commandType getTotalSize();
	commandType getType();
	commandType getTimestamp();
	commandType getSource();
	
	commandType* getCommand();
	
};


//===========================================================================================================
//
//          SetColorVMCommand  (class)
//
//===========================================================================================================

class SetColorVMCommand : public VMCommand {
public:
	
	SetColorVMCommand(commandType *c);
		
	Vecteur getColor();
};


//===========================================================================================================
//
//          SendMessageVMCommand  (class)
//
//===========================================================================================================

/*
class SendMessageVMCommand : public IncomingVMCommand, public Message {
public:
	
	
	SendMessageVMCommand(commandType *c);
	SendMessageVMCommand(SendMessageVMCommand *c);
	~SendMessageVMCommand();
};*/


//===========================================================================================================
//
//          TapVMCommand  (class)
//
//===========================================================================================================

class TapVMCommand : public VMCommand {
public:
	TapVMCommand(commandType *b, commandType ts, commandType src);	
};


#endif
