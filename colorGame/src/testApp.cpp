#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    ofToggleFullscreen();
    color = ofColor(255,0,0);
    
}

//--------------------------------------------------------------
void testApp::update(){

}

//--------------------------------------------------------------
void testApp::draw(){
    ofBackground(color);
    
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}

void testApp::mousePressed(int device, int button) {
    mode = (mode+1) % 3;
}

void testApp::mouseScroll(int device, int axis, int delta) {
    cout << device << "\t" << axis << "\t" << delta << endl;
    switch (mode) {
        case 0:
            color.setHue((int)(color.getHue()+5*delta+256) % 256);
            break;
        case 1:
            color.setBrightness((int)(color.getBrightness()+5*delta+256) % 256);
            break;
        case 2:
            color.setSaturation((int)(color.getSaturation()+5*delta+256) % 256);
            break;
    }
    
    
}