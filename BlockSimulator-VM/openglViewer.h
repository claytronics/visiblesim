/*
 * openglViewer.h
 *
 *  Created on: 21/02/2012
 *      Author: ben
 */

#ifndef OPENGLVIEWER_H_
#define OPENGLVIEWER_H_

#include <pthread.h>
#include "Matrice44.h"
#include "Camera.h"
#include "Blocs.h"
#include "interface.h"
#include "unistd.h"
class GlutContext;


//===========================================================================================================
//
//          GrilleSupport  (class)
//
//===========================================================================================================

class GrilleSupport {
private:
	static vector<Bloc*> listeBlocs;
public:
	static Cellule *tableauCellules;
	static int tailleX, tailleY;
	static GLfloat tailleBlockX, tailleBlockY, tailleBlockZ;
	static GLfloat blockSize[3];
	static GLuint idTextureBlock;
	static GLuint idTextureFloor;

	static void initGrilleSupport(int largeur, int hauteur);
	static void demarreCodeBloc(Message *msg=NULL);
	static bool ajouteBloc(Bloc *bloc);
	static void glDraw();
	static void glDrawId();
	static Cellule *getCellule(int x, int y);
	static void highlightBlockNumber(int n);
	static Bloc* getBlockFromId(int n);
};

//===========================================================================================================
//
//          OpenglViewer  (class)
//
//===========================================================================================================

class OpenglViewer {
public:
	Camera *camera;
	GLfloat lightDirectionVector[4];

	OpenglViewer(int argc, char **argv);
	~OpenglViewer();
	void visualise();

protected :
	pthread_t threadScheduler;
	char comment[128];
	
	void glDraw(void);
	void glDrawId(void);

	friend class GlutContext;
};

//===========================================================================================================
//
//          GlutContext  (class)
//
//===========================================================================================================
class GlutContext {
	static int screenWidth, screenHeight;
	static int keyboardModifier;
	static OpenglViewer *objectToDraw;
public :
    static GlutSlidingMainWindow *mainWindow;
	static bool showLinks;

	static void init(int argc, char **argv,OpenglViewer *o2d);
	static void deleteContext();
private :
    static void reshapeFunc(int w,int h);
	static void motionFunc(int x,int y);
	static void mouseFunc(int button,int state,int x,int y);
	static void keyboardFunc(unsigned char c, int x, int y);
	static void drawFunc(void);
	static void idleFunc(void);
	static void selectFunc(int x,int y);
	static int processHits(GLint hits, GLuint *buffer);
	
	static void *lanceScheduler(void *param);

};



#endif 
