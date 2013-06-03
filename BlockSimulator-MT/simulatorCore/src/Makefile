TINYXMLSRCS = TinyXML/tinyxml.cpp TinyXML/tinyxmlerror.cpp TinyXML/tinyxmlparser.cpp
# Note : when using TIXML_USE_STL flag, TinyXML/tinystr.cpp produce no code, so removed it from the source list as ar was complaining

SRCS = $(TINYXMLSRCS) simulator.cpp multiCoresSimulator.cpp world.cpp multiCoresWorld.cpp buildingBlock.cpp multiCoresBlock.cpp \
blockCode.cpp multiCoresBlockCode.cpp events.cpp scheduler.cpp message.cpp
OBJS = $(SRCS:.cpp=.o)
DEPS = $(SRCS:.cpp=.depends)




OUT = libsimMultiCores.a

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

%.depends: %.cpp
	$(CC) -M $(CCFLAGS) $(INCLUDES) $< > $@

all: $(OUT)

$(OUT): $(OBJS)
	ar rcs $(OUT) $(OBJS)
	cp $(OUT) ../lib

-include $(DEPS)

clean:
	rm -f *~ $(OBJS) $(OUT) $(DEPS) ../lib/*.a ../include/*.h