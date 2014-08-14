#pragma once

#include "ofMain.h"
#include "ofxOpenNI2.h"
#include "ofxVideoRecorder.h"
#include "ofxGui.h"
#include "ofxOpticalFlowFarneback.h"

#define CAM_WIDTH 640
#define CAM_HEIGHT 480



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
    ofVideoPlayer video;
    
    ofxOpticalFlowFarneback flowSolver;
    
    ofShader shader;
    ofShader shaderBlurX,shaderBlurY;
    ofShader shaderCool,shaderEcho;
    ofShader blend;
    
    ofImage image;
    
    ofMatrix4x4 mat,imat;
    
    
    
    ofxVideoRecorder recorder;
    bool bRecording;
    string fileName;
    string fileExt;
		
    ofFbo fbo;
    ofFbo ping;
    ofFbo pong;
    ofFbo echo1;
    ofFbo echo2;
    
    
    ofParameter<string> fps;
    ofParameter<float> minEdge,maxEdge,hue1,hue2,satOffset,opacityIn,feedback,feedbackOffset;
    ofParameter<ofVec2f> b0,b1;
    ofxPanel gui;
    bool bHide;
    
    ofVideoPlayer memory1,memory2;
    
};
