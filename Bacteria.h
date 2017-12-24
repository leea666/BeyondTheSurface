#pragma once
#include "cinder/Rand.h"

class Bacteria ;
typedef std::shared_ptr<Bacteria> BacteriaRef ;

class Bacteria {
public:
    static BacteriaRef create(glm::vec2 position, float lifespan, glm::vec2 acceleration, float size) ;
    void update() ;
    void draw() ;
    void applyForce(glm::vec2 force) ;
    float getTimerLeft() ;
    void increaseSize() ;
    void resetLifespan() ;
    float getSize() {return mSize;} ;
    float getLifeSpan(){return mLifespan;}
    glm::vec2 getPosition(){return mPosition;}
//    glm::vec2 getTempPosition(){return tempPosition;}
    float getColor(){return getTimerLeft()-getLifeSpan();}
    
    ~Bacteria() ;
    
private:
    Bacteria() ;
    void setup(glm::vec2 position, float lifespan,  glm::vec2 acceleration, float size) ;
    
    glm::vec2 mPosition ;
//    glm::vec2 tempPosition ;
    float mFriction ;
    float mLifespan ;
    glm::vec2 mVelocity ;
    glm::vec2 mAcceleration ;
    float mSize ;
    
//    int waitTime = 200 ;
//    unsigned long lastTime = 0 ;
    
    ci::Timer mTimer ;
};
