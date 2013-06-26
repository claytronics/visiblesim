
/*API TO HANDLE BREAKPOINTS, DUMPS, AND CONTINUES*/


#include <pthread.h>
#include <sstream>
#include <string>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>


#define FACTDER 1
#define FACTCON 2
#define FACTRET 3
#define ACTION 4
#define SENSE 5
#define BLOCK 6

using namespace std;



//returns the index of a character in a string, 
//if it is not there it returns -1
int characterInStringIndex(string str, char character){
  for(unsigned int i = 0; i < str.length(); i++){
    if (str[i] == character)
      return (int)i;
  }
  return -1;
}

//extracts the type from the specification
//returns the type of breakpoint from the specification
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


//extracts the name from the specification
//returns the name from the specification
//returns "" if name is not present
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


//extracts the node from the specification
//returns the node from the specification
//returns "" if node is not given
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

  

