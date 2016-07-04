/////////////////////////////////////////////////////////////////////////////
// File:        objLoader.h
// Project:     OBJ models file loader
// Author:      Benoît Piranda
// Modified by:
// Created:     2008-09-29
// Copyright:   (c) Benoît Piranda
// Licence:     UFC licence
// Version: 1.6 Windows
/////////////////////////////////////////////////////////////////////////////

#ifndef OBJLOADER_H_
#define OBJLOADER_H_

#ifdef WIN32
#include <windows.h>
#endif
#include <openglViewer.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <cmath>
#include <stdlib.h>
#include <string.h>

using namespace std;

namespace ObjLoader {
/////////////////////////////////////////////////////////////////////////////
// Point2
class Point2
{ 
public :
  float v[2];
  
  Point2() {};
  Point2(double x,double y) { this->v[0]=(float)x; this->v[1]=(float)y; };
  int scan(const char *str) 
#ifdef WIN32
  { return sscanf_s(str,"%f %f",&v[0],&v[1]); };
#else
  { return sscanf(str,"%f %f",&v[0],&v[1]); };
#endif
};

class Point3
{ 
public :
  float v[3];
  
  Point3() {};
  Point3(double x,double y,double z) { this->v[0]=(float)x; this->v[1]=(float)y; this->v[2]=(float)z; };
  int scan(const char *str) 
#ifdef WIN32
  { return sscanf_s(str,"%f %f %f",&v[0],&v[1],&v[2]); };
#else
  { return sscanf(str,"%f %f %f",&v[0],&v[1],&v[2]); };
#endif
};

/////////////////////////////////////////////////////////////////////////////
// sommet : vertex data
class Sommet
{ 
public :
  GLfloat v[3],t[2],n[3];
  
  Sommet() { v[0]=0; v[1]=0; v[2]=0; t[0]=0; t[1]=0; n[0]=0; n[1]=0; n[2]=0; };
  void set(GLfloat *tabV,GLfloat *tabN,GLfloat *tabT);
  bool operator==(const Sommet &s);
};

class FaceTri {
public :
  GLuint ind[3];

  FaceTri(GLuint a,GLuint b,GLuint c) {
	  ind[0]=a;
	  ind[1]=b;
	  ind[2]=c;
  };
};

/////////////////////////////////////////////////////////////////////////////
// mtl : material data
class Mtl
{ 
public :
  int id; 
  char *name;
  GLfloat Ka[4],Kd[4],Ks[4],Ke[4],Ns;
  char *mapKd;
  GLuint glTexId;
  Mtl();
  ~Mtl();
  void glBind();
};

/////////////////////////////////////////////////////////////////////////////
// mtlLib : list of materials
class MtlLib
{ 
public :
  vector <Mtl*> tabMtl;

  MtlLib(const char *rep,const char *titre);
  MtlLib() {};
  Mtl *getMtlByName(const char *nom);
  Mtl *getMtlById(int);
  Mtl *getDefaultMtl();
};

/////////////////////////////////////////////////////////////////////////////
// objData : sub-objects data
struct vertexPosNrmTx {
	GLfloat x,y,z;
	GLfloat nx,ny,nz;
	GLfloat s,t;
};

#define BUFFER_OFFSET(i) ((char *)NULL+i)
class ObjData
{ public :
  std::vector <Sommet*> tabVertex;
  std::vector <FaceTri*> tabFaces;
  vertexPosNrmTx *tabVertices;
  GLuint *tabIndices;
  GLuint nbreIndices;
  GLuint vboId,indexVboId;

  char nom[128],nomOriginal[64];
  Mtl *objMtl;
  
  ObjData(const char*);
  ~ObjData();
  void addFace(Sommet &ptr1,Sommet &ptr2,Sommet &ptr3);
  GLuint addVertex(const Sommet &s);
  void glDraw(void);
  void glDrawId(void);
  void createVertexArray();
};

/////////////////////////////////////////////////////////////////////////////
// objLoader : load and store a set of objects
class ObjLoader {
	std::vector <ObjData*> tabObj;
	MtlLib *mtls;
	Mtl *ptrMtlLighted;
protected :
	void numeroPoint(char* str,int &vert,int &norm,int &tex);
public:
	ObjLoader(const char *rep,const char *titre);
	~ObjLoader(void);
	void createVertexArrays();
	void glDraw(void);
	void glDrawIdByMaterial(int &i);
	void glDrawId(int &i);
	void setLightedColor(GLfloat *color);
	inline string getObjMtlName(int pos) { return tabObj[pos]->objMtl->name; };
};

}

#endif
