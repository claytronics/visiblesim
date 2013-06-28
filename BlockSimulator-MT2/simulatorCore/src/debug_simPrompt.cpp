
/* Interface for debugging- spawns a  prompt that will controll the main thread if it hits a specifies break point*/


#include <pthread.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
#include "debug_simHandler.hpp"

using namespace std;

/*function prototypes*/
void *run_debugger(void * holder);
void parseline(string line);
int handle_command(string command);
void help();
void messageHandler(uint64_t* msg);
void debugSend(int command, string build);

/*to store the last input in the debugger*/
int lastInstruction = 0;
string lastBuild = "";
ostream debugout = cout;
istream debugin = cin;


#define DEBUG 16
#define SIZE (sizeof(uint64_t))

static bool isPaused = false;
static void (*sendMessage)(int,int,uint64_t*);
static void (*pauseSimulation)(int);
static void (*unPauseSimulation)(void);

/*spawn the debbugging prompt as a separate thread to
  controll the main one*/
void (*initDebugger(void (*sendMsg)(int,int,uint64_t*),
		    void (*pauseSim)(int),
		    void (*unPauseSim)(void),
		    ostream& o, istream& i))(uint64_t*){
  

  pthread_t tid;
  
  debugout = o;
  debugin = i;

  sendMessage = sendMsg;
  pauseSimulation = pauseSim;
  unPauseSimulation = unPauseSim;

  isPaused = true;
  pthread_create(&tid,NULL,run_debugger, NULL);

  return messageHandler;
}


/*to be called by the simulator to controll the debugger
 * or to display some info*/
void messageHandler(uint64_t* msg){

  int sizeMsg;
  int command = (int)msg[2];

  if (command == BREAKPOINT){
    sizeMsg = 2*SIZE;
    uint64_t msgSend[sizeMsg/SIZE];
    msgSend[0] = sizeMsg;
    msgSend[1] = DEBUG;
    msgSend[2] = PAUSE;
    sendMessage(-1,sizeMsg+SIZE,(uint64_t*)msgSend);
    pauseSimulation(0);
    debugout << (char*)&msg[3];
    isPaused = true;

  } else if (command == PRINTCONTENT){
    debugout << (char*)&msg[3];
    isPaused = true;
  }
}


//continuously attend command line prompt for debugger
//when the system is not paused
void *run_debugger(void* holder){

  (void)holder;
  string inpt;
  
  debugout << "BLINKY BLOCK DEBUGGING MODE" << endl;

  while(true){
    if (isPaused){
      out << ">";
      isPaused = false;
      getline(debugin,inpt);
      //react to the input
      parseline(inpt);
    }
  }
  return NULL;
}



/*parses the command line and run the debugger*/
void parseline(string line){

  string build = "";
  int wordCount = 1;
  
  int command = NOTHING;

  /*empty input*/
  if (line == ""){
    //enterlast stored command
    debugSend(lastInstruction,lastBuild);
    return;
  }

  /*loop through input line*/
  for (unsigned int i = 0; i < line.length(); i++){
    

    /*parse line for words*/
    if (line[i]!=' '){
      build += line[i];

    } else {
      //exract the command
      if (wordCount == 1)
	command = handle_command(build);
    wordCount++;
    build = "";
    }
  }
    

  /*no whitespace at all-single word commands*/
  if (wordCount == 1){
    command = handle_command(build);
    
    if (command != BREAKPOINT && command!=DUMP){
      debugSend(command,"");
      lastInstruction = command;
      lastBuild = build;
      return;
    }
  }
  
  /*if not enough info - these  types must have a specification*/
  if ((command == BREAKPOINT||command == DUMP)&& wordCount == 1){
      debugout << "Please specify- type help for options" << endl;
      isPaused = true;
      return;
  }

  /*handle breakpoints and dumps*/
  if (wordCount == 2){
	if (command == BREAKPOINT||command == DUMP)
	  debugSend(command,build);
	else 
	  debugSend(command,"");
      lastInstruction = command;
      lastBuild = build;
  }
}


int stringType2Int(string type){
  if (type == "factDer"){
    return FACTDER;
  } else if (type == "factCon"){
    return FACTCON;
  } else if (type == "factRet"){
    return FACTRET;
  } else if (type == "sense"){
    return SENSE;
  } else if (type == "action"){
    return ACTION;
  } else if (type == "block"){
    return BLOCK;
  } else { 
    debugout << "unknown type-- enter help for options" << endl;
    return -1;
  }
}


/*constructs a message to be sent to the Simulator*/
void debugSend(int command, string build){
  
  int node;
  string name;
  int size;
  int type;
  char* nameSpot;

  
  if(command == CONTINUE){
    size = 2*SIZE;
    uint64_t msgCont[size/SIZE];
    msgCont[0] = size;
    msgCont[1] = DEBUG;
    msgCont[2] = UNPAUSE;
    /*broadcast unpause to all VMs*/
    sendMessage(-1,size+SIZE,(uint64_t*)msgCont);
    unPauseSimulation();

  } else if (command == BREAKPOINT) {
    if (build[0] == ':'||build[0] == '@'){
      debugout << "Please Specify a Type" << endl;
      isPaused = true;
      return;
    }
    type = stringType2Int(getType(build));
    if (type == -1){
      isPaused = true;
      return;
    }
    /*if no node specified broadcast to all*/
    if (getNode(build) == "")
      node = -1;
    else {
      node = atoi(getNode(build).c_str());
      if (node <= 0){
	debugout << "Invalid Node ID" << endl;
	isPaused = true;
	return;
      }
    }
    name = getName(build);
    size = 3*SIZE + (name.length() + 1) 
      + (SIZE - (name.length() + 1)%SIZE);
    uint64_t msgBreak[size/SIZE];
    msgBreak[0] = size;
    msgBreak[1] = DEBUG;
    msgBreak[2] = BREAKPOINT;
    msgBreak[3] = type;
    nameSpot = (char*)&msgBreak[4];
    memcpy(nameSpot,name.c_str(),name.length()+1);
    sendMessage(node,size+SIZE,(uint64_t*)msgBreak);

  } else if (command == DUMP){
    if (build == "all")
      node = -1;
    else {
      node = atoi(build.c_str());
      if (node<=0){
	debugout << "Invalid Node Range" << endl;
	isPaused = true;
	return;
      }
    }
    size = 2*SIZE;
    uint64_t msgDump[size/SIZE];
    msgDump[0] = size;
    msgDump[1] = DEBUG;
    msgDump[2] = DUMP;
    sendMessage(node,size+SIZE,(uint64_t*)msgDump);

  } else if (command == NOTHING){
    isPaused = true;
  }
  return;
}
		    
		    

	       


/*recognizes and sets different modes for the debugger*/
int handle_command(string command){

  int retVal;

  if (command == "break"){
    retVal = BREAKPOINT;
  } else if (command == "help"||command == "h") {
    help();
    retVal = NOTHING;
  } else if (command == "run"|| command == "r") {
    retVal = CONTINUE;
  } else if (command == "dump"||command == "d") {
    retVal = DUMP;
  } else if (command == "continue"||command == "c"){
    retVal = CONTINUE;
  } else if (command == "quit"||command == "q"){
    exit(0);
  } else {
    debugout << "unknown command: type 'help' for options " << endl;
    retVal = NOTHING;
  }
  return retVal;
}


/*prints the help screen*/
void help(){
  debugout << endl;
  debugout << "*******************************************************************" << endl;
  debugout << endl;
  debugout << "DEBUGGER HELP" << endl;
  debugout << "\t-break <Specification>- set break point at specified place" << endl;
  debugout << "\t\t-Specification Format:" << endl;
  debugout << "\t\t  <type>:<name>@<block> OR" << endl;
  debugout << "\t\t  <type>:<name>        OR" << endl;
  debugout << "\t\t  <type>@<block>" << endl;
  debugout << "\t\t    -type - [factRet|factDer|factCon|action|sense|block]" << endl;
  debugout <<             "\t\t\t-a type MUST be specified" << endl;
  debugout << "\t\t    -name - the name of certain type ex. the name of a fact" << endl;
  debugout << "\t\t    -node - the number of the node" << endl;
  debugout << "\t-dump or d <nodeID> <all> - dump the state of the system" << endl;
  debugout << "\t-continue or c - continue execution" << endl;
  debugout << "\t-run or r - start the program" << endl;
  debugout << "\t-quit - exit debugger" << endl;
  debugout << endl;
  debugout << "\t-Press Enter to use last Input" << endl;
  debugout << endl;
  debugout << "*******************************************************************" << endl;
}
  



  
    

  
