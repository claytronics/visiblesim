/*
 * openglViewer.h
 *
 *  Created on: 21/02/2012
 *      Author: ben
 */

#ifndef OPENGLVIEWER_H_
#define OPENGLVIEWER_H_

#include "shaders.h"

#include <pthread.h>
#include "matrice44.h"
#include "camera.h"
#include "glBlock.h"
#include "interface.h"
class GlutContext;

//===========================================================================================================
//
//          GlutContext  (class)
//
//===========================================================================================================
class GlutContext {
	static int screenWidth, screenHeight;
	static int keyboardModifier;
	public :
    static GlutSlidingMainWindow *mainWindow;
    static GlutPopupWindow *popup;
//	bool showLinks;

	static void init(int argc, char **argv);
	static void deleteContext();
	static void mainLoop(void);
private :
    static void reshapeFunc(int w,int h);
    static void passiveMotionFunc(int x,int y);
    static void motionFunc(int x,int y);
    static void mouseFunc(int button,int state,int x,int y);
	static void keyboardFunc(unsigned char c, int x, int y);
	static void drawFunc(void);
	static void idleFunc(void);
	static int selectFunc(int x,int y);
	static int processHits(GLint hits, GLuint *buffer);
	
	static void *lanceScheduler(void *param);
};



#endif 
