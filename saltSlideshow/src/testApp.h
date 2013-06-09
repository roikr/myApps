#pragma once

#include "ofMain.h"

class testApp : public ofBaseApp{
	public:
		void setup();
		void update();
		void draw();
    
//        void loadImage();
        void loadTexture();
//        void exit();
		
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y);
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
    vector<ofPixels> english;
    vector<ofPixels> hebrew;
    
//    vector<string> english;
//    vector<string> hebrew;
    
    int start;
    int current;
    float delayTimer;
    float resetTimer;
    
    bool bLangEng;
    bool bManual;
    
    ofMatrix4x4 mat;
    int delay;
    int reset;
    
    int width;
    int height;
    
//    ofImage image;
    ofTexture tex;
};
