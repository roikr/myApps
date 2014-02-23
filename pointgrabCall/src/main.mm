#include "ofMain.h"
#include "ofApp.h"

int main(){
//    ofSetLogLevel(OF_LOG_VERBOSE);
    ofAppiOSWindow *window = new ofAppiOSWindow();
    window->enableHardwareOrientation();
    window->enableRetina();
	ofSetupOpenGL(window,1024,768,OF_FULLSCREEN);			// <-------- setup the GL context

	ofRunApp(new ofApp());
}
