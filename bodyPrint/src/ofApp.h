#pragma once

#include "ofMain.h"
#include "ofxGui.h"
#include "ofxVideoRecorder.h"
#include "ofxOpenCv.h"
#include "ofxOpenNI2.h"

struct layer {
    ofFbo fbo;
    float hueOffset;
};

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
    
        void updateLayer(layer &l,ofFbo &depth,float decay);
        void renderDepth();
    
    
    ofxOpenNI2 cam0;
    ofxOpenNI2 cam1;
    ofTexture depthTex0;
    ofTexture depthTex1;
    ofShader depthShader;
    ofShader subtractShader;
    ofFbo depthFbo,blobFbo,backgroundFbo;
    
    ofParameter<float>minEdge0,maxEdge0,minEdge1,maxEdge1,tolerance;
    
    ofFbo recordFbo; // duplicate depthFbo for freeze and fade
    
    
    
    ofShader strobeShader;
    ofFbo ping,pong;
    ofParameter<float> decay0,decay1,sat,hueRate,offset;
    ofParameter<int> strobeRate;
    int frameNum;
    ofShader blurShader;
    ofParameter<float> variance;
    ofParameter<int> radius;
    
    layer camLayer;
    vector<layer> layers;
    ofFbo compFbo;
    ofShader compShader;
    
    ofxCvGrayscaleImage grayImg;
    ofxCvContourFinder contour;
    ofParameter<float> minArea,maxArea;
    ofParameter<bool>blobDetection;
    
    
    
    ofxVideoRecorder recorder;
    
    float startTime;
    ofParameter<string> videoQueue;
    ofParameter<float> recordDuration,waitDuration,freezeDuration,idleInterval,minimumDuration;
    float recordTimer,waitTimer,idleTimer;
    ofParameter<string> recordTime,waitTime;
    string filename;
    
    vector<string> memories;
    vector<ofVideoPlayer> players;
    
    
    
    
    ofParameter<string> fps;
    ofxPanel gui;
    
    ofMatrix4x4 mat;
    
    int state;
    
    
    bool bFirstIdle;
    
    ofSoundPlayer recSound;
    float fadeTime;
};
