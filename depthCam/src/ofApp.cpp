#include "ofApp.h"
#include "Shaders.h"

#define STRINGIFY(A) #A
//--------------------------------------------------------------
void ofApp::setup(){
    ofDisableArbTex();
    
    
    
    cam.setup();
    cam.listDepthModes();
    cam.setDepthMode(5);
    depthTex.allocate(cam.depthWidth, cam.depthHeight, GL_R16 );
    
    
    
    
    
    
    
    


}

//--------------------------------------------------------------
void ofApp::update(){
    ofSetWindowTitle(ofToString(ofGetFrameRate()));
    
    cam.update();
    
    
	if(cam.bNewDepth) {
        depthTex.loadData(cam.getDepth(), cam.depthWidth, cam.depthHeight,GL_RED);
        
        
        
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofSetColor(255);
    ofClear(0);
    
    depthTex.draw(0, 0);
    
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
