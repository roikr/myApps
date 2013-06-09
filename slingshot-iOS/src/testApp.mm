#include "testApp.h"

#define STRECH_CONSTANT 10.0f

enum {
    EVENT_LOAD,
    EVENT_GRAB,
    EVENT_RELEASE,
    EVENT_UNLOAD
};

//--------------------------------------------------------------
void testApp::setup(){
    ofSetFrameRate(30);
    data.bIsTrackable = false;
    data.bIsGrab = false;
  
	// initialize the accelerometer
	ofxAccelerometer.setup();
	ofxRegisterPointGrabNotification(this);
    ofxPointGrab.setup();
//    ofxPointGrab.setPreviewRect(ofRectangle(10,10,ofGetWidth()/3,0.75*ofGetWidth()/3));
    ofxPointGrab.setPreviewRect(ofRectangle(50/2,50/2,550/2,413/2));
  
//	//If you want a landscape oreintation 
	iPhoneSetOrientation(OFXIPHONE_ORIENTATION_PORTRAIT);
    
    [UIApplication sharedApplication].idleTimerDisabled = NO;
    [UIApplication sharedApplication].idleTimerDisabled = YES;
	
    
    //ofDisableArbTex();
    
	ofBackground(64);
    ofEnableAlphaBlending();
    
    slingshot.setup();
    if (iPhoneGetOFWindow()->isRetinaEnabled()) {
        slingshot.retinaScale = 0.5f;
    }
  
   
    
    slider.setup(ofRectangle(ofGetWidth()-100,0,100,ofGetHeight()));
    
    cout << ofGetWidth() << "\t" << ofGetHeight() << endl;
    background.loadImage("SlingShot_Backg.png");
    
}

//--------------------------------------------------------------
void testApp::update(){
    
    
    
    while (!events.empty()) {
        cout << "event: " << events.back().first << endl;
        switch (events.back().first) {
            case EVENT_LOAD:
                slingshot.load();
                break;
            case EVENT_GRAB:
                slingshot.grab();
                scaleStart = events.back().second.scale;
                break;
            case EVENT_RELEASE:
                slingshot.updateScale(ofClamp(STRECH_CONSTANT*slider.getValue()*log(scaleStart/events.back().second.scale), 0, 1));
                slingshot.release();
                break;
            case EVENT_UNLOAD:
                slingshot.release();
                break;
        }
        data = events.back().second;
        events.pop_back();
                
    }
    
    
    if (data.bIsGrab) {
        slingshot.updateScale(ofClamp(STRECH_CONSTANT*slider.getValue()*log(scaleStart/data.scale), 0, 1));
    }
    
    
    slingshot.update();
}

//--------------------------------------------------------------
void testApp::draw(){
    glDisable(GL_DEPTH_TEST);
    background.draw(0,0,ofGetWidth(),ofGetHeight());
    glEnable(GL_DEPTH_TEST);
    
    slingshot.draw();
    glDisable(GL_DEPTH_TEST);
    
    ofSetColor(255);
    
//    ofPoint pos = data.camRefPoint;
//    pos.x = ofMap(pos.x, 0, 640.0, 0, ofGetWidth());
//    pos.y = ofMap(pos.y, 0, 480.0, 0, ofGetHeight());
//    ofCircle(pos, 10);
    
    slider.draw();
    
    ofDrawBitmapString(ofToString(ofGetFrameRate(),2), ofGetWidth()-150, 30);
    ofDrawBitmapString(ofToString(log(scaleStart/data.scale), 2)+","+ofToString(data.scale, 2), ofGetWidth()-150, 50);
    ofDrawBitmapString(ofToString(data.camRefPoint.x)+","+ofToString(data.camRefPoint.y), ofGetWidth()-150, 70);
}

//--------------------------------------------------------------
void testApp::exit(){
    ofxPointGrab.exit();
}

void testApp::newFrame(pointGrabData &data) {
    if (!this->data.bIsTrackable && data.bIsTrackable) {
        events.push_front(make_pair(EVENT_LOAD,data));
    }
    
//    if (!this->data.bIsGrab && data.bIsGrab) {
//        events.push_front(make_pair(EVENT_GRAB,data));
//    }
//    
//    if (this->data.bIsGrab && !data.bIsGrab) {
//        events.push_front(make_pair(EVENT_RELEASE,data));
//    }
    
    
    if (this->data.bIsTrackable && !data.bIsTrackable) {
        events.push_front(make_pair(EVENT_UNLOAD,data));
    }
    
    
    
    this->data = data;
    
}

//--------------------------------------------------------------
void testApp::touchDown(ofTouchEventArgs & touch){
    
//     slingshot.grab();
    slider.touchDown(touch);
}

//--------------------------------------------------------------
void testApp::touchMoved(ofTouchEventArgs & touch){
//    cout << (float)touch.y/ofGetHeight() << endl;
//    slingshot.updateScale((float)touch.y/ofGetHeight());
    slider.touchMoved(touch);
}

//--------------------------------------------------------------
void testApp::touchUp(ofTouchEventArgs & touch){
    slider.touchUp(touch);
//    slingshot.release();
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

