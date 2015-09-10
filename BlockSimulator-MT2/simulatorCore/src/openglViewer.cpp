/*
 * openglViewer.cpp
 *
 *  Created on: 21/02/2012
 *      Author: Ben
 */

#include "openglViewer.h"
#include "world.h"
#include "scheduler.h"
#include "events.h"
#include "trace.h"
#include "meldProcessDebugger.h"

//===========================================================================================================
//
//          GlutContext  (class)
//
//===========================================================================================================

int GlutContext::screenWidth = 1024;
int GlutContext::screenHeight = 800;
int GlutContext::initialScreenWidth = 1024;
int GlutContext::initialScreenHeight = 800;
int GlutContext::keyboardModifier = 0;
int GlutContext::lastMotionTime=0;
int GlutContext::lastMousePos[2];
//bool GlutContext::showLinks=false;
bool GlutContext::fullScreenMode=false;
bool GlutContext::saveScreenMode=false;
bool GlutContext::mustSaveImage=false;
GlutSlidingMainWindow *GlutContext::mainWindow=NULL;
GlutPopupWindow *GlutContext::popup=NULL;
GlutPopupMenuWindow *GlutContext::popupMenu=NULL;
GlutHelpWindow *GlutContext::helpWindow=NULL;

void GlutContext::init(int argc, char **argv) {
	glutInit(&argc,argv);
	glutSetOption(GLUT_ACTION_ON_WINDOW_CLOSE,GLUT_ACTION_CONTINUE_EXECUTION);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);

	// creation of a new graphic window
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(screenWidth,screenHeight);
	if (glutCreateWindow("OpenGL BlockSimulator") == GL_FALSE) {
		puts("ERREUR : echec à la création de la fenêtre graphique");
		exit(EXIT_FAILURE);
	}

    if(fullScreenMode) {
		glutFullScreen();
	}
    
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
	delete popupMenu;
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
	camera->setW_H(double(w)/double(h));
	// size of the OpenGL drawing area
	glViewport(0,0,w,h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	camera->glProjection();
	// camera intrinsic parameters
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	mainWindow->reshapeFunc(w,h);
}

//////////////////////////////////////////////////////////////////////////////
// fonction associée aux interruptions générées par la souris bouton pressé
// - x,y : coordonnée du curseur dans la fenêtre
void GlutContext::motionFunc(int x,int y) {
	if (popup->isShown()) {
		glutPostRedisplay();
		popup->show(false);
	}
	if (mainWindow->mouseFunc(-1,GLUT_DOWN,x,screenHeight - y)>0) return;
	if (keyboardModifier!=GLUT_ACTIVE_CTRL) { // rotation du point de vue
		Camera* camera=getWorld()->getCamera();
		camera->mouseMove(x,y);
		glutPostRedisplay();
	}
}

void GlutContext::passiveMotionFunc(int x,int y) {
	if (popup->isShown()) {
		glutPostRedisplay();
		popup->show(false);
	}
	if (helpWindow && helpWindow->passiveMotionFunc(x,screenHeight - y)) {
		glutPostRedisplay();
		return;
	}
	if (popupMenu && popupMenu->passiveMotionFunc(x,screenHeight - y)) {
		glutPostRedisplay();
		return;
	}
	if (mainWindow->passiveMotionFunc(x,screenHeight - y)) {
		glutPostRedisplay();
		return;
	}
	lastMotionTime = glutGet(GLUT_ELAPSED_TIME);
	lastMousePos[0]=x;
	lastMousePos[1]=y;
}

//////////////////////////////////////////////////////////////////////////////
// fonction associée aux interruptions générées par le clic de souris
// - bouton : code du bouton
// - state : état des touches du clavier
// - x,y : coordonnée du curseur dans la fenêtre
void GlutContext::mouseFunc(int button,int state,int x,int y) {
	if (mainWindow->mouseFunc(button,state,x,screenHeight - y)>0) {
		glutPostRedisplay();
		return;
	}
	if (popupMenu) {
		int n=popupMenu->mouseFunc(button,state,x,screenHeight - y);
		if (n) {
			popupMenu->show(false);
			getWorld()->menuChoice(n);
		}
	}
	if (helpWindow) helpWindow->mouseFunc(button,state,x,screenHeight - y);

	keyboardModifier = glutGetModifiers();
	if (keyboardModifier!=GLUT_ACTIVE_CTRL) { // rotation du point de vue
		Camera* camera=getWorld()->getCamera();
		switch (button) {
			case GLUT_LEFT_BUTTON:
				if (state==GLUT_DOWN) {
					camera->mouseDown(x,y);
					int n=selectFunc(x,y);
					GlBlock *glb = BaseSimulator::getWorld()->getBlockByNum(n-1);
					GlBlock *glbs = BaseSimulator::getWorld()->getSelectedBlock();
					if (glbs != NULL && glbs == glb) {
						BaseSimulator::getWorld()->tapBlock(BaseSimulator::getScheduler()->now(), glb->blockId);
					}
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
		//cout << *camera << endl;
	} else { // selection of the clicked block
		if (state==GLUT_UP) {
			if (button==GLUT_LEFT_BUTTON) {
				int n=selectFunc(x,y);
				GlBlock *slct=BaseSimulator::getWorld()->getSelectedBlock();
				// unselect current if exists
				if (slct) slct->toggleHighlight();
				// set n-1 block selected block (no selected block if n=0
				if (n) BaseSimulator::getWorld()->setSelectedBlock(n-1)->toggleHighlight();
				else BaseSimulator::getWorld()->setSelectedBlock(-1);
				mainWindow->select(BaseSimulator::getWorld()->getSelectedBlock());
		  	} else if (button==GLUT_RIGHT_BUTTON) {
				int n=selectFaceFunc(x,y);
				if (n) {
					BaseSimulator::getWorld()->setSelectedFace(n-1);
					BaseSimulator::getWorld()->createPopupMenu(x,y);
				}
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
  //  static int modeScheduler;
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
      //case 'p' : getScheduler()->pauseSimulation(getScheduler()->now()); break;
	  case 'p' : MeldProcess::getDebugger()->handlePauseRequest(); break;
	  case 'R' : getScheduler()->start(SCHEDULER_MODE_FASTEST); break;
	  case 'u' : MeldProcess::getDebugger()->unPauseSim(); break;
	  case 'z' : {
		  World *world = BaseSimulator::getWorld();
		  GlBlock *slct=world->getSelectedBlock();
		  if (slct) {
			  world->getCamera()->setTarget(slct->getPosition());
		  }
	  }
	  break;
	  case 'w' : case 'W' :
          fullScreenMode = !fullScreenMode;
          if (fullScreenMode) {
        	  glutFullScreen();
          } else {
              glutReshapeWindow(initialScreenWidth,initialScreenHeight);
              glutPositionWindow(0,0);
          }
      break;
	  case 'h' :
		  if (!helpWindow) {
			  BaseSimulator::getWorld()->createHelpWindow();
		  }
		  helpWindow->showHide();
	  break;
	  case 's' : saveScreenMode=!saveScreenMode;
	  break;
	  case 'S' : saveScreen("capture.ppm");
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
	if (saveScreenMode && mustSaveImage) {
		static int num=0;
		char title[16];
		sprintf(title,"save%04d.ppm",num++);
		saveScreen(title);
		mustSaveImage=false;
	}
	if (lastMotionTime) {
			int tm = glutGet(GLUT_ELAPSED_TIME);
			if (tm-lastMotionTime>100) {
			int n=selectFunc(lastMousePos[0],lastMousePos[1]);
			if (n) {
				GlBlock *slct=BaseSimulator::getWorld()->getBlockByNum(n-1);
				popup->setCenterPosition(lastMousePos[0],screenHeight - lastMousePos[1]);
				popup->setInfo(slct->getPopupInfo());
				popup->show(true);
			} else {
				popup->show(false);
			}
			lastMotionTime=0;
		 	glutPostRedisplay();
		}
	}
	if (mainWindow->hasSelectedBlock() || getScheduler()->state==Scheduler::RUNNING) {
		glutPostRedisplay(); // for blinking
	}
}

void GlutContext::drawFunc(void) {
	World *wrl = getWorld();
	Camera*camera=wrl->getCamera();

	shadowedRenderingStep1(camera);
	glPushMatrix();
	wrl->glDraw();
	glPopMatrix();

	shadowedRenderingStep2(screenWidth,screenHeight);

	shadowedRenderingStep3(camera);
	glPushMatrix();
	wrl->glDraw();
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
	if (popupMenu) popupMenu->glDraw();
	if (helpWindow) helpWindow->glDraw();
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
// fonction de détection d'un objet à l'écran en position x,y.
int GlutContext::selectFaceFunc(int x,int y) {
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
  	BaseSimulator::getWorld()->glDrawIdByMaterial();
  	glPopMatrix();

  	glFlush();
  	hits = glRenderMode(GL_RENDER);
  	return processHits(hits,selectBuf);
 }

//////////////////////////////////////////////////////////////////////////////
// recherche du premier élément dans le tableau d'objet cliqués
// tableau d'entiers : { [name,zmin,zmax,n],[name,zmin,zmax,n]...}
int GlutContext::processHits(GLint hits, GLuint *buffer) {
  if (hits==0) {
	  return 0;
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
	getScheduler()->stop(BaseSimulator::getScheduler()->now());
	deleteContext();
}

void GlutContext::addTrace(const string &message,int id,const Color &color) {
	if (mainWindow) mainWindow->addTrace(id,message,color);
}

bool GlutContext::saveScreen(char *title) {
#ifdef WIN32
	FILE *fichier;
	fopen_s(&fichier,title,"wb");
#else
	FILE *fichier = fopen(title,"wb");
#endif
  if (!fichier) return false;
  unsigned char *pixels;
  int w,h;

  w = glutGet(GLUT_WINDOW_WIDTH);
  h = glutGet(GLUT_WINDOW_HEIGHT);
  if (w%4!=0) w=(int(w/4))*4;

  pixels = (unsigned char*) malloc(3*w*h);
  glReadPixels(0,0,w,h,GL_RGB,GL_UNSIGNED_BYTE,(GLvoid*) pixels);
  uint64_t t = BaseSimulator::getScheduler()->now();
  fprintf(fichier,"P6\n# time: %d:%d\n%d %d\n255\n",int(t/1000),int(t%1000),w,h);
  unsigned char *ptr = pixels+(h-1)*w*3;
  while (h--) {
    fwrite(ptr,w*3,1,fichier);
    ptr-=w*3;
  }
  fclose(fichier);
  free(pixels);
  return true;
}

void GlutContext::setFullScreenMode(bool b) {
	fullScreenMode = true;
}
