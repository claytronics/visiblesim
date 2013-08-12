/*
 * VMCommands.cpp
 *
 *  Created on: 11 aout 2013
 *      Author: Andre
 */

#include "VMCommands.h"

//===========================================================================================================
//
//          VMCommand  (class)
//
//===========================================================================================================

VMCommand::VMCommand(commandType *b, commandType s, commandType t, commandType ts, commandType src) {
	command = b;
	command[0] = s;
	command[1] = t;
	command[2] = ts;
	command[3] = src;
}

VMCommand::VMCommand(commandType *c) {
	command = c;
}

/*
static commandType VMCommand::getContentSize(commandType *c) {
	return c[0];
}

static commandType VMCommand::getTotalSize(commandType *c) {
	return getContentSize(c) + sizeof(commandType);
} */

commandType VMCommand::getType(commandType *c) {
	return c[1];
}

commandType VMCommand::getTimestamp(commandType *c) {
	return c[2];
}

commandType VMCommand::getContentSize() {
	return command[0];
}

commandType VMCommand::getTotalSize() {
	return getContentSize() + sizeof(commandType);
}

commandType VMCommand::getType() {
	return command[1];
}

commandType VMCommand::getTimestamp() {
	return command[2];
}

commandType VMCommand::getSource() {
	return command[3];
}

commandType* VMCommand::getCommand() {
	return command;
}

//===========================================================================================================
//
//          SetColorVMCommand  (class)
//
//===========================================================================================================

SetColorVMCommand::SetColorVMCommand(commandType* c): VMCommand(c) {};

Vecteur SetColorVMCommand::getColor() {
	return Vecteur((float)command[4]/255.0, (float)command[5]/255.0, (float)command[6]/255.0, (float)command[7]/255.0);
}

//===========================================================================================================
//
//          TapVMCommand  (class)
//
//===========================================================================================================

TapVMCommand::TapVMCommand(commandType *b, commandType ts, commandType src) : 
	VMCommand(b, 3*sizeof(commandType), VM_MESSAGE_TAP, ts, src) { };
