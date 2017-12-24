#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "bacteria.h"
#include "cinder/rand.h"

#include "cinder/Serial.h"
#include "cinder/Text.h"
#include "cinder/Log.h"

#include "cinder/qtime/QuickTimeGl.h"
#include "cinder/gl/Texture.h"
#include "cinder/Surface.h"

using namespace ci;
using namespace ci::app;
using namespace std;

#define BUFSIZE 80
#define READ_INTERVAL 0.25

class BeyondtheSurfaceApp : public App {
  public:
	void    setup() override;
	void    mouseDown( MouseEvent event ) override;
	void    update() override;
	void    draw() override;
    Rectf   getRectf(glm::vec2 startingPoint, glm::vec2 size, glm::vec2 scale) ;

    
    const int   Num_Bacteria = 1000 ;
    const int   Num_Human = 10 ;
    const float RANDOM_RANGE = 200.f ;
    const float RANDOM_FORCE_RANGE = 2.f ;
    const float lifeSpan = 100.f ;
    const float huLifeSpan = 15.f ;
    const float bacteriaSize = 2.5f ;
    const float bacteriaMovement = 0.05f ;
    int         mDifference ;
    
    std::vector<BacteriaRef> mBacteria ;
    float mTime ;
    std::vector<BacteriaRef> humanBacteria ;
    //    std::vector<Path2d>     bacPaths ;
    Path2d                   bacPaths ;
    
    //    std::vector<Metaball> balls ;
    ci::Surface8u   mSurface ;
    gl::TextureRef  mTex ;
    
    int NUM_BALLS = 4 ;
    float MIN_SIZE = 8.0f;
    float MAX_SIZE = 20.0f;
    int NUM_BANDS = 2;
    
    
    bool		mSendSerialMessage;
    SerialRef	mSerial;
    uint8_t		mCounter;
    string		mLastString;
    
    gl::TextureRef	mTexture;
    
    double mLastRead, mLastUpdate;
    //    SerialDeviceRef mSerial2 ;
    int         resistance ;
    
    ci::gl::TextureRef      mVideoTexture ;
    ci::qtime::MovieGlRef   mMovie ;
    ci::fs::path            mMoviePath ;
    float                   mVideoOpacity ;
    ci::Timer               mBackgroundTimer ;
    float                   mVideoOpacity2 ;
    ci::Timer               mBackgroundTimer2 ;
    const float             mBackgroundLifespan = 20.f ;
    const float             mBackgroundLifespan2 = 10.f ;
    bool                    mStayOpaque = false ;


};

void BeyondtheSurfaceApp::setup()
{
    for(int i = 0 ; i < Num_Bacteria ; i++) {
        mBacteria.push_back(Bacteria::create(glm::vec2(ci::randFloat(-RANDOM_RANGE, RANDOM_RANGE),ci::randFloat(-RANDOM_RANGE, RANDOM_RANGE)), ci::randFloat(30.f,lifeSpan), glm::vec2(ci::randFloat(0, 1),ci::randFloat(0,1)), bacteriaSize)) ;
    }

    for( const auto &dev : Serial::getDevices() )
        console() << "Device: " << dev.getName() << endl;
    
    try {
        Serial::Device dev = Serial::findDeviceByNameContains( "cu.usbmodem1421" );
        mSerial = Serial::create( dev, 9600 );
    }
    catch( SerialExc &exc ) {
        CI_LOG_EXCEPTION( "coult not initialize the serial device", exc );
        exit( -1 );
    }

    
    //Loading background video
    try {
        mMoviePath = ci::app::getAssetPath("BeyondTheSurfaceBackground.mov") ;
    } catch(std::exception& e) {
        ci::app::console() << e.what() << std::endl ;
    }
    
    try {
        mMovie = ci::qtime::MovieGl::create(mMoviePath) ;
        mMovie->setLoop() ;
        mMovie->play() ;
    } catch(std::exception& e) {
        ci::app::console() << e.what() << std::endl ;
        mMovie.reset() ;
    }
    
}

void BeyondtheSurfaceApp::mouseDown( MouseEvent event )
{
        for(int i = 0 ; i < Num_Human ; i++) {
            humanBacteria.push_back(Bacteria::create(glm::vec2(ci::randFloat(-RANDOM_RANGE, RANDOM_RANGE),ci::randFloat(-RANDOM_RANGE, RANDOM_RANGE)), ci::randFloat(30.f,lifeSpan), glm::vec2(ci::randFloat(0, 1),ci::randFloat(0,1)), bacteriaSize)) ;
        }
}

void BeyondtheSurfaceApp::update()
{
    
    while(mSerial->getNumBytesAvailable() > 0) {
        resistance = mSerial->readByte() ;
    }
    std::cout << resistance << std::endl ;
    
    for(auto& p:mBacteria) {
        p->update() ;
    }
    for(auto& g:humanBacteria) {
        g->update() ;
    }
    
    if(humanBacteria.empty()) {
        if(mBacteria.size() < 750) {
            mDifference = Num_Bacteria-mBacteria.size() ;
            for(int j = 0 ; j < 2 ; j++) {
                mBacteria.push_back(Bacteria::create(glm::vec2(ci::randFloat(-RANDOM_RANGE, RANDOM_RANGE),ci::randFloat(-RANDOM_RANGE, RANDOM_RANGE)), ci::randFloat(30.f,lifeSpan), glm::vec2(ci::randFloat(-RANDOM_FORCE_RANGE, RANDOM_FORCE_RANGE),ci::randFloat(-RANDOM_FORCE_RANGE, RANDOM_FORCE_RANGE)), bacteriaSize)) ;
            }
        }
    }
    
    if(mBacteria.size() < 100) {
        for(auto& p : mBacteria) {
            mBacteria.push_back(Bacteria::create((*p).getPosition(), ci::randFloat(30.f,lifeSpan), glm::vec2(-0.1,0.1), bacteriaSize)) ;
        }
    }
    
    for(auto p = mBacteria.begin() ; p != mBacteria.end();) {
        if((*p)->getTimerLeft() < 0) {
            //            mBacteria.push_back(Bacteria::create((*p)->getPosition(), ci::randFloat(30.f,lifeSpan), glm::vec2(-0.1,0.1), bacteriaSize)) ;
            p = mBacteria.erase(p) ;
        } else {
            ++p ;
        }
    }
    
    //    std::cout << mBacteria.size() << std::endl ;
    
    for(auto g = humanBacteria.begin() ; g != humanBacteria.end();) {
        if((*g)->getTimerLeft() < 0) {
            g = humanBacteria.erase(g) ;
        } else {
            ++g ;
        }
    }
    
    if(resistance > 60) {
        if(humanBacteria.size() < 200) {
            for(int i = 0 ; i < Num_Human ; i++) {
                humanBacteria.push_back(Bacteria::create(glm::vec2(ci::randFloat(-RANDOM_RANGE, RANDOM_RANGE),ci::randFloat(-RANDOM_RANGE,  RANDOM_RANGE)), ci::randFloat(10.f,huLifeSpan), glm::vec2(ci::randFloat(0, 1),ci::randFloat(0,1)), bacteriaSize)) ;
            }
        }
    }
    
    for(int i = 0 ; i < humanBacteria.size() ; i++) {
        for(int j = 0 ; j < mBacteria.size() ; j++) {
            if(humanBacteria[i]->getPosition().x >= mBacteria[j]->getPosition().x-1*humanBacteria[i]->getSize() &&
               humanBacteria[i]->getPosition().x <= mBacteria[j]->getPosition().x+1*humanBacteria[i]->getSize() &&
               humanBacteria[i]->getPosition().y >= mBacteria[j]->getPosition().y-1*humanBacteria[i]->getSize() &&
               humanBacteria[i]->getPosition().y <= mBacteria[j]->getPosition().y+1*humanBacteria[i]->getSize()) {
                mBacteria.erase(mBacteria.begin() + j) ;
                if(humanBacteria[i]->getSize() < 6) {
                    humanBacteria[i]->increaseSize() ;
                    humanBacteria[i]->resetLifespan() ;
                }
            }
        }
    }
    
    
    //Update Video Texture
    if(mMovie) {
        mVideoTexture = mMovie->getTexture() ;
    }

}

void BeyondtheSurfaceApp::draw()
{
	gl::clear( Color( 0, 0, 0 ) );
    
    gl::enableAlphaBlending() ;
//    cout << humanBacteria.size() << endl ;
    if(mVideoTexture && humanBacteria.empty()) {
        //        Rectf centeredRect = Rectf(mVideoTexture->getBounds()) ;
        float opacity = mVideoOpacity2/abs((mBackgroundLifespan2 - mBackgroundTimer2.getSeconds())) ;
        if(mStayOpaque == false) {
            gl::color( ColorA(1.0*opacity,1.0*opacity,1.0*opacity)) ;
            if(opacity > 1) {
                mStayOpaque = true ;
            }
        } else {
            gl::color( ColorA(1.0, 1.0, 1.0)) ;
        }
//        gl::color( ColorA(1.0,1.0,1.0)) ;
        gl::draw(mVideoTexture, getRectf(glm::vec2(0.f, 0.f), glm::vec2(ci::app::getWindowWidth(), ci::app::getWindowHeight()), glm::vec2(1,1))) ;
        mVideoOpacity = 1.f ;
        mBackgroundTimer.start() ;
    } else {
        float opacity = mVideoOpacity*(mBackgroundLifespan-mBackgroundTimer.getSeconds()) ;
        gl::color( ColorA(0.0, 0.25*opacity, 0.25*opacity, 0.5)) ;
        gl::draw(mVideoTexture, getRectf(glm::vec2(0.f, 0.f), glm::vec2(ci::app::getWindowWidth(), ci::app::getWindowHeight()), glm::vec2(1,1))) ;
        mVideoOpacity2 = 1.f ;
        mBackgroundTimer2.start() ;
        mStayOpaque = false ;
    }
    gl::disableAlphaBlending();
    
    {
        gl::ScopedColor rectColor(0,0,0) ;
        gl::drawSolidRect(getRectf(glm::vec2(ci::app::getWindowWidth()-50, 0), glm::vec2 (50, 50), glm::vec2(1,1))) ;
    }
    ci::gl::pushModelMatrix() ;
    gl::translate(ci::app::getWindowCenter()) ;
    for(auto& p:mBacteria) {
        {
            gl::ScopedColor socbyBac(0,0.25*p->getTimerLeft(),1.0, 1.0*p->getTimerLeft()) ;
            p->draw() ;
            p->applyForce(glm::vec2(ci::randFloat(-bacteriaMovement, bacteriaMovement),ci::randFloat(-bacteriaMovement, bacteriaMovement))) ;
        }
    }
    
    
    
    for(auto & g:humanBacteria) {
        {
            gl::ScopedColor humanBac(1.f, 1.f/g->getTimerLeft(), 0.f, 1*g->getTimerLeft()/2) ;
            g->draw() ;
            g->applyForce(glm::vec2(ci::randFloat(-bacteriaMovement, bacteriaMovement),ci::randFloat(-bacteriaMovement, bacteriaMovement))) ;
        }
    }
    
    
    
    gl::popModelMatrix() ;
}

Rectf BeyondtheSurfaceApp::getRectf(glm::vec2 startingPoint, glm::vec2 size, glm::vec2 scale) {
    return Rectf(startingPoint.x, startingPoint.y, (size.x+startingPoint.x) * scale.x, (size.y+startingPoint.y) * scale.y) ;
}

CINDER_APP( BeyondtheSurfaceApp, RendererGl )
