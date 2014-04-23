#pragma once

#include "ofMain.h"
#include "ofxGui.h"

#define ECHO_SHADER

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();

		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
        void paramChanged(float &param);
    
    ofShader depthShader,shader;
    ofxPanel gui;
    
    ofFbo depthFbo,ping,pong;
    
    ofVideoPlayer video;
    
    ofParameter<string> fps;
    ofParameter<float> hue,sat,offset;
    ofParameter<float> var,rad;

#ifdef ECHO_SHADER
    ofParameter<float> alpha;
#endif
		
};
