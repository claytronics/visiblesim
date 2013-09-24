visiblesim
==========

Dependencies:
 - Boost C++ Libraries >= 1.47.0
 - GLUT
 - GLEW

On Ubuntu 12.04:
 - libboost-all-dev
 - freeglut3-dev
 - libglew1.6-dev
 

## Mac Installation

 1. Install dependencies:

```
brew install boost --with-mpi --without-single
brew install freeglut
brew install glew
```

 2. Uncomment mac-specific GLOBAL_LIBS in BlockSimulator-MT2/Makefile
 3. make

## Running the simulator

To execute meld programs on the simulator, 
set vmPath and programPath in BlockSimulator-MT2/applicationsBin/blinky01/config.xml to point to 
your vmPath and the program you want to run.

Then `cd` into `BlockSimulator-MT2/applicationsBin/blinky01/` and run 
```
./blinky01 config.xml
```

The simulator window should appear and the console should read:
```
SIMULATION DEBUGGING MODE -- type help for options
>
```

**NOTE: you must press '<shift>-r' in the simulator window before you can start running commands from the console.**

After pressing '<shift>-r' in the simulation window, typing `run` in the console should start your program.
