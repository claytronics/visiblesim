APPLICATIONSINCLUDE = 	-Iapplications/mouvementsAleatoires \
						-Iapplications/demoMouvementsGroupe \
						-Iapplications/demoLocalTimer \
						-Iapplications/agents01 \
						-Iapplications/demoMobileMotion \
						-Iapplications/flavio01
	
CXX = g++

CXXFLAGS = -Wall -g  -I/usr/X11/include -I. $(APPLICATIONSINCLUDE) -ITinyXML
#CXXFLAGS = -I/usr/X11/include -I. $(APPLICATIONSINCLUDE) -ITinyXML

#LDFLAGS = -lm -L/usr/X11/lib -framework GLUT -framework OpenGL /usr/local/lib/libglut.dylib
LDFLAGS = -lglut -lGLU -lGL -lXext -lX11 -lpthread -lm 

TARGET=BlockSimulator
#TARGET=BlockSimulator
SRCS=BlockSimulator.cpp Blocs.cpp Camera.cpp CodeBloc.cpp Evenements.cpp Matrice44.cpp \
	Message.cpp physics.cpp Scheduler.cpp Vecteur.cpp glBlock.cpp interface.cpp openglViewer.cpp \
	TinyXML/tinystr.cpp TinyXML/tinyxmlerror.cpp TinyXML/tinyxmlparser.cpp TinyXML/tinyxml.cpp \
	applications/mouvementsAleatoires/CodeBlocMouvementsAleatoires.cpp \
	applications/demoMouvementsGroupe/CodeBlocDemoMouvementsGroupes.cpp \
	applications/demoLocalTimer/CodeBlocDemoLocalTimer.cpp \
	applications/agents01/CodeBlocAgents01.cpp \
	applications/demoMobileMotion/CodeBlocDemoMobileMotion.cpp \
	applications/flavio01/CodeBlocFlavio01.cpp

OBJS = $(SRCS:.cpp=.o)
DEPS = $(SRCS:.cpp=.depends)

.PHONY: clean all

all: $(TARGET) VMDrivingTest

$(TARGET): $(OBJS)
#	$(CXX) $(LDFLAGS) $(OBJS) -o $(TARGET)
	$(CXX) $(OBJS) -o $(TARGET) $(LDFLAGS)

.cpp.o:
	$(CXX) $(CXXFLAGS) -c $< -o $@

%.depends: %.cpp
	$(CXX) -M $(CXXFLAGS) $< > $@

-include $(DEPS)

VMDrivingTest: VMDrivingTest.cpp
	$(CXX) VMDrivingTest.cpp -o VMDrivingTest

clean:
	rm -f $(OBJS) $(DEPS) $(TARGET)

