#pragma once

#include "ofMain.h"
#include "ofxiOS.h"
#include "ofxiOSExtras.h"
#include "ofxPointGrab.h"
#include "ofxiVideoGrabber.h"

class ofApp : public ofxiOSApp {
	
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
    
        void showIcon(int index);
    
        ofxiVideoGrabber cam;
        ofTexture tex;
    
        ofxPointGrab pointGrab;
        pointGrabData data;
        float scaleStart;
        ofVec2f posStart;
    
    ofxiOSVideoPlayer video;
    vector<string> videos;
    int currentVideo;
    
    ofMatrix4x4 mat;
    
    vector<ofTexture> icons;
    map<int,int> gesturesIndices;
    int iconIndex;
    float iconTime;
    float iconScale;

    
    bool bPaused;
    float volume;
    int step;
    bool bScrub;
    float speed;
    float startTime;
    float startPos;
};


