/*
 * Interface.h
 *
 *  Created on: 27 févr. 2012
 *      Author: Ben
 */

#ifndef INTERFACE_H_
#define INTERFACE_H_

#include <vector>
#include <iostream>
#include <fstream>
#include <memory.h>

#include "glBlock.h"
#include "world.h"
#include "scheduler.h"

using namespace std;
using namespace BaseSimulator;

class GlutWindow {

protected :
	GLuint id;
	vector<GlutWindow*> children;
	GLuint idTexture;

	void addChild(GlutWindow *child);
	void bindTexture();
  public :
	GLint x,y,w,h;
	GlutWindow(GlutWindow *parent,GLuint pid,GLint px,GLint py,GLint pw,GLint ph,const char *titreTexture);
	virtual ~GlutWindow();
	inline void setGeometry(GLint px,GLint py,GLint pw,GLint ph) { x=px; y=py; w=pw; h=ph; };

	virtual void glDraw();
	virtual int mouseFunc(int button,int state,int x,int y);
	virtual void reshapeFunc(int mw,int mh) {};
	static GLuint loadTexture(const char *titre,int &tw,int &th);
	//GLuint loadTexture(const char *titre);
	static unsigned char *lectureTarga(const char *titre, int& width, int& height ,bool retourner=false);
	static GLfloat drawString(GLfloat x,GLfloat y,const char *str);
};

class GlutButton : public GlutWindow {
	bool isActive;
	bool isDown;
	bool isHighlighted;
public :
	GlutButton(GlutWindow *parent,GLuint pid,GLint px,GLint py,GLint pw,GLint ph,const char *titreTexture,bool pia=true);
	virtual ~GlutButton() {};

	inline void activate(bool v) { isActive=v; };
	int mouseFunc(int button,int state,int x,int y);
	void glDraw();
};

class GlutSlidingMainWindow : public GlutWindow {
	int openingLevel;
	GlutButton* buttonOpen, *buttonClose;

public :
	GlutSlidingMainWindow(GLint px,GLint py,GLint pw,GLint ph,const char *titreTexture);
	virtual ~GlutSlidingMainWindow() {};

	inline void open() { openingLevel++; };
	inline void close() { openingLevel--; };

	int mouseFunc(int button,int state,int mx,int my);
	void reshapeFunc(int mw,int mh);
	void glDraw();
};

class GlutPopupWindow : public GlutWindow {
	string info;
	bool isVisible;
public :
	GlutPopupWindow(GlutWindow *parent,GLint px,GLint py,GLint pw,GLint ph);
	virtual ~GlutPopupWindow() {};

	void show(bool v) { isVisible=v;};
	void setCenterPosition(int ix,int iy) { x=ix-w/2; y=iy; };
	void setInfo(const string &str) { info=str; };
	void glDraw();
};

class GlutPopupMenuWindow : public GlutWindow {
	bool isVisible;
public :
	GlutPopupMenuWindow(GlutWindow *parent,GLint px,GLint py,GLint pw,GLint ph);
	virtual ~GlutPopupMenuWindow() {};

	void show(bool v) { isVisible=v;};
	void setCenterPosition(int ix,int iy) { x=ix-w/2; y=iy; };
	void addButton(int id,const char *titre);
	int mouseFunc(int button,int state,int x,int y);
	void glDraw();
};

#define ID_SW_BUTTON_OPEN	1001
#define ID_SW_BUTTON_CLOSE	1002

#endif /* INTERFACE_H_ */
