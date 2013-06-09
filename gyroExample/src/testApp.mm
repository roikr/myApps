#include "testApp.h"

#import <CoreMotion/CoreMotion.h>
#define IMAGE_WIDTH 4543
#define IMAGE_HEIGHT 3319

static CMMotionManager *motionManager;


//--------------------------------------------------------------
void testApp::setup(){	
	
	
	//If you want a landscape oreintation 
	//iPhoneSetOrientation(OFXIPHONE_ORIENTATION_LANDSCAPE_RIGHT);
    
    ofEnableSmoothing();
//	glEnable(GL_DEPTH_TEST);

	
    // ofBox uses texture coordinates from 0-1, so you can load whatever
	// sized images you want and still use them to texture your box
	// but we have to explicitly normalize our tex coords here
	ofEnableNormalizedTexCoords();

    
    motionManager = [[CMMotionManager alloc] init];
    motionManager.showsDeviceMovementDisplay = YES;
    motionManager.deviceMotionUpdateInterval = 1.0/60.0;
    //[motionManager startDeviceMotionUpdatesUsingReferenceFrame:CMAttitudeReferenceFrameXTrueNorthZVertical];
    [motionManager startDeviceMotionUpdatesUsingReferenceFrame:CMAttitudeReferenceFrameXArbitraryZVertical];
    
	ofBackground(127,127,127);
   
    
    
    //image.loadImage("image.jpeg");
    mask.loadImage("mask.png");
    ofEnableAlphaBlending();
    //image.setAnchorPercent(0.5, 0.5);
    image.setup("17100u", 1024,IMAGE_WIDTH,IMAGE_HEIGHT);
    image.load();
}

//--------------------------------------------------------------
void testApp::update(){
    
    
    
    
    
    
    
   
}

//--------------------------------------------------------------
void testApp::draw(){
//    cam.begin();
//    
//    CMDeviceMotion *d = motionManager.deviceMotion;
//    if (d!=nil) {
//        CMRotationMatrix m = d.attitude.rotationMatrix;
//        
//        ofMatrix4x4 mat(m.m11,m.m21,m.m31,0.0f,m.m12,m.m22,m.m32,0.0f,m.m13,m.m23,m.m33,0.0f,0.0f,0.0f,0.0f,1.0f);
//        
//        ofMultMatrix(mat);
//        
//    }
//    
//    //ofSphere(100);
//    ofSetColor(255);
//    ofFill();
//	ofBox(200);
//    ofSetColor(0);
//    ofNoFill();
//    ofBox(201);
//    cam.end();
    
    ofPushMatrix();
    ofTranslate(ofGetWidth()/2, ofGetHeight()/2);
    
    CMDeviceMotion *d = motionManager.deviceMotion;
    if (d!=nil) {
        CMRotationMatrix m = d.attitude.rotationMatrix;

        ofMatrix4x4 mat(m.m11,m.m21,m.m31,0.0f,m.m12,m.m22,m.m32,0.0f,m.m13,m.m23,m.m33,0.0f,0.0f,0.0f,0.0f,1.0f);
        //cam.setTransformMatrix(mat);

        ofVec2f trans = ofVec3f(mat.preMult(ofVec3f(0,1,0)));
        trans.y*=-1;
        ofTranslate(IMAGE_WIDTH/2*trans);
        
        //ofLine(ofVec3f(0,0,0), vec);
       
    }
    
    ofTranslate(-IMAGE_WIDTH/2, -IMAGE_HEIGHT/2);
    image.draw();
    ofPopMatrix();
    mask.draw(0, 0);
}

//--------------------------------------------------------------
void testApp::exit(){
    image.unload();
    [motionManager stopDeviceMotionUpdates];
    [motionManager release];
    motionManager = nil;
}

//--------------------------------------------------------------
void testApp::touchDown(ofTouchEventArgs & touch){

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

