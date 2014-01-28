#pragma once

#include "ofMain.h"
#include "ofxOpenNI2.h"

#define CAM_WIDTH 640
#define CAM_HEIGHT 480


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
    
    ofxOpenNI2 cam;
    ofShader shader;
    
    ofImage image;
    
    ofMatrix4x4 mat,imat;
    
    ofVec2f downPos;
    float minEdge;
    float maxEdge;
		
};
