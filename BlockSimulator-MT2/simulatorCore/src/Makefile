TINYXMLSRCS = TinyXML/tinyxml.cpp TinyXML/tinyxmlerror.cpp TinyXML/tinyxmlparser.cpp
# Note : when using TIXML_USE_STL flag, TinyXML/tinystr.cpp produce no code, so I removed it from the source list as ar was complaining

BASESIMULATOR_SRCS = $(TINYXMLSRCS) simulator.cpp buildingBlock.cpp blockCode.cpp scheduler.cpp world.cpp network.cpp events.cpp glBlock.cpp interface.cpp openglViewer.cpp shaders.cpp vecteur.cpp matrice44.cpp camera.cpp objLoader.cpp vertexArray.cpp
BASESIMULATOR_OBJS = $(BASESIMULATOR_SRCS:.cpp=.o)
BASESIMULATOR_DEPS = $(BASESIMULATOR_SRCS:.cpp=.depends)

MULTICORES_SRCS = multiCoresSimulator.cpp multiCoresBlock.cpp multiCoresBlockCode.cpp multiCoresScheduler.cpp multiCoresWorld.cpp \
multiCoresEvents.cpp
MULTICORES_OBJS = $(MULTICORES_SRCS:.cpp=.o) $(BASESIMULATOR_OBJS)
MULTICORES_DEPS = $(MULTICORES_SRCS:.cpp=.depends) $(BASESIMULATOR_DEPS)

BLINKYBLOCKS_SRCS = blinkyBlocksSimulator.cpp blinkyBlocksBlock.cpp blinkyBlocksBlockCode.cpp blinkyBlocksScheduler.cpp blinkyBlocksWorld.cpp blinkyBlocksGlBlock.cpp blinkyBlocksEvents.cpp
BLINKYBLOCKS_OBJS = $(BLINKYBLOCKS_SRCS:.cpp=.o) $(BASESIMULATOR_OBJS)
BLINKYBLOCKS_DEPS = $(BLINKYBLOCKS_SRCS:.cpp=.depends) $(BASESIMULATOR_DEPS)

SMARTBLOCKS_SRCS = smartBlocksSimulator.cpp smartBlocksBlock.cpp smartBlocksBlockCode.cpp smartBlocksScheduler.cpp smartBlocksWorld.cpp smartBlocksGlBlock.cpp
SMARTBLOCKS_OBJS = $(SMARTBLOCKS_SRCS:.cpp=.o) $(BASESIMULATOR_OBJS)
SMARTBLOCKS_DEPS = $(SMARTBLOCKS_SRCS:.cpp=.depends) $(BASESIMULATOR_DEPS)


SRCS = $(BASESIMULATOR_SRCS) $(MULTICORES_SRCS) $(BLINKYBLOCKS_SRCS) $(SMARTBLOCKS_SRCS)
OBJS = $(SRCS:.cpp=.o)
DEPS = $(SRCS:.cpp=.depends)

OUT = libsimMultiCores.a libsimBlinkyBlocks.a libsimSmartBlocks.a

INCLUDES = $(GLOBAL_INCLUDES) -I. 

#for macOSX
#LIBS = -L./ -L/usr/local/lib -framework GLUT -framework OpenGL -L/usr/X11/lib /usr/local/lib/libglut.dylib -lboost_thread-mt

#for linux
#LIBS = -L./ -L/usr/local/lib  -L/usr/X11/lib -lboost_thread-mt
LIBS = $(GLOBAL_LIBS)

#for debug version
#CCFLAGS = -g -Wall -DTINYXML_USE_STL -DTIXML_USE_STL

#for production version
#CCFLAGS = -O2 -NDEBUG -Wall -DTINYXML_USE_STL -DTIXML_USE_STL

CCFLAGS = $(GLOBAL_CCFLAGS)

CC = g++


.PHONY: clean all

.cpp.o:
	$(CC) $(INCLUDES) $(CCFLAGS) -c $< -o $@

all: libsimMultiCores.a libsimBlinkyBlocks.a libsimSmartBlocks.a #$(OUT)

#$(OUT): $(OBJS)
#	ar rcs $(OUT) $(OBJS)
#	cp $(OUT) ../lib

libsimMultiCores.a: $(BASESIMULATOR_OBJS) $(MULTICORES_OBJS)
	ar rcs libsimMultiCores.a $(BASESIMULATOR_OBJS) $(MULTICORES_OBJS)
	cp libsimMultiCores.a ../lib

libsimBlinkyBlocks.a: $(BASESIMULATOR_OBJS) $(BLINKYBLOCKS_OBJS)
	ar rcs libsimBlinkyBlocks.a $(BASESIMULATOR_OBJS) $(BLINKYBLOCKS_OBJS)
	cp libsimBlinkyBlocks.a ../lib

libsimSmartBlocks.a: $(BASESIMULATOR_OBJS) $(SMARTBLOCKS_OBJS)
	ar rcs libsimSmartBlocks.a $(BASESIMULATOR_OBJS) $(SMARTBLOCKS_OBJS)
	cp libsimSmartBlocks.a ../lib

-include $(DEPS)

clean:
	rm -f *~ $(OBJS) $(OUT) $(DEPS) ../lib/*.a ../include/*.h
	
%.depends: %.cpp
	$(CC) -M $(CCFLAGS) $(INCLUDES) $< > $@
	
