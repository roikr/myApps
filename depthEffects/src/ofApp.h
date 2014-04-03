#pragma once

#include "ofMain.h"
#include "ofxOpenNI2.h"
#include "ofxVideoRecorder.h"
#include "ofxGui.h"

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
    
    ofShader shader;
    ofShader shaderBlurX,shaderBlurY;
    ofShader shaderCool,shaderEcho;
    
    ofImage image;
    
    ofMatrix4x4 mat,imat;
    
    
    
    ofxVideoRecorder recorder;
    bool bRecording;
    string fileName;
    string fileExt;
		
    ofFbo fbo;
    ofFbo ping;
    ofFbo pong;
    ofFbo echo;
    
    
    ofParameter<float> minEdge,maxEdge,blur,blurOffset,hue,satOffset,opacityIn,feedback,feedbackOffset;
    ofxPanel gui;
    bool bHide;
    
};
