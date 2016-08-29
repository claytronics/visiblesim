#include "csg.h"
#include "color.h"
#define EPS 1e-10

/******************************************************************/

void CSGCube::toString() {
    printf("cube([%lf, %lf, %lf], true);\n", size_x, size_y, size_z);
}

bool CSGCube::isInside(const Vector3D &p, Color &color) {
    //center
    if (false) {
        if (p.pt[0] < size_x/2 && p.pt[1] < size_y/2 && p.pt[2] < size_z/2)
            return true;
    }
    else {
        if (p.pt[0] < size_x && p.pt[1] < size_y && p.pt[2] < size_z)
            return true;
    }
    return false;
}

void CSGCube::boundingBox(BoundingBox &bb) {
    bb.P0.set(-size_x/2,-size_y/2,-size_z/2,1);
    bb.P1.set(size_x/2, size_y/2, size_z/2, 1);
}

/******************************************************************/

void CSGSphere::toString() {
    printf("sphere(%lf);\n", radius);
}

bool CSGSphere::isInside(const Vector3D &p, Color &color) {
    double dist = sqrt(pow(p.pt[0], 2) + pow(p.pt[1], 2) + pow(p.pt[2], 2));
    if (dist < radius)
        return true;
    return false;
}

void CSGSphere::boundingBox(BoundingBox &bb) {
    bb.P0.set(-radius, -radius, -radius,1);
    bb.P1.set(radius, radius, radius, 1);
}

/******************************************************************/

void CSGCylinder::toString() {
    printf("cylinder(%lf, %lf, %lf, true);\n", height, radius, radius);
}

bool CSGCylinder::isInside(const Vector3D &p, Color &color) {
    if (p.pt[2] <= height/2. && p.pt[2] >= -height/2.) {
        if (sqrt(pow(p.pt[0], 2) + pow(p.pt[1],2)) < radius) {
            return true;
        }
    }
    return false;
}

void CSGCylinder::boundingBox(BoundingBox &bb) {
    bb.P0.set(-radius, -radius, -height/2,1);
    bb.P1.set(radius, radius, height/2, 1);
}

/******************************************************************/
void CSGTranslate::toString() {
    printf("translate([%lf, %lf, %lf]) ", translate[0], translate[1], translate[2]);
    for (unsigned int i = 0; i < children.size(); i++)
        children[i]->toString();
}

bool CSGTranslate::isInside(const Vector3D &p, Color &color) {
    Vector3D new_point(p[0]-translate[0], p[1]-translate[1], p[2]-translate[2], 1.0);
    
    for (unsigned int i = 0; i < children.size(); i++) {
        if (children[i]->isInside(new_point, color)) return true;
    }
    return false;
}

void CSGTranslate::boundingBox(BoundingBox &bb) {
    for (unsigned int i = 0; i < children.size(); i++) {
        children[i]->boundingBox(bb);
    }
    bb.P0.set(bb.P0[0]+translate[0], bb.P0[1]+translate[1], bb.P0[2]+translate[2],1);
    bb.P1.set(bb.P1[0]+translate[0], bb.P1[1]+translate[1], bb.P1[2]+translate[2],1);
}

/******************************************************************/
CSGRotate::CSGRotate(float x, float y, float z) {
    vec.set(x,y,z,1.0);
    Matrix mat;
    mat.setRotationX(x);
    rotate = rotate*mat;
    mat.setRotationY(y);
    rotate = rotate*mat;
    mat.setRotationZ(z);
    rotate = rotate*mat;
    rotate.inverse(rotate_1);
}

void CSGRotate::toString() {
    printf("rotate([%lf, %lf, %lf]) ", vec[0], vec[1], vec[2]);
    for (unsigned int i = 0; i < children.size(); i++)
        children[i]->toString();
}

bool CSGRotate::isInside(const Vector3D &p, Color &color) {
    Vector3D new_point = rotate_1*((Vector3D&)p);
    for (unsigned int i = 0; i < children.size(); i++) {
        if (children[i]->isInside(new_point, color)) return true;
    }
    return false;
}

void CSGRotate::boundingBox(BoundingBox &bb) {
    for (unsigned int i = 0; i < children.size(); i++) {
        children[i]->boundingBox(bb);
    }
    Vector3D P0 = rotate*bb.P0;
    Vector3D P1 = rotate*bb.P1;
    bb.P0.set(min(P0[0], P1[0]), min(P0[1],P1[1]), min(P0[2], P1[2]),1);
    bb.P1.set(max(P0[0], P1[0]), max(P0[1],P1[1]), max(P0[2], P1[2]),1);
}

/******************************************************************/
void CSGScale::toString() {
    printf("scale([%lf, %lf, %lf]) ", scale[0], scale[1], scale[2]);
    for (unsigned int i = 0; i < children.size(); i++)
        children[i]->toString();
}

bool CSGScale::isInside(const Vector3D &p, Color &color) {
    Vector3D new_point(p[0]/scale[0], p[1]/scale[1], p[2]/scale[2], 1.0);
    for (unsigned int i = 0; i < children.size(); i++) {
        if (children[i]->isInside(new_point, color)) return true;
    }
    return false;
}

void CSGScale::boundingBox(BoundingBox &bb) {
    for (unsigned int i = 0; i < children.size(); i++) {
        children[i]->boundingBox(bb);
    }
    bb.P0.set(bb.P0[0]*scale[0], bb.P0[1]*scale[1], bb.P0[2]*scale[2],1);
    bb.P1.set(bb.P1[0]*scale[0], bb.P1[1]*scale[1], bb.P1[2]*scale[2],1);
}

/******************************************************************/
void CSGUnion::toString() {
    printf("union() {\n");
    for (unsigned int i = 0; i < children.size(); i++)
        children[i]->toString();
    printf("}\n");
}

bool CSGUnion::isInside(const Vector3D &p, Color &color) {
    for (unsigned int i = 0; i < children.size(); i++) {
        if (children[i]->isInside(p, color)) return true;
    }
    return false;
}
void CSGUnion::boundingBox(BoundingBox &bb) {
    if (children.size() != 0) {
        children[0]->boundingBox(bb);
    }
    BoundingBox bbChild;
    for (unsigned int i = 1; i < children.size(); i++) {
        children[i]->boundingBox(bbChild);
        bb = bb | bbChild;
    }
}
/******************************************************************/
void CSGDifference::toString() {
    printf("difference() {\n");
    for (unsigned int i = 0; i < children.size(); i++)
        children[i]->toString();
    printf("}\n");
}

bool CSGDifference::isInside(const Vector3D &p, Color &color) {
    if (children.size() > 0 && children[0]->isInside(p, color)) {
        for (unsigned int i = 1; i < children.size(); i++) {
            if (children[i]->isInside(p, color)) return false;
        }
        return true;
    }
    return false;
}
void CSGDifference::boundingBox(BoundingBox &bb) {
    if (children.size() != 0) {
        children[0]->boundingBox(bb);
    }
}
/******************************************************************/
void CSGIntersection::toString() {
    printf("intersection() {\n");
    for (unsigned int i = 0; i < children.size(); i++)
        children[i]->toString();
    printf("}\n");
}

bool CSGIntersection::isInside(const Vector3D &p, Color &color) {
    for (unsigned int i = 0; i < children.size(); i++) {
        if (!children[i]->isInside(p, color)) return false;
    }
    return children.size() > 0 ? true : false;
}

void CSGIntersection::boundingBox(BoundingBox &bb) {
    if (children.size() != 0) {
        children[0]->boundingBox(bb);
    }
}
/******************************************************************/
void CSGColor::toString() {
    printf("color([%lf, %lf, %lf]) ", color[0], color[1], color[2]);
    for (unsigned int i = 0; i < children.size(); i++)
        children[i]->toString();
}


bool CSGColor::isInside(const Vector3D &p, Color &color) {
    for (unsigned int i = 0; i < children.size(); i++) {
        if (children[i]->isInside(p, color)) { 
            color = this->color;
            return true;
        }
    }
    return false;
}

void CSGColor::boundingBox(BoundingBox &bb) {
    for (unsigned int i = 0; i < children.size(); i++) {
        children[i]->boundingBox(bb);
    }
}
/******************************************************************/

void CSGNode::addChild(CSGNode *node) {
    children.push_back(node);
}

void CSGNode::getStats(CSGTreeStats &stats, int depth) {
    if (children.size() == 0) {
        stats.leaf++;
        stats.depth = max(stats.depth, depth);
    } 
    else {
        stats.internal++;
        for (unsigned int i = 0; i < children.size(); i++) {
            children[i]->getStats(stats, depth+1);
        }
    }
}
/******************************************************************/

const BoundingBox operator |(const BoundingBox bb1,const BoundingBox bb2) {
    BoundingBox bb;
    bb.P0.set(min(bb1.P0[0],bb2.P0[0]),min(bb1.P0[1],bb2.P0[1]),min(bb1.P0[2],bb2.P0[2]), 1.0);
    bb.P1.set(max(bb1.P1[0],bb2.P1[0]),max(bb1.P1[1],bb2.P1[1]),max(bb1.P1[2],bb2.P1[2]), 1.0);
    return bb;
}
