#pragma once

#include "ofMain.h"
#include "ofxOsc.h"


enum {
    MODE_LAYOUT,
    MODE_WARP,
    MODE_CONTENT
};

struct sink {
    
    int width;
    int height;
    int orientation;
    
    
    string name;
    
    vector<pair<ofPoint,ofPoint> > corners;
    
    
    ofxOscSender *sender;
    string host;
    int port;
    
    ofMatrix4x4 mat;
    ofMatrix4x4 imat;
    
    ofPoint position;
    ofRectangle rect;
    vector<ofRectangle> intersections;
};

class testApp : public ofBaseApp{
	public:
		void setup();
		void update();
		void draw();
        void exit();
    
        void loadPrefs();
        void savePrefs();

        void updateSinks();
		
		void keyPressed(int key);
		void keyReleased(int key);
		void mouseMoved(int x, int y);
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		void dragEvent(ofDragInfo dragInfo);
		void gotMessage(ofMessage msg);
    
    vector<sink> sinks;
    bool bSelected;
    
    bool bCorner;
    int corner;
    
    ofPoint lastPos;
    
    
    int mode;
    bool bZoom;
    
    ofMatrix4x4 defMat;
    ofMatrix4x4 mat;
    ofMatrix4x4 imat;
    
    ofImage image;
    
    
};
