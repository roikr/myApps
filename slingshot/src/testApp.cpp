#include "testApp.h"


//--------------------------------------------------------------
void testApp::setup(){
    ofSetFrameRate(60);
    ofSetVerticalSync(true);
    ofBackground(100);
    
    slingshot.setup();
    
    lastTime=ofGetElapsedTimef();
    
}

//--------------------------------------------------------------
void testApp::update(){
    slingshot.update(ofGetElapsedTimef()-lastTime,5,1./300.);
    lastTime=ofGetElapsedTimef();

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
        case 'u':
//            slingshot.unload();
            break;
//        case 'g':
//            slingshot.grab();
//            break;
        case 's':
            slingshot.shoot();
            break;
        case 'r':
            slingshot.exitPhysics();
            slingshot.initPhysics();

        default:
            break;
    }
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
   
    
    
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y){
    slingshot.slingshotRoated((float)x/(float)ofGetWidth(), (float)y/(float)ofGetHeight());
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
    slingshot.slingshotStreched((float)y/(float)ofGetHeight());
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