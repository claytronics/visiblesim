/*
 * Interface.cpp
 *
 *  Created on: 27 févr. 2012
 *      Author: Ben
 */

#include "interface.h"

GlutWindow::GlutWindow(GlutWindow *parent,GLuint pid,GLint px,GLint py,GLint pw,GLint ph,const char *titreTexture)
:id(pid) {
	if (parent) parent->addChild(this);
	setGeometry(px,py,pw,ph);
	idTexture=loadTexture(titreTexture);
}

GlutWindow::~GlutWindow() {
	vector <GlutWindow*>::const_iterator cw = children.begin();
	while (cw!=children.end())
	{	delete (*cw);
		cw++;
	}
	children.clear();
}

void GlutWindow::addChild(GlutWindow *child) {
	children.push_back(child);
}

void GlutWindow::glDraw() {
	vector <GlutWindow*>::const_iterator cw = children.begin();
	while (cw!=children.end())
	{	glPushMatrix();
	    glTranslatef(x,y,0);
	    (*cw)->glDraw();
	    glPopMatrix();
		cw++;
	}
}

int GlutWindow::mouseFunc(int button,int state,int mx,int my) {
	int id=0;

	if (mx>=x && mx<=x+w && my>=y && my<=y+h)
	{ vector <GlutWindow*>::const_iterator cw = children.begin();
	  while (cw!=children.end())
	  {	id = (*cw)->mouseFunc(button,state,mx-x,my-y);
	    if (id!=0) return id;
		cw++;
	  }
	  return id;
	}
	return 0;
}

void GlutWindow::bindTexture() {
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D,idTexture);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
}

// function printing a string on the glut screen on x,y
// warning : must be ended by \0
GLfloat GlutWindow::drawString(GLfloat x,GLfloat y,const char *str)
{ glRasterPos2f(x,y);
  while (*str)
  { if (*str=='\n')
    { y-=14;
	  glRasterPos2f(x,y);
    } else
    { glutBitmapCharacter(GLUT_BITMAP_8_BY_13,*str);
    }
    str++;
  }
  return y-14;
}

/***************************************************************************************/
/* GlutSlidingMainWindow */
/***************************************************************************************/

GlutSlidingMainWindow::GlutSlidingMainWindow(GLint px,GLint py,GLint pw,GLint ph,const char *titreTexture):
GlutWindow(NULL,1,px,py,pw,ph,titreTexture) {
	openingLevel=0;
	buttonOpen = new GlutButton(this,ID_SW_BUTTON_OPEN,5,68,32,32,"../../simulatorCore/smartBlocksTextures/boutons_fg.tga");
	buttonClose = new GlutButton(this,ID_SW_BUTTON_CLOSE,5,26,32,32,"../../simulatorCore/smartBlocksTextures/boutons_fd.tga",false);
}

void GlutSlidingMainWindow::glDraw() {
    // drawing of the tab
	bindTexture();
	glPushMatrix();
	glTranslatef(x,y,0.0);
	glBegin(GL_QUADS);
	glTexCoord2f(0,0.0);
	glVertex2i(0,0);
	glTexCoord2f(0.3125,0.);
	glVertex2i(40,0);
	glTexCoord2f(0.3125,1);
	glVertex2i(40,128);
	glTexCoord2f(0,1);
	glVertex2i(0,128);
	glEnd();

	if (openingLevel) {
		glBegin(GL_QUADS);
		glTexCoord2f(0.3125,0.);
		glVertex2i(40,0);
		glTexCoord2f(1.0,0.);
		glVertex2i(w,0);
		glTexCoord2f(1.0,1.0);
		glVertex2i(w,h);
		glTexCoord2f(0.3125,1.0);
		glVertex2i(40,h);
		glEnd();
		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);
		glColor4f(1.0,1.0,1.0,1.0);
		char str[256];
		uint64_t t = BaseSimulator::getScheduler()->now();
		sprintf(str,"Current time : %d:%d",int(t/1000000),int((t%1000000)/10000));

		glColor3f(1.0,1.0,0.0);
		drawString(42.0,h-20.0,str);
		glColor3f(1.0,1.0,1.0);
		GlBlock *selectedBlock = getWorld()->getSelectedBlock();
		if (selectedBlock) {
			sprintf(str,"Selected Block : %s",selectedBlock->getInfo().c_str());
		} else {
			sprintf(str, "Selected Block : None\n(use [Ctrl]+click)");
		}
		drawString(42.0,h-40.0,str);
/*
        if (!Scheduler::lstInfos.empty())
	    { list<Info*>::const_iterator ci = Scheduler::lstInfos.begin();
		  GLfloat posy = h-70;
		  while (posy>2 && ci!=Scheduler::lstInfos.end())
		  { posy = drawString(42.0,posy,(*ci)->str);
		    ci++;
		  }
	    }*/
	}
	glPopMatrix();
	GlutWindow::glDraw();
}

int GlutSlidingMainWindow::mouseFunc(int button,int state,int mx,int my)
{ int n = GlutWindow::mouseFunc(button,state,mx,my);
  switch (n) {
	  case ID_SW_BUTTON_OPEN :
		  openingLevel++;
		  x-=200;
		  w+=200;
		  buttonOpen->activate(false);
		  buttonClose->activate(true);
	  break;
	  case ID_SW_BUTTON_CLOSE :
		  openingLevel--;
		  x+=200;
		  w-=200;
		  buttonOpen->activate(true);
		  buttonClose->activate(false);
	  break;
  }
  return n;
}

void GlutSlidingMainWindow::reshapeFunc(int mw,int mh)
{ int sz = 40+200*openingLevel;
  setGeometry(mw-sz,50,sz,mh-60);
}

/***************************************************************************************/
/* GlutButton */
/***************************************************************************************/

GlutButton::GlutButton(GlutWindow *parent,GLuint pid,GLint px,GLint py,GLint pw,GLint ph,const char *titreTexture,bool pia) :
		GlutWindow(parent,pid,px,py,pw,ph,titreTexture) {
	isActive = pia;
	isDown=false;
}

void GlutButton::glDraw()
{	GLfloat tx=0,ty=0.5;

    bindTexture();
    if (isActive) {
    	if (isDown) {
    		tx=0.0;
    		ty=0.0;
    	} else {
    		tx=0.5;
    		ty=0.5;
    	}

    } else {
    	tx=0.0;
    	ty=0.5;
    }
    glPushMatrix();
	glTranslatef(x,y,0.0f);
	glBegin(GL_QUADS);
	glTexCoord2f(tx,ty);
	glVertex2i(0,0);
	glTexCoord2f(tx+0.5f,ty);
	glVertex2i(w,0);
	glTexCoord2f(tx+0.5f,ty+0.5f);
	glVertex2i(w,h);
	glTexCoord2f(tx,ty+0.5f);
  	glVertex2i(0,h);
  	glEnd();

  	glPopMatrix();
}

int GlutButton::mouseFunc(int button,int state,int mx,int my)
{ if (mx>x && mx<x+w && my>y && my<y+h) {
	  isDown=(state==GLUT_DOWN);
	  return (isActive && isDown)? id:0;
  }
  return 0;
}


/////////////////////////////////////////////////////////////////////////////
// loadTextures
// lecture de l'identifiant de texture
GLuint GlutWindow::loadTexture(const char *titre) {
	unsigned char *image;
	int w,h;
	GLuint id=0;
	cout << "loading " << titre << endl;
	if (!(image=lectureTarga(titre,w,h))) {
		cerr << "Error : can't open " << titre << endl;
	} else {
		glGenTextures(1,&id);
		glBindTexture(GL_TEXTURE_2D,id);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		gluBuild2DMipmaps(GL_TEXTURE_2D,GL_RGBA,w,h,GL_RGBA,GL_UNSIGNED_BYTE,image);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
		delete [] image;
	}
	return id;
}

unsigned char *GlutWindow::lectureTarga(const char *titre, int& width, int& height ,bool retourner)
{
	#define DEF_targaHeaderLength			12
	#define DEF_targaHeaderContent		"\x00\x02\x00\x00\x00\x00\x00\x00\x00\x00\x00"

  ifstream fin;
  char *pData;
  streampos maxLen=0;

  fin.open(titre,ios::binary);
  if (!fin.is_open()) return NULL;

// calcul la longueur du fichier
  fin.seekg (0, ios::end);
  maxLen = fin.tellg();
  fin.seekg (0, ios::beg);

  // allocation de la mémoire pour le fichier
  pData = new char [int(maxLen)];

  // lecture des données du fichier
  fin.read(pData,maxLen);

  fin.close();

	int commentOffset = int( (unsigned char)*pData );
	if( memcmp( pData + 1, DEF_targaHeaderContent, DEF_targaHeaderLength - 1 ) != 0 )
	{ cerr << "Format non reconnu : " << titre << endl;
	  return 0;
    }
	unsigned char smallArray[ 2 ];

	memcpy( smallArray, pData + DEF_targaHeaderLength + 0, 2 );
	width = smallArray[ 0 ] + smallArray[ 1 ] * 0x0100;

	memcpy( smallArray, pData + DEF_targaHeaderLength + 2, 2 );
	height = smallArray[ 0 ] + smallArray[ 1 ] * 0x0100;

	memcpy( smallArray, pData + DEF_targaHeaderLength + 4, 2 );
	int depth = smallArray[ 0 ];
//	int pixelBitFlags = smallArray[ 1 ];

	if( ( width <= 0 ) || ( height <= 0 ) )
		return 0;

	// Only allow 24-bit and 32-bit!
	bool is24Bit( depth == 24 );
	bool is32Bit( depth == 32 );
	if( !( is24Bit || is32Bit ) )
		return 0;

	// Make it a BGRA array for now.
	int bodySize( width * height * 4 );
	unsigned char * pBuffer = new unsigned char[ bodySize ];
	if( is32Bit )
	{
		// Easy, just copy it.
		memcpy( pBuffer, pData + DEF_targaHeaderLength + 6 + commentOffset, bodySize );
	}
	else if( is24Bit )
	{
		int bytesRead = DEF_targaHeaderLength + 6 + commentOffset;
		for( int loop = 0; loop < bodySize; loop += 4, bytesRead += 3 )
		{
			memcpy( pBuffer + loop, pData + bytesRead, 3 );
			pBuffer[ loop + 3 ] = 255;			// Force alpha to max.
		}
	}
	else return NULL;

	// Swap R & B (convert to RGBA).
	for( int loop = 0; loop < bodySize; loop += 4 )
	{
		unsigned char tempC = pBuffer[ loop + 0 ];
		pBuffer[ loop + 0 ] = pBuffer[ loop + 2 ];
		pBuffer[ loop + 2 ] = tempC;
	}

  delete [] pData;


  if (retourner)
  { unsigned char * pBufferRet = new unsigned char[ bodySize ],
                  *ptr1=pBuffer+width*(height-1)*4,*ptr2=pBufferRet;
	for (int loop=0; loop<height; loop++)
	{ memcpy(ptr2,ptr1,width*4);
	  ptr2+=width*4;
	  ptr1-=width*4;
	}
	delete [] pBuffer;
	return pBufferRet;
  }
	// Ownership moves out.
	return pBuffer;
}

/***************************************************************************************/
/* GlutPopupWindow */
/***************************************************************************************/
GlutPopupWindow::GlutPopupWindow(GlutWindow *parent,GLint px,GLint py,GLint pw,GLint ph)
:GlutWindow(parent,99,px,py,pw,ph,"") {
	isVisible=false;
}

void GlutPopupWindow::glDraw() {
	if (isVisible) {
		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);
		glColor4f(1.0,1.0,0.0,0.75);
		glPushMatrix();
		glTranslatef(x,y,0);
		glBegin(GL_QUADS);
		glVertex2i(0,0);
		glVertex2i(w,0);
		glVertex2i(w,h);
		glVertex2i(0,h);
		glEnd();
		glColor4f(0.0,0.0,0.5,1.0);
		drawString(5.0,h-20.0,info.c_str());
		glPopMatrix();
	}
}
