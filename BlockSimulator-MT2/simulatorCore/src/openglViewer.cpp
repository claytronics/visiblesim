/*
 * openglViewer.cpp
 *
 *  Created on: 21/02/2012
 *      Author: Ben
 */

#include "openglViewer.h"
#include "world.h"
#include "scheduler.h"

//===========================================================================================================
//
//          GlutContext  (class)
//
//===========================================================================================================

int GlutContext::screenWidth = 1024;
int GlutContext::screenHeight = 800;
int GlutContext::keyboardModifier = 0;
//bool GlutContext::showLinks=false;
GlutSlidingMainWindow *GlutContext::mainWindow=NULL;
GlutPopupWindow *GlutContext::popup=NULL;

void GlutContext::init(int argc, char **argv) {

	cout << "Avant glutInit()" << endl;
	glutInit(&argc,argv);
	cout << "Après glutInit()" << endl;
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE,GLUT_ACTION_CONTINUE_EXECUTION);
	cout << "Après glutSetOption()" << endl;
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);

	// creation of a new graphic window
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(screenWidth,screenHeight);
	if (glutCreateWindow("OpenGL BlockSimulator") == GL_FALSE) {
		puts("ERREUR : echec à la création de la fenêtre graphique");
		exit(EXIT_FAILURE);
	}

	//glutFullScreen();
	initShaders();

	////// GL parameters /////////////////////////////////////
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_NORMALIZE);

	glClearColor(0.3f,0.3f,0.8f,0.0f);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);


	glutReshapeFunc(reshapeFunc);
	glutDisplayFunc(drawFunc);
	glutMouseFunc(mouseFunc);
	glutMotionFunc(motionFunc);
	glutPassiveMotionFunc(passiveMotionFunc);
	glutKeyboardFunc(keyboardFunc);
	glutIdleFunc(idleFunc);

	mainWindow = new GlutSlidingMainWindow(screenWidth-40,50,40,screenHeight-60,"../../simulatorCore/smartBlocksTextures/fenetre_onglet.tga");
	popup = new GlutPopupWindow(NULL,0,0,40,30);
}

void GlutContext::deleteContext() {
	delete mainWindow;
	delete popup;
}

void *GlutContext::lanceScheduler(void *param) {
	int mode = *(int*)param;
	BaseSimulator::getScheduler()->start(mode);
	return(NULL);
}

//////////////////////////////////////////////////////////////////////////////
// fonction de changement de dimensions de la fenetre,
// - width  : largeur (x) de la zone de visualisation
// - height : hauteur (y) de la zone de visualisation
void GlutContext::reshapeFunc(int w,int h) {
	screenWidth=w;
	screenHeight=h;
	Camera* camera=getWorld()->getCamera();
	camera->updateIntrinsics(60,double(w)/double(h),1.0,1500.0);
	// size of the OpenGL drawing area
	glViewport(0,0,w,h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	camera->glProjection();
	// camera intrinsic parameters
	//gluPerspective(60,(double)w/(double)h,10.0,5000.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	mainWindow->reshapeFunc(w,h);
}

//////////////////////////////////////////////////////////////////////////////
// fonction associée aux interruptions générées par la souris bouton pressé
// - x,y : coordonnée du curseur dans la fenêtre
void GlutContext::motionFunc(int x,int y) {
	if (keyboardModifier!=GLUT_ACTIVE_CTRL) { // rotation du point de vue
		Camera* camera=getWorld()->getCamera();
		camera->mouseMove(x,y);
	}
}

void GlutContext::passiveMotionFunc(int x,int y) {
	int n=selectFunc(x,y);
	if (n) {
		GlBlock *slct=BaseSimulator::getWorld()->getBlockByNum(n-1);
		popup->setCenterPosition(x,screenHeight - y);
		popup->setInfo(slct->getPopupInfo());
		popup->show(true);
	} else {
		popup->show(false);
	}
}
//////////////////////////////////////////////////////////////////////////////
// fonction associée aux interruptions générées par le clic de souris
// - bouton : code du bouton
// - state : état des touches du clavier
// - x,y : coordonnée du curseur dans la fenêtre
void GlutContext::mouseFunc(int button,int state,int x,int y) {
	mainWindow->mouseFunc(button,state,x,screenHeight - y);
	keyboardModifier = glutGetModifiers();
	if (keyboardModifier!=GLUT_ACTIVE_CTRL) { // rotation du point de vue
		Camera* camera=getWorld()->getCamera();
		switch (button) {
			case GLUT_LEFT_BUTTON:
				if (state==GLUT_DOWN) {
					camera->mouseDown(x,y);
				} else
				if (state==GLUT_UP) {
					camera->mouseUp(x,y);
				}
			break;
			case GLUT_RIGHT_BUTTON:
				if (state==GLUT_DOWN) {
					camera->mouseDown(x,y,true);
				} else
				if (state==GLUT_UP) {
					camera->mouseUp(x,y);
				}
			break;
			case 3 :
				camera->mouseZoom(-10);
			break;
			case 4 :
				camera->mouseZoom(10);
			break;
		}
	} else { // selection of the clicked block
		if (state==GLUT_DOWN) {
			int n=selectFunc(x,y);
		 	if (n) {
		  		GlBlock *slct=BaseSimulator::getWorld()->getSelectedBlock();
		  		if (slct) {
		  			slct->toggleHighlight();
		  		}
		  		BaseSimulator::getWorld()->setSelectedBlock(n-1)->toggleHighlight();
		  	}
		}
	}
	glutPostRedisplay();
}

/////////////////////////////////////run/////////////////////////////////////////
// fonction associée aux interruptions clavier
// - c : caractère saisi
// - x,y : coordonnée du curseur dans la fenètre
void GlutContext::keyboardFunc(unsigned char c, int x, int y)
{
	static int modeScheduler;
	Camera* camera=getWorld()->getCamera();

	switch(c)
    { case 27 : case 'q' : case 'Q' : // quit
        glutLeaveMainLoop();
      break;
      case 'f' : glPolygonMode(GL_FRONT_AND_BACK,GL_LINE); break;
      case 'F' : glPolygonMode(GL_FRONT_AND_BACK,GL_FILL); break;
      case '+' : camera->mouseZoom(0.5); break;
      case '-' : camera->mouseZoom(-0.5); break;
	//  case 'l' : showLinks = !showLinks; break;
      case 'r' : getScheduler()->start(SCHEDULER_MODE_REALTIME); break;
	  case 'R' : getScheduler()->start(SCHEDULER_MODE_FASTEST); break;
	  case 'z' : {
		  World *world = BaseSimulator::getWorld();
		  GlBlock *slct=world->getSelectedBlock();
		  if (slct) {
			  world->getCamera()->setTarget(slct->getPosition());
		  }
	  }
	  break;
    }

  glutPostRedisplay();
}


//////////////////////////////////////////////////////////////////////////////
// fonction de mise à jour des données pour l'animation
void GlutContext::idleFunc(void) {
#ifdef _WIN32
	  Sleep(20);
#else
	  usleep(20000);
#endif
	glutPostRedisplay();
}

void GlutContext::drawFunc(void) {
	Camera*camera=getWorld()->getCamera();

	shadowedRenderingStep1(camera);
	glPushMatrix();
	BaseSimulator::getWorld()->glDraw();
	glPopMatrix();
	
	shadowedRenderingStep2(screenWidth,screenHeight);

	shadowedRenderingStep3(camera);
	glPushMatrix();
	BaseSimulator::getWorld()->glDraw();
	glPopMatrix();
  
	shadowedRenderingStep4();

	// drawing of the interface
	glEnable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	gluOrtho2D(0,screenWidth,0,screenHeight);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	mainWindow->glDraw();
	popup->glDraw();

	glEnable(GL_DEPTH_TEST);
	glutSwapBuffers();
}

//////////////////////////////////////////////////////////////////////////////
// fonction de détection d'un objet à l'écran en position x,y.
int GlutContext::selectFunc(int x,int y) {
	GLuint selectBuf[512];
	GLint hits;
	GLint viewport[4];
	Camera* camera=getWorld()->getCamera();

	glGetIntegerv(GL_VIEWPORT,viewport); // récupération de la position et de la taille de la fenêtre

	glSelectBuffer(512,selectBuf);
	glRenderMode(GL_SELECT);
	glInitNames();
	glPushName(0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPickMatrix((float) x,(float)(screenHeight-y),3.0,3.0,viewport);
	camera->glProjection();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	camera->glLookAt();

	glPushMatrix();
  	BaseSimulator::getWorld()->glDrawId();
  	glPopMatrix();

  	glFlush();
  	hits = glRenderMode(GL_RENDER);
  	return processHits(hits,selectBuf);
 }


//////////////////////////////////////////////////////////////////////////////
// recherche du premier élément dans le tableau d'objet cliqués
// tableau d'entiers : { [name,zmin,zmax,n],[name,zmin,zmax,n]...}
int GlutContext::processHits(GLint hits, GLuint *buffer) {
  if (hits==0)
  { return 0;
  }
  GLuint *ptr=buffer;
  GLuint nmini = ptr[3];
  GLuint zmini = ptr[1];
  ptr+=4;
  for (int i=1; i<hits; i++)
  { if (ptr[1]<zmini)
    { zmini = ptr[1];
      nmini = ptr[3];
    }
    ptr+=4;
  }
  // traitement d'une selection
  // nmini contient le numéro de l'élément sélectionné
  // celui de z minimum
  return nmini;
}

void GlutContext::mainLoop() {
	glutMainLoop();
	deleteContext();
}
