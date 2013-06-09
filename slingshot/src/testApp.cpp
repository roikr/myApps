#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup(){
    ofSetFrameRate(60);
    ofSetVerticalSync(true);
    ofBackground(100);
    
    slingshot.setup();
    
    
}

//--------------------------------------------------------------
void testApp::update(){
    slingshot.update();
    slingshot.updateScale((float)ofGetMouseY()/(float)ofGetHeight());
}

//--------------------------------------------------------------
void testApp::draw(){
    glEnable(GL_DEPTH_TEST);
    slingshot.draw();
}

void testApp::exit() {
    
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    switch (key) {
        case 'l':
            slingshot.load();
            break;
        case 'g':
            slingshot.grab();
            break;
        case 'r':
            slingshot.release();
            break;

        default:
            break;
    }
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