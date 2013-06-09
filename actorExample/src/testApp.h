#pragma once

#include "ofMain.h"
#include "singer.h"
#include "player.h"
#include "ofxGamepadHandler.h"

class testApp : public ofBaseApp{
	public:
		void setup();
		void update();
		void draw();
    
        void axisChanged(ofxGamepadAxisEvent &e);
        void buttonPressed(ofxGamepadButtonEvent &e);
        void buttonReleased(ofxGamepadButtonEvent &e);
	
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y);
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
    player tom;
    singer dolly;
    player blank;
    
    ofImage background;
            
};
