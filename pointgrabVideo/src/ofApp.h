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
        void hideIcon();
    
    ofxiVideoGrabber cam;
    ofTexture tex;
    ofMatrix4x4 camMat,camiMat;
    bool bShowCam;

    ofxPointGrab pointGrab;
    pointGrabData data;
    
    ofxiOSVideoPlayer video;
    vector<string> videos;
    int currentVideo;
    
    ofMatrix4x4 mat;
    
    vector<ofTexture> icons;
    map<int,int> gesturesIndices;
    int iconIndex;
    float iconTime;
    ofMatrix4x4 iconMat;
   
    bool bPaused;
    float volume;
    bool bMute;
    int step;
    bool bScrub;
    float speed;
    float startTime;
    float startPos;
    int scrollCount;
    
    ofTexture mute;
};


