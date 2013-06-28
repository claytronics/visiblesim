#ifndef DEBUG_SIMPROMPT_HPP
#define DEBUG_SIMPROMPT_HPP


void (*initDebugger(void (*sendMsg)(int,int,uint64_t*),
		    void (*pauseSim)(int),
		    void (*unPauseSim)(void),
		    ostream& o = cout, istream& i = cin))(uint64_t*);

#endif
