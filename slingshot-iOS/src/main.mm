#include "ofMain.h"
#include "testApp.h"
#include "ofAppiOSWindow.h"

int main(){
    ofAppiOSWindow *window = new ofAppiOSWindow();
    window->enableHardwareOrientation();
    window->enableRetina();
    window->enableDepthBuffer();
    
	ofSetupOpenGL(window,1024,768, OF_FULLSCREEN);
	ofRunApp(new testApp);
}
