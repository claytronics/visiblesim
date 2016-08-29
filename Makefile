#######################
#
# --- Tune this area to your needs ---
#
# You have to set GLOBAL_LIBS and GLOBAL_CFLAGS
# Those variables will then be sent to all the sub-makefiles
#
# two versions are proposed for each, you have to choose

OS = $(shell uname -s)

# VisibleSim local libraries
VSIM_LIBS = -lsimCatoms3D -lsimCatoms2D -lsimRobotBlocks -lsimBlinkyBlocks -lsimSmartBlocks -lsimMultiRobots

#for debug version
TEMP_CCFLAGS = -g -Wall -std=c++11 -DTINYXML_USE_STL -DTIXML_USE_STL

# ADDITIONAL CCFLAGS
# ================
#
# MeldProcess: Uncomment this block to enable
# TEMP_CCFLAGS += -DENABLE_MELDPROCESS # Enable MeldProcess source files compilation,
# 				       # required by any MeldProcess CodeBlock.
# 				       # if this flag is not set, the boost libraries will not be included,
# 				       # since they are the only source files using them.

# You can add those constant definitions to get a more verbose output
# -DDEBUG_EVENTS          :  trace creation and destruction of all events
# -DDEBUG_CONSUME_EVENTS  : trace the consomption of all events
# -DDEBUG_MESSAGES        :  trace creation and destruction of all messages inside the simulator
# -DDEBUG_VM_MESSAGES     : trace the messages sent to the multicores VM

#for production version
#TEMP_CCFLAGS = "-O3 -DNDEBUG -Wall -DTINYXML_USE_STL -DTIXML_USE_STL"

#for TEST VERSION
#TEMP_CCFLAGS = "-g -Wall -DTINYXML_USE_STL -DTIXML_USE_STL -DDEBUG_VM_MESSAGES -DTEST_DETER"

ifeq ($(OS),Darwin)
#MacOS (Assuming you are using the Clang compiler)
OSX_CCFLAGS = -DGL_DO_NOT_WARN_IF_MULTI_GL_VERSION_HEADERS_INCLUDED -Wno-deprecated-declarations -Wno-overloaded-virtual

ifneq ($(filter -DENABLE_MELDPROCESS, $(TEMP_CCFLAGS)),)
INC_BOOST_IF_NEEDED = -lboost_thread-mt  -lboost_system-mt -lboost_chrono-mt
endif

GLOBAL_LIBS = "-L./ -L/usr/local/lib -lGLEW -lglut -framework GLUT -framework OpenGL -L/usr/X11/lib /usr/local/lib/libglut.dylib $(VSIM_LIBS) $(INC_BOOST_IF_NEEDED)"

else

#Linux, Solaris, ... (gcc)
ifneq ($(filter -DENABLE_MELDPROCESS, $(TEMP_CCFLAGS)),)
INC_BOOST_IF_NEEDED = -lboost_thread -lboost_system -lboost_chrono
endif

GLOBAL_LIBS = "-L./ -L/usr/local/lib -L/usr/X11/lib $(VSIM_LIBS) -lglut -lGL -lGLEW -lGLU -lpthread $(INC_BOOST_IF_NEEDED)"

endif

GLOBAL_CCFLAGS = "$(TEMP_CCFLAGS) $(OSX_CCFLAGS)"

#
# --- End of tunable area ---
#
########################


SUBDIRS = simulatorCore/src applicationsSrc

GLOBAL_INCLUDES = "-I/usr/local/include -I/opt/local/include -I/usr/X11/include"

.PHONY: subdirs $(SUBDIRS) test doc
#.PHONY: subdirs $(SUBDIRS) test doc

subdirs: $(SUBDIRS)

$(SUBDIRS):
	@$(MAKE) -C $@ APPDIR=../../applicationsBin/ GLOBAL_INCLUDES=$(GLOBAL_INCLUDES) GLOBAL_LIBS=$(GLOBAL_LIBS) GLOBAL_CCFLAGS=$(GLOBAL_CCFLAGS)

#subdirs:
#	@for dir in $(SUBDIRS); do \
#	$(MAKE) -C $$dir; \
#	done

test: subdirs
	@$(MAKE) -C applicationsSrc test;

doc: 	
	@$(MAKE) -C doc;
clean:
	rm -f *~ *.o
	@for dir in $(SUBDIRS); do \
	$(MAKE) -C $$dir APPDIR=../../applicationsBin clean; \
	done

realclean: clean
	$(MAKE) -C applicationsSrc APPDIR=../../applicationsBin realclean; \
