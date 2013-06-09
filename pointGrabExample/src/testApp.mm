#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
    data.bIsTrackable = false;
    data.bIsGrab = false;
	// initialize the accelerometer
	ofxAccelerometer.setup();
	ofxRegisterPointGrabNotification(this);
    ofxPointGrab.setup();
    ofxPointGrab.setPreviewRect(ofRectangle(10,10,320,240));
    //ofxRegisterPointGrabEvents(this);
//	//If you want a landscape oreintation 
	iPhoneSetOrientation(OFXIPHONE_ORIENTATION_PORTRAIT);
    
    [UIApplication sharedApplication].idleTimerDisabled = NO;
    [UIApplication sharedApplication].idleTimerDisabled = YES;
	
	ofBackground(127,127,127);
   
    
    cout << ofGetWidth() << "\t" << ofGetHeight() << endl;
    scaleStart = 1.0f;
}

//--------------------------------------------------------------
void testApp::update(){
    
    while (!trackableEvents.empty()) {
        cout << "trackable: " <<trackableEvents.back().bIsTrackable << endl;
    
        data = trackableEvents.back();
        trackableEvents.pop_back();
        
    }
    
    while (!grabEvents.empty()) {
        cout << "grab: " <<grabEvents.back().bIsGrab << endl;
        
        if (grabEvents.back().bIsGrab) {
            scaleStart = grabEvents.back().scale;
        }         
        data = grabEvents.back();
        grabEvents.pop_back();
        
    }
    
    
    
    
    
}

//--------------------------------------------------------------
void testApp::draw(){
    ofPoint pos = data.camRefPoint;
    pos.x = ofMap(pos.x, 0, 640.0, 0, ofGetWidth());
    pos.y = ofMap(pos.y, 0, 480.0, 0, ofGetHeight());
    ofCircle(pos, 10);
	
    ofDrawBitmapString(ofToString(scaleStart, 2)+" / "+ofToString(data.scale, 2), ofGetWidth()-150, 30);
    
    ofDrawBitmapString(ofToString(log(scaleStart/data.scale), 2),ofGetWidth()-150, 50);
}

//--------------------------------------------------------------
void testApp::exit(){
    ofxPointGrab.exit();
}

void testApp::newFrame(pointGrabData &data) {
    if (this->data.bIsTrackable!=data.bIsTrackable) {
        trackableEvents.push_front(data);
    }
    
    if (this->data.bIsGrab!=data.bIsGrab) {
        grabEvents.push_front(data);
    }
    
//    cout << data.bIsTrackable << '\t' << data.scale << endl;;
    
    this->data = data;
    
}
//--------------------------------------------------------------
void testApp::touchDown(ofTouchEventArgs & touch){
    cout << touch.x << "\t" << touch.y << endl;
}

//--------------------------------------------------------------
void testApp::touchMoved(ofTouchEventArgs & touch){

}

//--------------------------------------------------------------
void testApp::touchUp(ofTouchEventArgs & touch){

}

//--------------------------------------------------------------
void testApp::touchDoubleTap(ofTouchEventArgs & touch){

}

//--------------------------------------------------------------
void testApp::touchCancelled(ofTouchEventArgs & touch){
    
}

//--------------------------------------------------------------
void testApp::lostFocus(){

}

//--------------------------------------------------------------
void testApp::gotFocus(){

}

//--------------------------------------------------------------
void testApp::gotMemoryWarning(){

}

//--------------------------------------------------------------
void testApp::deviceOrientationChanged(int newOrientation){

}

