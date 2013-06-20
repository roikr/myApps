#include "testApp.h"

#define STRECH_CONSTANT 10.0f

enum {
    EVENT_LOAD,
    EVENT_SHOOT,
    
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
    
   
   
  
//	//If you want a landscape oreintation 
	iPhoneSetOrientation(OFXIPHONE_ORIENTATION_PORTRAIT);
    
    [UIApplication sharedApplication].idleTimerDisabled = NO;
    [UIApplication sharedApplication].idleTimerDisabled = YES;
	
    
    //ofDisableArbTex();
    
	ofBackground(64);
    ofEnableAlphaBlending();
    
    slingshot.setup();
    
    if([UIDevice currentDevice].userInterfaceIdiom == UIUserInterfaceIdiomPad)
    {
        ofxPointGrab.setPreviewRect(ofRectangle(50/2,50/2,550/2,413/2));
        if (iPhoneGetOFWindow()->isRetinaEnabled()) {
            slingshot.retinaScale = 0.5f;
        }
    }
    else
    {
        ofxPointGrab.setPreviewRect(ofRectangle(50/2,50/2,120,90));
    }
    
  
   
    
    slider.setup(ofRectangle(ofGetWidth()-100,0,100,ofGetHeight()));
    
    cout << ofGetWidth() << "\t" << ofGetHeight() << endl;
    background.loadImage("SlingShot_Backg.png");
    lastTime = ofGetElapsedTimef();
    
    
}

//--------------------------------------------------------------
void testApp::update(){
    
   
    while (!events.empty()) {
         cout << "event: " << events.back().first << endl;
        switch (events.back().first) {
            
            case EVENT_LOAD:
                slingshot.load();
                scaleStart = events.back().second.scale;
                break;
            case EVENT_SHOOT:
                slingshot.shoot();
                break;
        }

        
        data = events.back().second;
        events.pop_back();
                
    }
    
    if (data.bIsTrackable) {
        slingshot.slingshotRoated(data.camRefPoint.x/640,data.camRefPoint.y/480);
    }
    
    if (data.bIsGrab) {
//        slingshot.updateScale(ofClamp(STRECH_CONSTANT*slider.getValue()*log(scaleStart/data.scale), 0, 1));
        slingshot.slingshotStreched(ofClamp(STRECH_CONSTANT*slider.getValue()*log(scaleStart/data.scale), 0, 1));
        
    }
    
    slingshot.update(ofGetElapsedTimef()-lastTime,5,1./150.);
    lastTime = ofGetElapsedTimef();
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
    
    ofPushMatrix();
    ofTranslate(ofGetWidth()-200, 30);
    ofScale(2.0, 2.0);
    
    ofDrawBitmapString(ofToString(ofGetFrameRate(),2),0,0);
    ofDrawBitmapString(ofToString(log(scaleStart/data.scale), 2)+","+ofToString(data.scale, 2), 0, 20);
    ofDrawBitmapString(ofToString(data.camRefPoint.x)+","+ofToString(data.camRefPoint.y),0,40);
    ofPopMatrix();
}

//--------------------------------------------------------------
void testApp::exit(){
    ofxPointGrab.exit();
}

void testApp::newFrame(pointGrabData &data) {
       
    if (!this->data.bIsGrab && data.bIsGrab) {
        events.push_front(make_pair(EVENT_LOAD,data));
    }

    if (this->data.bIsGrab && !data.bIsGrab) {
        events.push_front(make_pair(EVENT_SHOOT,data));
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
    slingshot.exitPhysics();
    slingshot.initPhysics();
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

