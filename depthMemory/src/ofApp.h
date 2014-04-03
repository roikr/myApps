#pragma once

#include "ofMain.h"
#include "ofxOpenNI2.h"
#include "ofxOpenCv.h"
#include "ofxGui.h"
#include "ofxVideoRecorder.h"


struct memory {
    ofVec2f pos;
    string filename;
};

class ofApp : public ofBaseApp{

	public:
		void setup();
		void update();
		void draw();
        void exit();

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
    ofTexture depthTexture;
    
    
    ofShader shader;
    ofShader thresholdShader;
    
    ofFbo fbo,thresholdFbo;
    
    ofParameter<float> minEdge,maxEdge,minArea,maxArea,startArea,stopArea;
    ofParameter<string> queueSize;
    
    ofxPanel gui;
    
    ofxCvGrayscaleImage grayImg;
    ofxCvContourFinder contour;
    
    deque<ofPixels> frames;
    
    bool bRecording;
    ofxVideoRecorder recorder;
    float startTime,stopTime;
    bool bReset; // wait for blob to disappear after last recording
    memory currentMemory;
    
    vector<memory> memories;
    
};
