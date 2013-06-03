/*
 * openglViewer.cpp
 *
 *  Created on: 21/02/2012
 *      Author: Ben
 */

#include "openglViewer.h"
#include "Scheduler.h"
#include "physics.h"

OpenglViewer::OpenglViewer(int argc, char **argv) {
	
	camera = new Camera(-M_PI/2.0,M_PI/6.0,20.0);
	lightDirectionVector[0] = 1.0;
    lightDirectionVector[1] = 1.0;
    lightDirectionVector[2] = -1.0;
	lightDirectionVector[3] = 1.0; // directional source
	double d=sqrt(lightDirectionVector[0]*lightDirectionVector[0]+
		lightDirectionVector[1]*lightDirectionVector[1]+
		lightDirectionVector[2]*lightDirectionVector[2]);
	lightDirectionVector[0] /= d;
    lightDirectionVector[1] /= d;
    lightDirectionVector[2] /= d;
	
	GlutContext::init(argc,argv,this);
	
	camera->setTarget(Vecteur(0,0,1.0));
#ifdef WIN32
	strcpy_s(comment,"bloc :");
#else
	strcpy(comment,"bloc :");
#endif
 }

OpenglViewer::~OpenglViewer()
{ delete camera;
}

void OpenglViewer::visualise() {
  glutMainLoop();
}

void OpenglViewer::glDraw()
{ 
  // placement de la scène relativement à la caméra face à la scène
  camera->glLookAt();
  GrilleSupport::glDraw();

  // dessine rapidement un repère
  glPushMatrix();
  glScalef(GrilleSupport::tailleBlockX,GrilleSupport::tailleBlockY,GrilleSupport::tailleBlockZ);
  glDisable(GL_LIGHTING);
  glDisable(GL_TEXTURE_2D);
  glBegin(GL_LINES);
  glColor3f(1.f,0.f,0.f);
  glVertex3f(0.f,0.f,0.01f);
  glVertex3f(5.f,0.f,0.01f);
  glColor3f(0.f,1.f,0.f);
  glVertex3f(0.f,0.f,0.01f);
  glVertex3f(0.f,5.f,0.01f);
  glColor3f(0.f,0.f,1.f);
  glVertex3f(0.f,0.f,0.01f);
  glVertex3i(0,0,5);
  glEnd();
  glPopMatrix();
}

void OpenglViewer::glDrawId()
{
	cout << "OpenglViewer::glDrawId()" << endl;
  // placement de la scène relativement à la caméra face à la scène
  camera->glLookAt();

  GrilleSupport::glDrawId();
}


//===========================================================================================================
//
//          GrilleSupport  (class)
//
//===========================================================================================================

vector<Bloc*> GrilleSupport::listeBlocs;
Cellule *GrilleSupport::tableauCellules;
int GrilleSupport::tailleX;
int GrilleSupport::tailleY;
GLfloat GrilleSupport::tailleBlockX=25.0;
GLfloat GrilleSupport::tailleBlockY=25.0;
GLfloat GrilleSupport::tailleBlockZ=11.0;
GLuint GrilleSupport::idTextureBlock;
GLuint GrilleSupport::idTextureFloor;

void GrilleSupport::initGrilleSupport(int largeur, int hauteur) {
	cout << "Initialisation GrilleSupport\n";

	tailleX = largeur;
	tailleY = hauteur;

	tableauCellules = new Cellule[tailleX*tailleY];

	idTextureBlock = GlutWindow::loadTexture("donnees/texture_block.tga");
	idTextureFloor = GlutWindow::loadTexture("donnees/circuit.tga");
}

void GrilleSupport::demarreCodeBloc(Message *msg) {
	vector<Bloc*>::const_iterator it_block = GrilleSupport::listeBlocs.begin();
	
	while (it_block!=GrilleSupport::listeBlocs.end()) {
			Scheduler::schedule(new EvenementDemarreCodeBloc(0,*it_block,msg));
			it_block++;
	}
}

bool GrilleSupport::ajouteBloc(Bloc *bloc) {
	GLfloat xd,yd;
	int x,y;
	bloc->get2DPosition(xd,yd);
	x = (int)floor(xd);
	y = (int)floor(yd);

	Cellule *cell;

	cell = getCellule(x,y);
	if (cell == NULL) return false;
	if (cell->blocContenu != NULL) return false;

	cell->blocContenu = bloc;

	cell = getCellule(x,y+1);
	if (cell != NULL && cell->blocContenu != NULL) {
		cell->blocContenu->voisinSud = bloc;
		bloc->voisinNord = cell->blocContenu;
		bloc->interfaceReseauNord->connecte(cell->blocContenu->interfaceReseauSud);
	}
	cell = getCellule(x,y-1);
	if (cell != NULL && cell->blocContenu != NULL) {
		cell->blocContenu->voisinNord = bloc;
		bloc->voisinSud = cell->blocContenu;
		bloc->interfaceReseauSud->connecte(cell->blocContenu->interfaceReseauNord);
	}
	cell = getCellule(x+1,y);
	if (cell != NULL && cell->blocContenu != NULL) {
		cell->blocContenu->voisinOuest = bloc;
		bloc->voisinEst = cell->blocContenu;
		bloc->interfaceReseauEst->connecte(cell->blocContenu->interfaceReseauOuest);
	}
	cell = getCellule(x-1,y);
	if (cell != NULL && cell->blocContenu != NULL) {
		cell->blocContenu->voisinEst = bloc;
		bloc->voisinOuest = cell->blocContenu;
		bloc->interfaceReseauOuest->connecte(cell->blocContenu->interfaceReseauEst);
	}

	listeBlocs.push_back(bloc);
	return(true);
}

void GrilleSupport::glDraw() {
	vector <Bloc*>::const_iterator ic=listeBlocs.begin();
	
	glPushMatrix();

	glTranslatef(-tailleX/2.0f,-tailleY/2.0f,0);
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,idTextureFloor);
	glNormal3f(0,0,1.0f);
	glBegin(GL_QUADS);
  	glTexCoord2f(-1.0f/tailleBlockX,-1.0f/tailleBlockY);
  	glVertex3f(-1.0f,-1.0f,0.0f);
  	glTexCoord2f(tailleX/tailleBlockX,-1.0f/tailleBlockY);
  	glVertex3f(tailleX,-1.0f,0.0f);
  	glTexCoord2f(tailleX/tailleBlockX,tailleY/tailleBlockY);
  	glVertex3f(tailleX,tailleY,0.0f);
  	glTexCoord2f(-1.0f/tailleBlockX,tailleY/tailleBlockY);
  	glVertex3f(-1.0f,tailleY,0.0f);
  	glEnd();
  	
	glBindTexture(GL_TEXTURE_2D,idTextureBlock);
	
	Scheduler::lock();

	while (ic!=listeBlocs.end()) {
		(*ic)->glDraw();
    	ic++;
	}
	Scheduler::unlock();
	// drawing the mobiles
    //Physics::glDraw();

	glPopMatrix();
}

void GrilleSupport::glDrawId() {
	int i=1;
	vector <Bloc*>::const_iterator ic=listeBlocs.begin();

	glPushMatrix();

	glTranslatef(-tailleX/2.0f,-tailleY/2.0f,0.f);

	while (ic!=listeBlocs.end()) {
		glLoadName(i++);
		(*ic)->glDrawId();
    	ic++;
	}
	glPopMatrix();
}

Cellule *GrilleSupport::getCellule(int x, int y) {
	if (x < 0 || x >= tailleX || y < 0 || y >= tailleY) {
		return(NULL);
	}
	return(&tableauCellules[x+y*tailleX]);
}

void GrilleSupport::highlightBlockNumber(int n) {
	int i=1;
	vector <Bloc*>::const_iterator ic=listeBlocs.begin();

	while (ic!=listeBlocs.end() && i<n) {
		i++;
		ic++;
	}
	if (ic!=listeBlocs.end()) {
		GlutContext::mainWindow->selectBlock((*ic));
	}
}

Bloc *GrilleSupport::getBlockFromId(int n) {
	vector <Bloc*>::const_iterator ic=listeBlocs.begin();
	while (ic!=listeBlocs.end() && (*ic)->blocID!=n) {
			ic++;
	}
	return (ic==listeBlocs.end()?NULL:(*ic));
}
//===========================================================================================================
//
//          GlutContext  (class)
//
//===========================================================================================================

int GlutContext::screenWidth = 1024;
int GlutContext::screenHeight = 800;
int GlutContext::keyboardModifier = 0;	
OpenglViewer *GlutContext::objectToDraw=NULL;
bool GlutContext::showLinks=true;
GlutSlidingMainWindow *GlutContext::mainWindow=NULL;

void GlutContext::init(int argc, char **argv,OpenglViewer *o2d) {

	objectToDraw = o2d;

	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);

	// creation of a new graphic window
	glutInitWindowPosition(0, 0);
	glutInitWindowSize(screenWidth,screenHeight);
	if (glutCreateWindow("OpenGL BlockSimulator") == GL_FALSE) {
		puts("ERREUR : echec à la création de la fenêtre graphique");
		exit(EXIT_FAILURE);
	}

	//glutFullScreen();

	////// GL parameters /////////////////////////////////////
	glEnable(GL_DEPTH_TEST);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_CULL_FACE);
	glCullFace(GL_BACK);
	glEnable(GL_NORMALIZE);

	glClearColor(0.3f,0.3f,0.3f,0.0f);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glEnable(GL_NORMALIZE);

	glutReshapeFunc(reshapeFunc);
	glutDisplayFunc(drawFunc);
	glutMouseFunc(mouseFunc);
	glutMotionFunc(motionFunc);
	glutKeyboardFunc(keyboardFunc);
	glutIdleFunc(idleFunc);

	mainWindow = new GlutSlidingMainWindow(screenWidth-40,50,40,screenHeight-60,"donnees/fenetre_onglet.tga");
}

void GlutContext::deleteContext() {
	delete mainWindow;
}

void *GlutContext::lanceScheduler(void *param) {
	int mode = *(int*)param;
	cout << "lance" << endl;
	Scheduler::run(mode);
	return(NULL);
}

//////////////////////////////////////////////////////////////////////////////
// fonction de changement de dimensions de la fenetre,
// - width  : largeur (x) de la zone de visualisation
// - height : hauteur (y) de la zone de visualisation
void GlutContext::reshapeFunc(int w,int h) {
	screenWidth=w;
	screenHeight=h;

	// size of the OpenGL drawing area
	glViewport(0,0,w,h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	// camera intrinsic parameters
	gluPerspective(60,(double)w/(double)h,10.0,5000.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	mainWindow->reshapeFunc(w,h);
}

//////////////////////////////////////////////////////////////////////////////
// fonction associée aux interruptions générées par la souris bouton pressé
// - x,y : coordonnée du curseur dans la fenêtre
void GlutContext::motionFunc(int x,int y) {
	if (keyboardModifier!=GLUT_ACTIVE_CTRL) { // rotation du point de vue
		objectToDraw->camera->mouseMove(x,y);
	}
	glutPostRedisplay();
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
		switch (button) {
		case GLUT_LEFT_BUTTON:
			if (state==GLUT_DOWN) {
				objectToDraw->camera->mouseDown(x,y);
			} else
			if (state==GLUT_UP) {
				objectToDraw->camera->mouseUp(x,y);
			}
		break;
		case GLUT_RIGHT_BUTTON:
			if (state==GLUT_DOWN) {
				objectToDraw->camera->mouseDown(x,y,true);
			} else
			if (state==GLUT_UP) {
				objectToDraw->camera->mouseUp(x,y);
			}
		break;
		case 3 : 
			objectToDraw->camera->mouseZoom(-10);
		break;
		case 4 : 
			objectToDraw->camera->mouseZoom(10);
		break;
		}

	} else { // selection du cube survolé
		if (state==GLUT_DOWN) {
			selectFunc(x,y);

		}
	}

}

//////////////////////////////////////////////////////////////////////////////
// fonction associée aux interruptions clavier
// - c : caractère saisi
// - x,y : coordonnée du curseur dans la fenètre
void GlutContext::keyboardFunc(unsigned char c, int x, int y)
{
	static int modeScheduler;
	switch(c)
    { case 27 : case 'q' : case 'Q' : // quit
    	deleteContext();
        glutLeaveMainLoop();
      break;
      case 'f' : glPolygonMode(GL_FRONT_AND_BACK,GL_LINE); break;
      case 'F' : glPolygonMode(GL_FRONT_AND_BACK,GL_FILL); break;
      case '+' : objectToDraw->camera->mouseZoom(0.5); break;
      case '-' : objectToDraw->camera->mouseZoom(-0.5); break;
      case 'r' : 
		  modeScheduler = MODE_SCHEDULER_TEMPS_REEL;
		  //pthread_create(&objectToDraw->threadScheduler,NULL,lanceScheduler,&modeScheduler);
		  Scheduler::run(modeScheduler);
		  cout <<'R' << endl;
		  break;
	  case 'R' :
		  modeScheduler = MODE_SCHEDULER_VITESSE_MAXIMALE;
		  //pthread_create(&objectToDraw->threadScheduler,NULL,lanceScheduler,&modeScheduler);
		  cout <<'R' << endl;
		  Scheduler::run(modeScheduler);
		  break;
	  case 'l' : showLinks = !showLinks; break;
    }

  glutPostRedisplay();
}


//////////////////////////////////////////////////////////////////////////////
// fonction de mise à jour des données pour l'animation
void GlutContext::idleFunc(void) {
#ifdef _WIN32
	  Sleep(40);
#else
	  usleep(40000);
#endif
	glutPostRedisplay();
}

void GlutContext::drawFunc(void)
{ // effacement de l'écran et du Z-buffer

  glClear(GL_DEPTH_BUFFER_BIT|GL_COLOR_BUFFER_BIT);

  glEnable(GL_LIGHTING);
  glLightfv(GL_LIGHT0,GL_POSITION,objectToDraw->lightDirectionVector);

  glPushMatrix();
  objectToDraw->glDraw();
  glPopMatrix();

  // drawing of the interface
  glEnable(GL_BLEND);
  glDisable(GL_DEPTH_TEST);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  gluOrtho2D(0,screenWidth,0,screenHeight);
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  glLoadIdentity();
  mainWindow->glDraw();

  // retour au mode caméra 3D
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);
  glPopMatrix();
  glDisable(GL_BLEND);
  glEnable(GL_DEPTH_TEST);

  glutSwapBuffers();
}

//////////////////////////////////////////////////////////////////////////////
// fonction de détection d'un objet à l'écran en position x,y.
void GlutContext::selectFunc(int x,int y)
{ GLuint selectBuf[512];
  GLint hits;
  GLint viewport[4];
  glGetIntegerv(GL_VIEWPORT,viewport); // récupération de la position et de la taille de la fenêtre

  glSelectBuffer(512,selectBuf);
  glRenderMode(GL_SELECT);
  // initialisation de la pile de nom (-1 nom par défaut)
  glInitNames();
  glPushName(0);
  // mise en place de la matrices de projection (cohérente avec l'affichage)
  glMatrixMode(GL_PROJECTION);
  glPushMatrix();
  glLoadIdentity();
  // filtrage de la zone de sélection (ici un bloc de 9 pixels centrés sur x,y)
  gluPickMatrix((float) x,(float)(screenHeight-y),3.0,3.0,viewport);
  // paramètres de la caméra identiques à ceux de la fonction reshape
  // paramètres intrinsèques de la caméra
  gluPerspective(60,double(screenWidth)/double(screenHeight),1.0,500.0);
  // mise en place de la matrices de modélisation (cohérente avec l'affichage)
  glMatrixMode(GL_MODELVIEW);
  glPushMatrix();
  // il faut ici dessiner la scène en nommant les éléments sélectionnables
  // placement de la caméra comme pour la fonction draw
  // placement de la scène relativement à la caméra face à la scène
  glPushMatrix();
    objectToDraw->glDrawId();
  glPopMatrix();

  glPopMatrix();
  // reset de la matrice GL_PROJECTION
  glMatrixMode(GL_PROJECTION);
  glPopMatrix();
  glMatrixMode(GL_MODELVIEW);


  glFlush();
  hits = glRenderMode(GL_RENDER);
  int n = processHits(hits,selectBuf);
#ifdef WIN32
  sprintf_s(objectToDraw->comment,"Block : %d",n);
#else
  sprintf(objectToDraw->comment,"Block : %d",n);
#endif
  GrilleSupport::highlightBlockNumber(n);
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
