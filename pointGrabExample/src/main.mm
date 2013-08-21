#include "ofMain.h"
#include "testApp.h"

int main(){
   
	ofSetupOpenGL(1024,768, OF_FULLSCREEN);			// <-------- setup the GL context
     iPhoneGetOFWindow()->enableRetina();
    iPhoneGetOFWindow()->setOrientation(OF_ORIENTATION_90_RIGHT);
	ofRunApp(new testApp);
}
