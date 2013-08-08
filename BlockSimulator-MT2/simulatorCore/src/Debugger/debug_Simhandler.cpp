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
#include "blinkyBlocksVM.h"

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
	static bool okayToBroadcastPause = true;

    static bool okayToPauseSimulation = false;

    static bool printLimitation = true;
    static bool okayToPrint = true;

    /*number of messages the Master expects to recieve*/
    int numberExpected = 0;

    bool verboseMode = false;
    bool serializationMode = false;

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

   void setFlags(string specification){
        ostringstream msg;
        for (int i = 0; i < specification.length(); i++){
            if ((uint)specification[i] == 'V'){
                verboseMode = true;
            } else if ((uint)specification[i] == 'S'){
                serializationMode = true;
            }
        }
    }


    void handlePauseCommand(void){

        sendMsg(-1,PAUSE,"",BROADCAST);
        okayToBroadcastPause = false;

    }

    void debugController(int instruction, string specification){

        string type;
        string name;
        string node;


        okayToPrint = true;
        if (instruction == CONTINUE || instruction == UNPAUSE){
            okayToBroadcastPause = true;
            okayToPauseSimulation = true;
            /*continue a paused system by broadcasting an CONTINUE signal*/
            unPauseSimulation(-1);
            printLimitation = false;
            numberExpected = sendMsg(-1,CONTINUE,"",BROADCAST);
        } else if (instruction == RUN){
            okayToBroadcastPause = true;
            okayToPauseSimulation = true;
            printLimitation = false;
            /*continue a paused system by broadcasting an CONTINUE signal*/
            unPauseSimulation(-1);
            numberExpected = sendMsg(-1,CONTINUE,"",BROADCAST);
        } else if (instruction == DUMP) {

            /*broadcast the message to all VMs*/
            if (specification == "all"){

                numberExpected = sendMsg(-1,DUMP,specification,BROADCAST);

                printLimitation = false;
            } else {

                printLimitation = false;
                /*send to a specific VM to dump content*/
                sendMsg(atoi(specification.c_str()),DUMP,specification);
                numberExpected = 1;
            }

            /*handle the breakpoints in the lists*/
        } else if (instruction == REMOVE||instruction == BREAKPOINT) {

            /*extract the destination*/
            node = getNode(specification);
            if (node == ""){

                printLimitation = true;
                /*broadcast the message if the node is not specified*/
                numberExpected = sendMsg(-1,instruction,specification,BROADCAST);

            } else {

                printLimitation = true;
                /*send break/remove to a specific node */
                sendMsg(atoi(node.c_str()),instruction,specification);
                numberExpected = 1;

            }


        } else if (instruction == PRINTLIST) {

            printLimitation = false;
            /*broadcast  a pause message*/
            numberExpected = sendMsg(-1,PRINTLIST,"",BROADCAST);

        } else if (instruction == MODE) {

            printLimitation = true;
            setFlags(specification);
            numberExpected = sendMsg(-1,MODE,specification,
                                     BROADCAST);
        } else if (instruction == TIME) {
            unPauseSimulation(atoi(specification.c_str()));
            numberExpected = 1;
        }

    }


    /*the controller for the master MPI debugger-called when messages are
    *received*/
    void debugMasterController(int instruction, string specification){

        /*print the output and then tell all other VMs to pause*/
        if (instruction == BREAKFOUND){
            printf("%s",specification.c_str());
			/*print content from a VM*/
            if(okayToBroadcastPause) {
				sendMsg(-1,PAUSE,"",BROADCAST);
				okayToBroadcastPause = false;
            }
        } else if (instruction == PRINTCONTENT){
            if (!verboseMode&&printLimitation&&okayToPrint){
                printf("%s",specification.c_str());
                okayToPrint  = false;
            } else if (verboseMode||!printLimitation) {
                printf("%s",specification.c_str());
            }
        } else if (instruction == TERMINATE){
            printf("PROGRAM FINISHED\n");
            exit(0);
        } else if (instruction == PAUSE){

            /*prints more information*/
            if (verboseMode){
                printf("%s",specification.c_str());
            }
        } else if (instruction == TIME){
            printf("%s",specification.c_str());
            if(okayToBroadcastPause) {
				sendMsg(-1,PAUSE,"",BROADCAST);
				okayToBroadcastPause = false;
            }
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
        size_t size = content.length() +1;
        size_t bufSize = MAXLENGTH*SIZE;
        message_type msgSize = bufSize-SIZE;
        utils::byte anotherIndicator = 0;
        message_type timeStamp = 0;
        message_type nodeId = 0;


        utils::pack<message_type>(&msgSize,1,msg,bufSize,&pos);
        utils::pack<message_type>(&debugFlag,1,msg,bufSize,&pos);
        /*timestamp*/
        utils::pack<message_type>(&timeStamp,1,msg,bufSize,&pos);
        /*VM ID*/
        utils::pack<message_type>(&nodeId,1,msg,bufSize,&pos);
        /*indicate if another message is coming*/
        utils::pack<utils::byte>(&anotherIndicator,1,msg,bufSize,&pos);

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
         utils::byte anotherIndicator;
        message_type nodeId;
        message_type timeStamp;

		BlinkyBlocks::checkForReceivedVMMessages();

        while(!messageQueue->empty()){
            /*process each message until empty*/
            /*extract the message*/
            msg = (utils::byte*)messageQueue->front();

            /*unpack the message into readable form*/
            utils::unpack<message_type>(msg,MAXLENGTH*SIZE,&pos,&size,1);
            utils::unpack<message_type>(msg,MAXLENGTH*SIZE,&pos,&debugFlag,1);
            utils::unpack<message_type>(msg,MAXLENGTH*SIZE,&pos,&timeStamp,1);
            utils::unpack<message_type>(msg,MAXLENGTH*SIZE,&pos,&nodeId,1);
            utils::unpack<utils::byte>(msg,MAXLENGTH*SIZE,&pos,
                                        &anotherIndicator,1);
            utils::unpack<int>(msg,MAXLENGTH*SIZE,&pos,&instruction,1);
            utils::unpack<size_t>(msg,MAXLENGTH*SIZE,&pos,&specSize,1);
            utils::unpack<char>(msg,MAXLENGTH*SIZE,&pos,
                                &specification,specSize);
            string spec(specification);


            debugMasterController(instruction,spec);
            numberExpected--;

            if (numberExpected == 0 && okayToPauseSimulation)
                pauseSimulation();

            /*set up the variables and buffers for next message*/
            memset(specification,0,MAXLENGTH*SIZE);
            messageQueue->pop();
            memset(msg,0,MAXLENGTH*SIZE);
            pos = 0;
        }
    }

}
