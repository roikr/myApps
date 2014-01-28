#pragma once

#include "ofMain.h"
#include "ofxiPhone.h"
#include "ofxiPhoneExtras.h"
#include "ofxPointGrab.h"
#include "ofxiVideoGrabber.h"
#include "Slingshot.h"
#include "Slider.h"

class testApp : public ofxiPhoneApp{
	
    public:
        void setup();
        void update();
        void draw();
        void exit();
	
        void touchDown(ofTouchEventArgs & touch);
        void touchMoved(ofTouchEventArgs & touch);
        void touchUp(ofTouchEventArgs & touch);
        void touchDoubleTap(ofTouchEventArgs & touch);
        void touchCancelled(ofTouchEventArgs & touch);

        void lostFocus();
        void gotFocus();
        void gotMemoryWarning();
        void deviceOrientationChanged(int newOrientation);
        
        ofxiVideoGrabber grabber;
        ofTexture tex;
    
        ofxPointGrab pointGrab;
        pointGrabData data;
        float scaleStart;
        ofPoint pos;

        Slingshot slingshot;
        Slider slider;
        ofImage background;
        float lastTime;
    
    ofImage target;
    ofImage grab;
    ofPoint targetPos;
    
};


