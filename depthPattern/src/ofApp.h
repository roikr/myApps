#pragma once

#include "ofMain.h"
#include "ofxOpenNI2.h"
#include "ofxOpenCv.h"
#include "ofxGui.h"

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
    ofTexture depthTexture;
    ofShader shader;
    ofFbo patternsFbo,shapesFbo;
    
    ofParameter<float> patternsMin,patternsMax,patternsThreshVal,shapesMin,shapesMax,shapesThreshVal,minArea,maxArea;
    ofParameter<bool> patternsInv,patternsThresh,shapesInv,shapesThresh;
    ofParameter<string> fps;
    ofxPanel gui;
    
    ofxCvContourFinder patternsBlobs,shapesBlobs;
    ofxCvGrayscaleImage patternsImg,shapesImg;
    
};
