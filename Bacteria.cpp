#include "bacteria.h"

BacteriaRef Bacteria::create(glm::vec2 position, float lifespan, glm::vec2 acceleration, float size)
{
    BacteriaRef ref = std::shared_ptr<Bacteria> (new Bacteria) ;
    ref->setup(position, lifespan, acceleration, size) ;
    return ref ;
}

Bacteria::Bacteria() :
mPosition(glm::vec2(ci::randFloat(-50.f, 50.f), ci::randFloat(-50.f,50.f))),
mAcceleration(glm::vec2(0,0)),
mVelocity(glm::vec2(0,0)),
mFriction(0.98f)
{
    
}


void Bacteria::setup(glm::vec2 position, float lifespan, glm::vec2 acceleration, float size)
{
    mLifespan = lifespan ;
    mPosition = position ;
    mAcceleration = acceleration ;
    mSize = size ;
    mVelocity += mAcceleration ;
//    tempPosition = mPosition ;
    //    cinder::gl::drawSolidCircle(mPosition, 10.f) ;
    mTimer.start() ;
    
}

void Bacteria::increaseSize()
{
    mSize++ ;
}

float Bacteria::getTimerLeft()
{
    return (mLifespan-mTimer.getSeconds()) ;
}

void Bacteria::resetLifespan()
{
    mLifespan = mLifespan + 5.f ;
}

void Bacteria::update()
{
    
    mVelocity *= mFriction ;
    
//    if(ci::app::getElapsedSeconds() - lastTime > waitTime) {
//        tempPosition = mPosition ;
//        lastTime = ci::app::getElapsedSeconds() ;
//    }
    mPosition += mVelocity ;
    //    ci::gl::color(1.0, 0.f, 0.f) ;
    //    cinder::gl::drawSolidCircle(mPosition, 10.f) ;
}

void Bacteria::draw()
{
    //    ci::gl::color(1.0, 0.f, 0.f) ;
    cinder::gl::drawStrokedCircle(mPosition, mSize) ;
}

Bacteria::~Bacteria()
{
    mTimer.stop() ;
}

void Bacteria::applyForce(glm::vec2 force)
{
    mVelocity += force ;
}
