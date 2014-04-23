#pragma once

#include "ofMain.h"
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
    
    void updateSource(ofFbo &src,ofFbo &echo);
    
    
    ofVideoPlayer video,video2;

    ofShader blur1Shader,echoShader,blur2Shader,threshShader,screenShader,depthShader,shader;
    
    ofFbo depthFbo,ping,layer1,layer2,echo1,echo2,layer3,src1,src2;
    
    ofParameter<string> fps;
    ofxPanel gui;
    bool bShowGui;
    
    
    
    ofParameter<float> edge0,edge1,alpha,variance1,variance2,scale,hue1,hue2,sat,offset;
    ofParameter<int> radius1,radius2;
		
};
