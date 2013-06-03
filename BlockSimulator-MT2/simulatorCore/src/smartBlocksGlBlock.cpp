#include "smartBlocksGlBlock.h"

SmartBlocksGlBlock::SmartBlocksGlBlock(int id) : GlBlock(id) {
}

SmartBlocksGlBlock::~SmartBlocksGlBlock() {

}

void SmartBlocksGlBlock::glDraw(ObjLoader::ObjLoader *ptrObj) {
	glPushMatrix();

	glTranslatef(position[0]+12.5,position[1]+12.5,position[2]);
	if (isHighlighted) {
		GLfloat n = 0.5+1.5*(1.0-(glutGet(GLUT_ELAPSED_TIME)%1000)/1000.0);
		GLfloat c[4];
		c[0]=color[0]*n;
	    c[1]=color[1]*n;
	    c[2]=color[2]*n;
	    c[3]=1.0;
	    ptrObj->setAmbientAndDiffuseColor(c);
//	    glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE,c);
	} else {
//		GLfloat gray[]={0.2,0.2,0.2,1.0};
	    ptrObj->setAmbientAndDiffuseColor(color);
//		glMaterialfv(GL_FRONT,GL_AMBIENT_AND_DIFFUSE,color);
	}
	ptrObj->glDraw();
    glPopMatrix();
}

void SmartBlocksGlBlock::glDrawId(ObjLoader::ObjLoader *ptrObj) {
	glPushMatrix();
	glTranslatef(position[0]+12.5,position[1]+12.5,position[2]);
	ptrObj->glDraw();
	glPopMatrix();
}

string SmartBlocksGlBlock::getInfo() {
	return GlBlock::getInfo();
}
