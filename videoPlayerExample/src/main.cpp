#include "ofMain.h"
#include "ofApp.h"

//========================================================================
int main( ){
#ifdef TARGET_OSX
    ofSetupOpenGL(1024,768, OF_WINDOW);
#else
    ofSetupOpenGL(1024,768, OF_FULLSCREEN);			// <-------- setup the GL context
#endif
	

	// this kicks off the running of my app
	// can be OF_WINDOW or OF_FULLSCREEN
	// pass in width and height too:
	ofRunApp( new ofApp());

}
