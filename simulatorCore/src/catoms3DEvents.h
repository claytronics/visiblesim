/*!
 * \file catoms3DEvents.h
 * \brief Motion events for 2D Catoms
 * \date 15/02/2015
 * \author Benoît Piranda
 */

#ifndef CATOMS3DEVENTS_H_
#define CATOMS3DEVENTS_H_

#include "matrix44.h"
#include "catoms3DBlock.h"
#include "events.h"

namespace Catoms3D {

const int nbRotationSteps=100;

class Rotations {
public :
/**
    \brief Create a couple of rotations
    \param p : fixed pivot catom
    \param ax1 : rotation axe for the first rotation
    \param ang1 : rotation angle for the first rotation
    \param ax2 : rotation axe for the second rotation
    \param ang2 : rotation angle for the second rotation
*/
    Rotations(Catoms3DBlock *mobile,Catoms3DBlock *fixe,const Vector3D &ax1,double ang1,const Vector3D &ax2,double ang2);
    Rotations() {};

    void init(const Matrix& m) {
        firstRotation=true;
        step=0;
        initialMatrix=m;
        firstStepMatrix.identity();
    }

/**
    \brief Return current transformation matrix in m
    \param m : result matrix
*/
//    void getCurrentMatrix(Matrix &m) { m=mat; };
/**
    \brief Calculate transformation matrix for one more step of rotations
    \param m : result matrix
    \return true at the end of the animation.
*/
    bool nextStep(Matrix &m);
    void getFinalPositionAndOrientation(Cell3DPosition &position, short &orientation);
protected :
    bool firstRotation;
    short step;
    Matrix initialMatrix,firstStepMatrix;//,matTBA,matTAB,matTBC,matTCB;
    Vector3D AB,AD,CB;
    Vector3D axe1;
    Vector3D axe2;
    double angle1;
    double angle2;
    static constexpr double angleArticulation=6.46237535743;
    static constexpr double coefRayonCourbure=1.00639465274;
};

//===========================================================================================================
//
//          MotionStartEvent  (class)
//
//===========================================================================================================

class MotionStartEvent : public BlockEvent {
    Rotations rot;
public:
	MotionStartEvent(uint64_t, Catoms3DBlock *block,const Rotations& r);
	MotionStartEvent(MotionStartEvent *ev);
	~MotionStartEvent();
	void consumeBlockEvent() {};
	void consume();
	const virtual string getEventName();
};

//===========================================================================================================
//
//          MotionStepEvent  (class)
//
//===========================================================================================================

class MotionStepEvent : public BlockEvent {
    Rotations rot;
public:
	MotionStepEvent(uint64_t, Catoms3DBlock *block,const Rotations& r);
	MotionStepEvent(MotionStepEvent *ev);
	~MotionStepEvent();
	void consumeBlockEvent() {};
	void consume();
	const virtual string getEventName();
};

//===========================================================================================================
//
//          MotionStopEvent  (class)
//
//===========================================================================================================

class MotionStopEvent : public BlockEvent {
    Rotations rot;
public:
	MotionStopEvent(uint64_t, Catoms3DBlock *block,const Rotations& r);
	MotionStopEvent(MotionStepEvent *ev);
	~MotionStopEvent();
	void consumeBlockEvent() {};
	void consume();
	const virtual string getEventName();
};

//===========================================================================================================
//
//          MotionEndEvent  (class)
//
//===========================================================================================================

class MotionEndEvent : public BlockEvent {
public:
	MotionEndEvent(uint64_t, Catoms3DBlock *block);
	MotionEndEvent(MotionEndEvent *ev);
	~MotionEndEvent();
	void consumeBlockEvent() {};
	void consume();
	const virtual string getEventName();
};

}
#endif /* CATOMS3DEVENTS_H_ */
