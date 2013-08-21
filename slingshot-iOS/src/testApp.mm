#include "testApp.h"

#define STRECH_CONSTANT 10.0f

//--------------------------------------------------------------
void testApp::setup(){
    ofSetFrameRate(30);
      
	// initialize the accelerometer
	ofxAccelerometer.setup();
	   
  

    
    [UIApplication sharedApplication].idleTimerDisabled = NO;
    [UIApplication sharedApplication].idleTimerDisabled = YES;
	
    
    //ofDisableArbTex();
    
	ofBackground(64);
    ofEnableAlphaBlending();
    
    iPhoneSetOrientation(OFXIPHONE_ORIENTATION_PORTRAIT);
    
    slingshot.setup();
    
    if([UIDevice currentDevice].userInterfaceIdiom == UIUserInterfaceIdiomPad)
    {
        if (iPhoneGetOFWindow()->isRetinaEnabled()) {
            slingshot.retinaScale = 0.5f;
        }
    }
    
    //	//If you want a landscape oreintation
//	iPhoneSetOrientation(OFXIPHONE_ORIENTATION_LANDSCAPE_RIGHT);
  
   
    
    slider.setup(ofRectangle(ofGetWidth()-100,0,100,ofGetHeight()));
    
    cout << ofGetWidth() << "\t" << ofGetHeight() << endl;
    background.loadImage("SlingShot_Backg.png");
    target.loadImage("Target.png");
    target.setAnchorPercent(0.5, 0.5);
    grab.loadImage("Target_Grab.png");
    grab.setAnchorPercent(0.5, 0.5);
    lastTime = ofGetElapsedTimef();
    
    data.bIsTrackable = false;
    data.bIsGrab = false;
    scaleStart = 1.0f;
    pointGrab.setup();
    
    grabber.initGrabber(640, 480);
    tex.allocate(640, 480, GL_LUMINANCE);
    
}

//--------------------------------------------------------------
void testApp::update(){
    
    ofPixels &pixels = pointGrab.getNextPixels();
    if (grabber.copyPixels(pixels)) {
        tex.loadData(pixels);
        pointGrabData data = pointGrab.update();
        
        if (!this->data.bIsGrab && data.bIsGrab) {
            scaleStart = data.scale;
            slingshot.load();
        }
        
        if (this->data.bIsGrab && !data.bIsGrab) {
            slingshot.shoot();
        }
        
        this->data = data;
        
        if (this->data.bIsTrackable) {
            pos+=this->data.delta;
        } else {
            pos = ofPoint(0,0);
        }
        
    }
    
   
       
    if (data.bIsTrackable) {
        slingshot.slingshotRoated(-pos/1000); // -1 mostright, 1 - mostleft,
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
    
    ofPushMatrix();
    ofTranslate(20, 20);
    float scale = ofGetWidth()/tex.getWidth()/4;
    ofScale(scale, scale);
    ofPushMatrix();
    ofScale(-1, 1);
    ofTranslate(-tex.getWidth(),0);
    tex.draw(0, 0);
    ofPopMatrix();
    if (data.bIsTrackable) {
        ofTranslate(data.camRefPoint);
        ofScale(2, 2);
        if (data.bIsGrab) {
            grab.draw(0, 0);
        } else {
            target.draw(0, 0);
        }
    }
    ofPopMatrix();
    
    glEnable(GL_DEPTH_TEST);
    slingshot.draw();
    glDisable(GL_DEPTH_TEST);
   
    ofSetColor(255);
    
    
    
    slider.draw();
    
    ofPushMatrix();
    ofTranslate(ofGetWidth()-200, 30);
    ofScale(2.0, 2.0);
    
    ofDrawBitmapString(ofToString(ofGetFrameRate(),2),0,0);
    ofDrawBitmapString(ofToString(grabber.getFPS(),2),0,20);
    ofDrawBitmapString(ofToString(log(scaleStart/data.scale), 2)+","+ofToString(data.scale, 2), 0, 40);
    ofDrawBitmapString(ofToString(data.delta.x)+","+ofToString(data.delta.y),0,60);
    ofPopMatrix();
}

//--------------------------------------------------------------
void testApp::exit(){
    pointGrab.exit();
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

