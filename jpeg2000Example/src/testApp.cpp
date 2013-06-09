#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    
    unsigned long long time = ofGetElapsedTimeMillis();
	jp2Image.loadImage("images/Rome.jp2");
    cout << ofGetElapsedTimeMillis() - time << endl;
    //jp2Image.setUseTexture(false);
	
}

//--------------------------------------------------------------
void testApp::update(){
	ofBackground(255);	
}

//--------------------------------------------------------------
void testApp::draw(){	
	ofSetColor(255);

	jp2Image.draw(0, 0);
	
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
