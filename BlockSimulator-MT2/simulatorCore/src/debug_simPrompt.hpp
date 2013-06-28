#ifndef DEBUG_SIMPROMPT_HPP
#define DEBUG_SIMPROMPT_HPP


void (*initDebugger(int (*sendMsg)(int,int,uint64_t*),
		    void (*pauseSim)(int),
		    void (*unPauseSim)(void),
		    void (*quit)(void),
		    ostream& o = cout, istream& i = cin))(uint64_t*);

#endif
