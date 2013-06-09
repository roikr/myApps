#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    ofSetLogLevel(OF_LOG_SILENT);
}

//--------------------------------------------------------------
void testApp::update(){

}

//--------------------------------------------------------------
void testApp::draw(){

}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y){
    cout << "mouseMoved\t" << x << "\t" << y << endl;
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
    cout << "mouseDragged\t" << x << "\t" << y << "\t" << button << endl;
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
    cout << "mousePressed\t" << x << "\t" << y << "\t" << button << endl;
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
    cout << "mouseReleased\t" << x << "\t" << y << "\t" << button << endl;
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