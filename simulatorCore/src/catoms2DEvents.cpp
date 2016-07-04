/*
 * catoms2DEvents.cpp
 *
 *  Created on: 2014 fevrary 1st
 *      Author: Benoît
 */

#include "catoms2DEvents.h"
#include "catoms2DWorld.h"
#include "catoms2DMove.h"
#include "utils.h"

const int ANIMATION_DELAY=40000;
const int COM_DELAY=2000;
const int ANGULAR_STEP=12;

namespace Catoms2D {

//===========================================================================================================
//
//          MotionStartEvent  (class)
//
//===========================================================================================================

MotionStartEvent::MotionStartEvent(uint64_t t, Catoms2DBlock *block, Catoms2DMove &m): BlockEvent(t,block) {
    EVENT_CONSTRUCTOR_INFO();
    eventType = EVENT_MOTION_START;
    
    pivot.set(m.getPivot()->ptrGlBlock->position[0],m.getPivot()->ptrGlBlock->position[1],m.getPivot()->ptrGlBlock->position[2]);
    angle = 60;
    sens = m.getDirection();
}

MotionStartEvent::MotionStartEvent(MotionStartEvent *ev) : BlockEvent(ev) {
    EVENT_CONSTRUCTOR_INFO();
}

MotionStartEvent::~MotionStartEvent() {
    EVENT_DESTRUCTOR_INFO();
}

void MotionStartEvent::consume() {
    EVENT_CONSUME_INFO();
    Scheduler *scheduler = getScheduler();
    Catoms2DBlock *rb = (Catoms2DBlock *)concernedBlock;
    Catoms2DWorld::getWorld()->disconnectBlock(rb);
    rb->setColor(DARKGREY);
    scheduler->schedule(new MotionStepEvent(scheduler->now() + ANIMATION_DELAY, rb,pivot,angle,sens));
}

const string MotionStartEvent::getEventName() {
    return("MotionStart Event");
}

//===========================================================================================================
//
//          MotionStepEvent  (class)
//
//===========================================================================================================

MotionStepEvent::MotionStepEvent(uint64_t t, Catoms2DBlock *block,const Vector3D &p,double angle2goal,int s): BlockEvent(t,block) {
    EVENT_CONSTRUCTOR_INFO();
    eventType = EVENT_MOTION_STEP;

    pivot = p;
    angle = angle2goal;
    sens = s;
}

MotionStepEvent::MotionStepEvent(MotionStepEvent *ev) : BlockEvent(ev) {
    EVENT_CONSTRUCTOR_INFO();
}

MotionStepEvent::~MotionStepEvent() {
    EVENT_DESTRUCTOR_INFO();
}

void MotionStepEvent::consume() {
    EVENT_CONSUME_INFO();
    Catoms2DBlock *rb = (Catoms2DBlock*)concernedBlock;

    Scheduler *scheduler = getScheduler();

    Matrix roty;
    if (angle<ANGULAR_STEP) {
        roty.setRotationY(-sens*angle);
        Vector3D BA(rb->ptrGlBlock->position[0] - pivot[0],
                    rb->ptrGlBlock->position[1] - pivot[1],
                    rb->ptrGlBlock->position[2] - pivot[2]);
        Vector3D BC = roty*BA;
        Vector3D pos = pivot+BC;
        rb->angle += angle*sens;
        Catoms2DWorld::getWorld()->updateGlData(rb,pos,((Catoms2DGlBlock*)rb->ptrGlBlock)->angle+angle*sens);
        scheduler->schedule(new MotionStopEvent(scheduler->now() + ANIMATION_DELAY, rb));
    } else {
        roty.setRotationY(-sens*ANGULAR_STEP);
        Vector3D BA(rb->ptrGlBlock->position[0] - pivot[0],
                    rb->ptrGlBlock->position[1] - pivot[1],
                    rb->ptrGlBlock->position[2] - pivot[2]);
        Vector3D BC = roty*BA;
        Vector3D pos = pivot+BC;
        rb->angle += ANGULAR_STEP*sens;
        Catoms2DWorld::getWorld()->updateGlData(rb,pos,
                                                ((Catoms2DGlBlock*)rb->ptrGlBlock)->angle+ANGULAR_STEP*sens);
        scheduler->schedule(new MotionStepEvent(scheduler->now() + ANIMATION_DELAY,rb,
                                                pivot,angle-ANGULAR_STEP,sens));
    }
}

const string MotionStepEvent::getEventName() {
    return("MotionStep Event");
}

//===========================================================================================================
//
//          MotionStepEvent  (class)
//
//===========================================================================================================

MotionStopEvent::MotionStopEvent(uint64_t t, Catoms2DBlock *block): BlockEvent(t,block) {
    EVENT_CONSTRUCTOR_INFO();
    eventType = EVENT_MOTION_STOP;
}

MotionStopEvent::MotionStopEvent(MotionStepEvent *ev) : BlockEvent(ev) {
    EVENT_CONSTRUCTOR_INFO();
}

MotionStopEvent::~MotionStopEvent() {
    EVENT_DESTRUCTOR_INFO();
}

void MotionStopEvent::consume() {
    EVENT_CONSUME_INFO();
    Catoms2DBlock *rb = (Catoms2DBlock*)concernedBlock;
    rb->setColor(YELLOW);

    /* Transformer les coordonnées GL en coordonnées grille*/

    Catoms2DWorld *wrld=Catoms2DWorld::getWorld();
    Vector3D worldPos = Vector3D(rb->ptrGlBlock->position[0],
                                 rb->ptrGlBlock->position[1],
                                 rb->ptrGlBlock->position[2]);
    Cell3DPosition gridPos = wrld->lattice->worldToGridPosition(worldPos);
    cout << "---------------motion end-----------------"<<endl;
    cout << worldPos << endl;
    cout << gridPos << endl;
    cout << "------------------------------------------"<<endl;
    rb->setPosition(gridPos);    

    stringstream info;
    info.str("");
    info << "connect Block " << rb->blockId;
    getScheduler()->trace(info.str(),rb->blockId,LIGHTBLUE);
    wrld->connectBlock(rb);
    Scheduler *scheduler = getScheduler();
    scheduler->schedule(new MotionEndEvent(scheduler->now() + ANIMATION_DELAY, rb));
}

const string MotionStopEvent::getEventName() {
    return("MotionStop Event");
}

//===========================================================================================================
//
//          MotionEndEvent  (class)
//
//===========================================================================================================

MotionEndEvent::MotionEndEvent(uint64_t t, Catoms2DBlock *block): BlockEvent(t,block) {
    EVENT_CONSTRUCTOR_INFO();
    eventType = EVENT_MOTION_END;
}

MotionEndEvent::MotionEndEvent(MotionEndEvent *ev) : BlockEvent(ev) {
    EVENT_CONSTRUCTOR_INFO();
}

MotionEndEvent::~MotionEndEvent() {
    EVENT_DESTRUCTOR_INFO();
}

void MotionEndEvent::consume() {
    EVENT_CONSUME_INFO();
    Catoms2DBlock *rb = (Catoms2DBlock*)concernedBlock;
    concernedBlock->blockCode->processLocalEvent(EventPtr(new MotionEndEvent(date+COM_DELAY,rb)));
}

const string MotionEndEvent::getEventName() {
    return("MotionEnd Event");
}


} // Catoms2D namespace
