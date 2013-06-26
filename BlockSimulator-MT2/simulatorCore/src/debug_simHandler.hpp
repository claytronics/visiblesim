
#ifndef DEBUG_SIMHANDLER_HPP
#define DEBUG_SIMHANDLER_HPP

#include <string>

using namespace std;

#define DUMP 1
#define CONTINUE 7
#define BREAKPOINT 2
#define NOTHING 8
#define PAUSE 4
#define UNPAUSE 3 
#define BREAKFOUND 6
#define PRINTCONTENT 5


#define FACTDER 1
#define FACTCON 2
#define FACTRET 3
#define ACTION 4
#define SENSE 5
#define BLOCK 6


string getType(string specification);
string getNode(string specification);
string getName(string specification);
string typeInt2String(int type);

#endif
