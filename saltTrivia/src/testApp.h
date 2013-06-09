#pragma once

#include "ofMain.h"

struct slide {
    vector<ofTexture>layers;
    int correct;
};

struct layout {
    vector<ofRectangle> rects;
};

struct slideshow {
    vector<slide> slides;
    ofRectangle next;
    ofRectangle previous;
    vector<layout>layouts;
    
};

class testApp : public ofBaseApp{
	public:
		void setup();
		void update();
		void draw();
        void exit();
		
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y);
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
   
    int currentSlideshow;
    int current;
    
    vector<slideshow > slideshows;
    vector<int> layers;
    
    ofMatrix4x4 mat;
    ofMatrix4x4 imat;
//    vector<ofVec3f> downPos;
    
    int delay;
    int reset;
    float delayTimer;
    float resetTimer;
    int resetState;
    
    ofImage fingerprint;
    
    
};
