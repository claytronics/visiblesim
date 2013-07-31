
/* Interface for debugging- spawns a  prompt that will controll the main thread if it hits a specifies break point*/

#include <pthread.h>
#include <iostream>
#include <string>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "debug_Simprompt.hpp"

using namespace std;

namespace debugger {

    /*to store the last input in the debugger*/
    int lastInstruction = 0;
    string lastBuild = "";


    int (*debugSendMsg)(int,message_type*,int);
    void (*pauseSimulation)(void);
    void (*unPauseSimulation)(void);
    void (*quitDebugger)(void);

    /*spawn the debbugging prompt as a separate thread to
      controll the main one*/
    ftrp initDebugger(int (*sendMsg)(int,message_type*,int),
                      void (*pauseSim)(void),
                      void (*unPauseSim)(void),
                      void (*quitDebug)(void),
                      std::ostream& o, std::istream& i){


        pthread_t tid;


        cin.rdbuf(i.rdbuf());
        cout.rdbuf(o.rdbuf());

        messageQueue = new std::queue<message_type*>();

        debugSendMsg = sendMsg;
        pauseSimulation = pauseSim;
        unPauseSimulation = unPauseSim;
        quitDebugger = quitDebug;

        pthread_create(&tid,NULL,run, NULL);

        return messageQueueInsert;
    }

  //continuously attend command line prompt for debugger
  //when the system is not paused
  void *run(void* debugAll){

      (void)debugAll;
    string inpt;
    bool expectingMessage;

    while(true){
      if (numberExpected==0){
        cout << ">";
        getline(cin,inpt);
        //react to the input
        expectingMessage = parseline(inpt);
      }

      if (expectingMessage){
          usleep(10000);
          receiveMsg();
      }

    }
    return NULL;
  }



  /*parses the command line and run the debugger*/
  bool parseline(string line){

    string build = "";
    int wordCount = 1;

    int command = NOTHING;

    /*empty input*/
    if (line == ""){
      //enterlast stored command
      debugController(lastInstruction, lastBuild);
      return true;
    }

    /*loop through input line*/
    for (unsigned int i = 0; i < line.length(); i++){


      /*parse line for words*/
      if (line[i]!=' '){
        build += line[i];
      } else {
        //exract the command
        if (wordCount == 1){
          command = handle_command(build);
          if (command==NOTHING){
              return false;
          }
        wordCount++;
        build = "";
        }
      }
    }


    /*no whitespace at all-single word commands*/
    if (wordCount == 1){
      command = handle_command(build);

      if (command==NOTHING){
              return false;
          }

      if (command != BREAKPOINT && command!=DUMP
          && command != REMOVE && command != MODE){
        debugController(command, build);
        lastInstruction = command;
        lastBuild = build;
        return true;
      }
    }

    /*if not enough info - these  types must have a specification*/
    if ((command == BREAKPOINT||command == DUMP||command == REMOVE||
            command == MODE)&&
        wordCount == 1){
      cout << "Please specify- type help for options" << endl;
      return false;
    }

    /*handle breakpointsand dumps*/
    if (wordCount == 2){
        if (command == BREAKPOINT||command == DUMP||command == REMOVE
            ||command == MODE){
        debugController(command,build);
        } else
        debugController(command,"");
      lastInstruction = command;
      lastBuild = build;
      return true;
    }
    return false;

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
      retVal = RUN;
    } else if (command == "dump"||command == "d") {
      retVal = DUMP;
    } else if (command == "print" || command == "p"){
      retVal = PRINTLIST;
    } else if (command == "mode" || command == "m"){
        retVal = MODE;
    } else if (command == "remove" || command == "rm"){
      retVal = REMOVE;
    } else if (command == "continue"||command == "c"){
      retVal = CONTINUE;
    } else if (command == "quit"||command == "q"){
      sendMsg(-1,TERMINATE,"",true);
      delete messageQueue;
      quitDebugger();
      exit(0);
    } else {
      cout << "unknown command: type 'help' for options " << endl;
      retVal = NOTHING;
    }
    return retVal;
  }


  /*prints the help screen*/
  void help(){
    cout << endl;
    cout << "*******************************************************************" << endl;
    cout << endl;
    cout << "DEBUGGER HELP" << endl;
    cout << "\t-break <Specification>- set break point at specified place" << endl;
    cout << "\t\t-Specification Format:" << endl;
    cout << "\t\t  <type>:<name>@<node> OR" << endl;
    cout << "\t\t  <type>:<name>        OR" << endl;
    cout << "\t\t  <type>@<node>" << endl;
    cout << "\t\t    -type - [factRet|factDer|factCon|action|sense|block]" << endl;
    cout << "\t\t\t-a type MUST be specified" << endl;
    cout << "\t\t    -name - the name of certain type ex. the name of a fact" << endl;
    cout << "\t\t    -node - the number of the node" << endl;
    cout << "\t-dump or d <nodeID> <all> - dump the state of the system" << endl;
    cout << "\t-continue or c - continue execution" << endl;
    cout << "\t-run or r - start the program" << endl;
    cout << "\t-quit - exit debugger" << endl;
    cout << endl;
    cout << "\t-Press Enter to use last Input" << endl;
    cout << endl;
    cout << "*******************************************************************" << endl;
  }
}
