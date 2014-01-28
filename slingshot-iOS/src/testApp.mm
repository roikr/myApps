#include "testApp.h"
#include "Settings.h"

#define STRECH_CONSTANT 10.0f

//--------------------------------------------------------------
void testApp::setup(){
    ofSetFrameRate(30);
      
	// initialize the accelerometer
	ofxAccelerometer.setup();
	   
    
    if (!ofFile(ofxiPhoneGetDocumentsDirectory() + "pg_user.ini").exists()) {
        ofFile src(ofToDataPath("pg_user.ini"));
        src.copyTo(ofxiPhoneGetDocumentsDirectory() + "pg_user.ini");
        cout << "copy default pg_user.ini" << endl;
               
    }
    
    
    if (Settings::getBool("update_preference")) {
        
        ofHttpResponse response = ofLoadURL(Settings::getString("url_preference")+"/pg_user.ini");
        if (response.status==200) {
            Settings::setBool(false,"update_preference");
            ofFile file;
            file.open(ofxiPhoneGetDocumentsDirectory() + "pg_user.ini", ofFile::WriteOnly);
            file << response.data;
            file.close();
            
            cout << "pg_user.ini downloaded" << endl;
        }
    }
    
    [UIApplication sharedApplication].idleTimerDisabled = NO;
    [UIApplication sharedApplication].idleTimerDisabled = YES;
	
    
    //ofDisableArbTex();
    
	ofBackground(64);
    ofEnableAlphaBlending();
    
    ofSetOrientation(OF_ORIENTATION_90_LEFT);
    
    slingshot.setup();
    
    if([UIDevice currentDevice].userInterfaceIdiom == UIUserInterfaceIdiomPad)
    {
        if (ofxiOSGetOFWindow()->isRetinaEnabled()) {
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
    pointGrab.setup(ofxiPhoneGetDocumentsDirectory(),true);
    
    grabber.initGrabber(640, 480);
    grabber.setOrientation(OF_ORIENTATION_90_LEFT,true);
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
        
        slingshot.setTrackable(this->data.bIsTrackable);
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
    ofSetColor(255);
    background.draw(0,0,ofGetWidth(),ofGetHeight());
    
    ofPushMatrix();
    ofTranslate(20, 20);
    float scale = ofGetWidth()/tex.getWidth()/4;
    ofScale(scale, scale,1);
//    ofPushMatrix();
//    ofScale(-1, 1);
//    ofTranslate(-tex.getWidth(),0);
    ofSetColor(255);
    tex.draw(0, 0);
//    ofPopMatrix();
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

