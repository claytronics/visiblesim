#include "catoms3DGlBlock.h"

namespace Catoms3D {

void Catoms3DGlBlock::glDraw(ObjLoader::ObjLoader *ptrObj) {
	glPushMatrix();
	mat.glMultMatrix();
	/*glTranslatef(position[0],position[1],position[2]);
	glRotatef(psi,1,0,0);
	glRotatef(phi,0,1,0);
	glRotatef(theta,0,0,1);*/
	if (isHighlighted) {
		GLfloat n = 0.5+1.5*(1.0-(glutGet(GLUT_ELAPSED_TIME)%1000)/1000.0);
		GLfloat c[4];
		c[0]=color[0]*n;
		c[1]=color[1]*n;
		c[2]=color[2]*n;
		c[3]=1.0;
		ptrObj->setLightedColor(c);
	} else {
		ptrObj->setLightedColor(color);
	}
	glDisable(GL_CULL_FACE);
	if (color[3] > 0) ptrObj->glDraw();
	glEnable(GL_CULL_FACE);
    glPopMatrix();
}

}
