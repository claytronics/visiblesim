#include <string.h>
#include <pthread.h>
#include <iostream>
#include <sstream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "debug_Simprompt.hpp"
#include "debug_Simhandler.hpp"
#include "serialization.hpp"
#include "types.hpp"

using namespace std;
using namespace debugger;


namespace debugger {

#define SIZE (sizeof(message_type))
#define BROADCAST true

    /*****************************************************************/

    /*GLOBAL STORED VARS: debug handling specific*/

    /****************************************************************/

    std::queue<message_type*> *messageQueue;
    /*global variables to controll main thread*/
    static bool isSystemPaused = true;
    static bool isDebug = false;
    static bool isSimDebug = false;
    static bool isPausedAtBreakpoint = false;

    /*number of messages the Master expects to recieve*/
    int numberExpected = 0;

    /**********************************************************************/

    /* I/0 SPECIFICATION PARSING */

    /*********************************************************************/


    /*returns the index of a character in a string,
     *if it is not there it returns -1*/
    int characterInStringIndex(string str, char character){
        for(unsigned int i = 0; i < str.length(); i++){
            if (str[i] == character)
                return (int)i;
        }
        return -1;
    }


    /*extracts the type from the specification from input line
     *returns the type of breakpoint from the specification
     *invariant-- the type is always specified*/
    string getType(string specification){
        string build = "";
        for (unsigned int i = 0; i < specification.length(); i++){
            if(specification[i] == ':' || specification[i] == '@')
                return build;
            else
                build += specification[i];
        }
        return build;
    }


    /*extracts the name from the specification
     *returns the name from the specification
     *returns "" if name is not present*/
    string getName(string specification){
        string build = "";
        //find index of colon
        int index = characterInStringIndex(specification, ':');
        // if colon not there
        if (index == -1)
            return "";
        for (unsigned int i = index +1;
             i < specification.length(); i++){
            if (specification[i] == '@')
                return build;
            else
                build += specification[i];
        }
        return build;
    }


    /*extracts the node from the specification
     *returns the node from the specification
     *returns "" if node is not given*/
    string getNode(string specification){
        string build = "";
        int index = characterInStringIndex(specification, '@');
        if (index == -1)
            return "";
        for (unsigned int i = index+1; i < specification.length(); i++){
            build+=specification[i];
        }
        return build;
    }

    /*given the encoding return the corresponding string for
     *break point types*/
    string typeInt2String(int type){
        switch(type){
            case FACTDER:
                return "factDer";
            case FACTCON:
                return "factCon";
            case FACTRET:
                return "factRet";
            case ACTION:
                return "action";
            case SENSE:
                return "sense";
            case BLOCK:
                return "block";
        }
        return "";
    }

    /**********************************************************************/

    /*SIMULATION DEBUGGING FUNCTIONS*/

    /*********************************************************************/


    /*DEBUG CONTROLLER -- main controller of pausing/unpausing/dumping VMs*/
    /*execute instruction based on encoding and specification
     *call from the debug_prompt -- There are two different sides to
     *this function:  There is one side that handles sending messages to
     *processes in which these process will recieve that message
     *The other side pertains to the processes that are controlled by the
     *the master process.  They will change their system state and give
     *feed back to the master process (see debugger::display())
     *When the master sends a message, it will expect to see a certain
     *amount of messages sent back*/
    void debugController(int instruction, string specification){

        string type;
        string name;
        string node;

        /*for use of numberExpected see debug_prompt.cpp, run()*/

        /*if MPI debugging and the master process:
         *send a  message instead of changing the system state
         *as normally done in normal debugging*/

            /*process of master debugger in MPI DEBUGGINGMODE*/
            if (instruction == CONTINUE || instruction == UNPAUSE){

                /*continue a paused system by broadcasting an UNPAUSE signal*/
                numberExpected = sendMsg(-1,CONTINUE,"",BROADCAST);
                unPauseSimulation();

            } else if (instruction == DUMP) {

                /*broadcast the message to all VMs*/
                if (specification == "all"){

                    numberExpected = sendMsg(-1,DUMP,specification,BROADCAST);

                } else {

                    /*send to a specific VM to dump content*/
                    numberExpected = sendMsg(atoi(specification.c_str()),
                                             DUMP,specification);
                }

                /*handle the breakpoints in the lists*/
            } else if (instruction == REMOVE||instruction == BREAKPOINT) {

                /*extract the destination*/
                node = getNode(specification);
                if (node == ""){

                    /*broadcast the message if the node is not specified*/
                    numberExpected = sendMsg(-1,instruction,specification,
                                             BROADCAST);

                } else {

                    /*send break/remove to a specific node */
                    numberExpected = sendMsg(atoi(node.c_str()),
                                             instruction,specification);

                }


            } else if (instruction == PRINTLIST) {

                /*broadcast  a pause message*/
                numberExpected = sendMsg(-1,PRINTLIST,"",BROADCAST);

            }

    }


    /*the controller for the master MPI debugger-called when messages are
    *received*/
    void debugMasterController(int instruction, string specification){

        /*print the output and then tell all other VMs to pause*/
        if (instruction == BREAKFOUND){
            printf("%s",specification.c_str());
        /*print content from a VM*/
            sendMsg(-1,PAUSE,"",BROADCAST);
            pauseSimulation();
        } else if (instruction == PRINTCONTENT){
            printf("%s",specification.c_str());
        } else if (instruction == TERMINATE){
            printf("PROGRAM FINISHED\n");
            exit(0);
        }
    }


    /***************************************************************************/

    /*DEBUG MESSAGE SENDING*/

    /***************************************************************************/


    /*return the size of a packed array stored with content of an unkown size*/
    inline int getSize(string content){

        /*will always return an integer*/
        return  3 + ((content.size()+1) + (SIZE-(content.size()+1)%SIZE))/SIZE;
    }


    void messageQueueInsert(uint64_t* msg){
        messageQueue->push(msg);
    }

    /*given the type encoding and content, pack the information into
     *a sendable messeage*/
    message_type* pack(int msgEncode, string content){

        utils::byte* msg = (utils::byte*)new message_type[MAXLENGTH];
        int pos = 0;
        message_type debugFlag =  DEBUG;
        size_t size = content.length() + 1;
        size_t bufSize = MAXLENGTH*SIZE;
        message_type msgSize = bufSize;



        /*same as above for first three fields*/
        utils::pack<message_type>(&msgSize,1,msg,bufSize,&pos);
        utils::pack<message_type>(&debugFlag,1,msg,bufSize,&pos);
        utils::pack<int>(&msgEncode,1,msg,bufSize,&pos);
        utils::pack<size_t>(&size,1,msg,bufSize,&pos);

        /*add the content into the buffer*/
        utils::pack<char>((char*)content.c_str(),content.size()+1,
                                 msg,bufSize,&pos);
        return (message_type*)msg;

    }




    /*the desination specified is the process*/
    /*send a debug message to another process through the MPI layer*/
    /*if send to all, specify BROADCAST (see top)*/
    int sendMsg(int destination, int msgType,
              string content, bool broadcast)  {


        /*pack the message*/
        message_type* msg = pack(msgType,content);

        size_t msgSize = MAXLENGTH*SIZE;

        if (broadcast){

            return debugSendMsg(-1,msg,msgSize);

        } else {

            /*get the process id (getVMId) and send the message*/
            return debugSendMsg(destination,msg,
                              msgSize);
        }

    }


    void receiveMsg(void){

        utils::byte *msg;
        int instruction;
        char specification[MAXLENGTH*SIZE];
        int pos = 0;
        message_type size;
        message_type debugFlag;
        size_t specSize;

        while(!messageQueue->empty()){
            /*process each message until empty*/
            /*extract the message*/
            msg = (utils::byte*)messageQueue->front();

            /*unpack the message into readable form*/
            utils::unpack<message_type>(msg,MAXLENGTH*SIZE,&pos,&size,1);
            utils::unpack<message_type>(msg,MAXLENGTH*SIZE,&pos,&debugFlag,1);
            utils::unpack<int>(msg,MAXLENGTH*SIZE,&pos,&instruction,1);
            utils::unpack<size_t>(msg,MAXLENGTH*SIZE,&pos,&specSize,1);
            utils::unpack<char>(msg,MAXLENGTH*SIZE,&pos,
                                &specification,specSize);
            string spec(specification);


            debugMasterController(instruction,spec);
            numberExpected--;

            /*set up the variables and buffers for next message*/
            memset(specification,0,MAXLENGTH*SIZE);
            messageQueue->pop();
            memset(msg,0,MAXLENGTH*SIZE);
            pos = 0;
        }
    }

}
