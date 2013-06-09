#include "testApp.h"
#include "utils.h"

//--------------------------------------------------------------
void testApp::setup(){
    receiver.setup(3000);
    
   
    ofEnableAlphaBlending();
    image.loadImage("checkers.jpg");
}

//--------------------------------------------------------------
void testApp::update(){
    while (receiver.hasWaitingMessages()) {
        ofxOscMessage m;
        receiver.getNextMessage(&m);
        
        
        if (m.getAddress()=="/warp") {
            for (int i=0;i<16;i++) {
                warp.getPtr()[i] = m.getArgAsFloat(i);
            }
            
            
             
         } else if (m.getAddress()=="/xform") {
             for (int i=0;i<16;i++) {
                 xform.getPtr()[i] = m.getArgAsFloat(i);
             }
             
         } else if (m.getAddress()=="/clear") {
             intersections.clear();
             
         } else if (m.getAddress()=="/intersection") {
             intersections.push_back(ofRectangle(m.getArgAsInt32(0),m.getArgAsInt32(1),m.getArgAsInt32(2),m.getArgAsInt32(3)));
             
         }
    }
}

//--------------------------------------------------------------
void testApp::draw(){
    ofPushMatrix();
    ofMultMatrix(warp);
    
    ofPushStyle();
    ofPushMatrix();
    ofMultMatrix(xform);
    image.draw(0,0);
    ofPopMatrix();
    
    ofPushStyle();
    ofFill();
    ofSetColor(255, 255, 0,50);
    for (vector<ofRectangle>::iterator iter=intersections.begin(); iter!=intersections.end(); iter++) {
        ofRect(*iter);
    }
    
    ofNoFill();
    ofSetColor(0, 0, 255);
    
//    for (vector<ofPoint>::iterator iter=corners.begin();iter!=corners.end();iter++) {
//        
//     
//        ofRectangle rect;
//        rect.setFromCenter(*iter, 10, 10);
//        ofRect(rect);
//        
//    }
    ofPopStyle();
    ofPopMatrix();
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    ofToggleFullscreen();
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y){
//    cout << "mouseMoved\t" << x << "\t" << y << endl;
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
//    cout << "mouseDragged\t" << x << "\t" << y << "\t" << button << endl;
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
//    cout << "mousePressed\t" << x << "\t" << y << "\t" << button << endl;
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
//    cout << "mouseReleased\t" << x << "\t" << y << "\t" << button << endl;
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