#pragma once

#include "ofMain.h"
#include "ofxiOS.h"
#include "ofxiOSExtras.h"
#include "ofxPointGrab.h"



struct clip {
    string name;
    float start;
    float end;
    vector<float> markers;
    bool bMute;
    
};

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
    
        void play(string name);
        
    
    ofVideoGrabber cam;
    ofMatrix4x4 camMat,camiMat;
    bool bShowCam;
    ofxPointGrab pointGrab;
    pointGrabData data;
    ofxiOSVideoPlayer video;
    ofFbo fbo;
    ofPixels pixels;
    ofMatrix4x4 fboMat;
    
    vector<clip> clips;
    vector<clip>::iterator current;
    
    vector<ofTexture> icons;
    map<int,int> gesturesIndices;
    map<int,int> palmTypeIndices;
    int iconIndex;
    float iconTime;
    ofMatrix4x4 iconMat;
    
    ofTexture background;
    
    ofMatrix4x4 mat;
    
    

};


