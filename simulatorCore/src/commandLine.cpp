
#include "commandLine.h"

#include <iostream>
#include <cstdlib>

#include "statsIndividual.h"
#include "openglViewer.h"
#include "simulator.h"
#include "trace.h"

void CommandLine::help() {
    cerr << "VisibleSim options:" << endl;
    cerr << "\t -f \t\t\tfull screen" << endl;
    cerr << "\t -p <name>\t\tprogram file (Meld for instance)" << endl;
    cerr << "\t -D \t\t\tdebugging mode (used in Meld only)" << endl;
    cerr << "\t -c <name>\t\txml configuration file" << endl;
    cerr << "\t -r \t\t\trun realtime mode on startup" << endl;
    cerr << "\t -R \t\t\trun fastest mode on startup" << endl;
    cerr << "\t -x \t\t\tterminate simulation when scheduler ends (Graphical Mode only)" << endl;
    cerr << "\t -t \t\t\tterminal mode only (no graphical output)" << endl;
    cerr << "\t -s [<maximumDate> | inf] \tScheduler mode:\tBy default, stops when event list is empty\n"
         << "\t\t maximumDate (us) : the scheduler will stop when even list is empty, or when the maximum date is reach\n"
         << "\t\t inf : the scheduler will have an infinite duration and can only be stopped by the user" << endl;
    cerr << "\t -m <VMpath>:<VMport>\tpath to the MeldVM directory and port" << endl;
    cerr << "\t -k {\"BB\", \"RB\", \"SB\", \"C2D\", \"C3D\", \"MR\"}\t module type for generic Block Code execution" << endl;
    cerr << "\t -g \t\t\tEnable regression testing (export terminal configuration)" << endl;
    cerr << "\t -l \t\t\tEnable printing of log information to file simulation.log" << endl;
    cerr << "\t -i \t\t\tEnable printing more detailed simulation stats" << endl;
    cerr << "\t -a <seed>\t\tSet simulation seed" << endl;
    cerr << "\t -h \t\t\thelp" << endl;
    exit(EXIT_SUCCESS);
}

CommandLine::CommandLine(int argc, char *argv[]) {
	read(argc,argv);
}

void CommandLine::read(int argc, char *argv[]) {    
    /* Reading the command line */
    argv++;
    argc--;    
    while ( (argc > 0) && (argv[0][0] == '-')) {
        switch(argv[0][1]) {
            case 'p':   {
                //if (programPath != "")
                //   help();
                if (argc < 1) {
                    cerr << "error: No meld program provided after -p" << endl;
                    help();
                }
                programPath = argv[1];
                argc--;
                argv++;
            } break;
            case 'm': {             // MeldVM Path and Port
                stringstream vm(argv[1]); // <vmPath:vmPort>
                cerr << &argv[1] << endl;
                string portStr;
                std::getline(vm, vmPath, ':');
                std::getline(vm, portStr, ':');
                try {
                    vmPort = stoi(portStr);
                } catch(std::invalid_argument&) {                
                    cerr << "error: MeldVM port must be a number!" << endl;
                    help();
                    exit(EXIT_FAILURE);
                }
                argc--;
                argv++;            
            } break;
            case 'D': {
                meldDebugger = true;
            } break;
            case 'r': {
                if (schedulerMode == CMD_LINE_UNDEFINED)
                    schedulerMode = SCHEDULER_MODE_REALTIME;
                else {
                    cerr << "error: -r and -R options cannot be enabled at the same time" << endl;
                    help();
                }
                
            } break;
            case 'R': {
                if (schedulerMode == CMD_LINE_UNDEFINED)                           
                    schedulerMode = SCHEDULER_MODE_FASTEST;
                else {
                    cerr << "error: -r and -R options cannot be enabled at the same time" << endl;
                    help();
                }
            } break;
            case 'x': {
                schedulerAutoStop = true;
            } break;
            case 'c': {
                // Configuration file, already managed in Simulator constructor
                if (argc < 1) {
                    cerr << "error: No configuration file provided after -c" << endl;
                    help();
                }
                configFile= argv[1];
                argc--;
                argv++;
            } break;
            case 's': {
                schedulerLength = SCHEDULER_LENGTH_BOUNDED;
                try {
                    maximumDate = stoull(argv[1]);
                } catch(std::invalid_argument&) {
                    if (strcmp(argv[1], "inf") == 0) {
                        schedulerLength = SCHEDULER_LENGTH_INFINITE;
                    } else {
                        cerr << "error: Found unknown parameter after option -s. Expected <MaximumDate> or \"inf\""
                             << endl;
                        help();
                        exit(EXIT_FAILURE);
                    }
                } catch(std::out_of_range&) {                    
                        cerr << "error: Maximum Date must be an integer and smaller than (2^63 - 1)!" << endl;
                        help();
                        exit(EXIT_FAILURE);
                }
                
                argc--;
                argv++;
            } break;
            case 't': {
                GlutContext::GUIisEnabled = false;
            } break;
            case 'f' : {
                //fullScreen = true;
                GlutContext::setFullScreenMode(true);
            } break;
            case 'k' : {
                // Already handled by meld blockCode, nothing to do
                argc--;
                argv++;
            } break;
            case 'l' : {
                log_file.open("simulation.log");
            } break;
            case 'g' : {
                Simulator::regrTesting = true;
            } break;
            case 'i' : {
                utils::StatsIndividual::enable = true;
            } break;
            case 'a' : {
                string str(argv[1]);
                try {
                    simulationSeed = stoi (str);
                    simulationSeedSet = true;
                } catch(std::invalid_argument&) {                
                    cerr << "error: Simulation seed must be an integer!" << endl;
                    help();
                    exit(EXIT_FAILURE);
                } catch(std::out_of_range&) {
                    cerr << "error: Simulation seed is out of range!" << endl;
                    help();
                    exit(EXIT_FAILURE);
                }            

                argc--;
                argv++;      
            } break;
            default:
                help();
        }
        argc--;
        argv++;
    }    
}

bool CommandLine::randomWorldRequested() {
    return topology != CMD_LINE_UNDEFINED;
}

ModuleType CommandLine::readModuleType(int argc, char **argv) {
    // Locate -k command line argument
    for (int i = 0; i < argc; i++) {
        if (argv[i][0] == '-' && argv[i][1] == 'k') {
            if (!argv[i+1]) break;
            
            if (strcmp(argv[i+1], "BB") == 0) return BB;
            else if (strcmp(argv[i+1], "RB") == 0) return RB;
            else if (strcmp(argv[i+1], "SB") == 0) return SB;
            else if (strcmp(argv[i+1], "C2D") == 0) return C2D;
            else if (strcmp(argv[i+1], "C3D") == 0) return C3D;
            else if (strcmp(argv[i+1], "MR") == 0) return MR;
            else {
                cerr << "error: unknown module type: " << argv[i+1] << endl;
                exit(EXIT_FAILURE);
            }
        }
    }

    // Did not find it
    cerr << "error: module type for generic Block Code execution not provided: -k"
         << " {\"BB\", \"RB\", \"SB\", \"C2D\", \"C3D\", \"MR\"}\t" << endl;
    exit(EXIT_FAILURE);
}
