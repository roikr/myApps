#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
  
    data.bIsTrackable = false;
    data.bIsGrab = false;
	// initialize the accelerometer
	ofxAccelerometer.setup();
	
    
//    ofxPointGrab.setPreviewRect(ofRectangle(10,10,320,240));
    //ofxRegisterPointGrabEvents(this);
//	//If you want a landscape oreintation 
//	iPhoneSetOrientation(OFXIPHONE_ORIENTATION_LANDSCAPE_RIGHT);
    
    
    
    [UIApplication sharedApplication].idleTimerDisabled = NO;
    [UIApplication sharedApplication].idleTimerDisabled = YES;
	
	ofBackground(127,127,127);
    ofSetColor(255);
   
    
    cout << ofGetWidth() << "\t" << ofGetHeight() << endl;
    scaleStart = 1.0f;
    data.bIsTrackable = false;
    data.bIsGrab = false;
    pointGrab.setup();
    
    
    grabber.initGrabber(480,360);
    
    tex.allocate(640, 480, GL_LUMINANCE);
//    tex.setAnchorPercent(0.5, 0.5);

    
}

//--------------------------------------------------------------
void testApp::update(){
    
    /*
    
    CMSampleBufferRef sbuf = (CMSampleBufferRef)CMBufferQueueDequeueAndRetain([grabber previewBufferQueue]);
    
    if (sbuf) {
        
        CVImageBufferRef pixelBuffer = CMSampleBufferGetImageBuffer(sbuf);
        CVPixelBufferLockBaseAddress( pixelBuffer, 0 );
        
        vImage_Buffer src;
        src.height = CVPixelBufferGetHeight(pixelBuffer);
        src.width = CVPixelBufferGetWidth(pixelBuffer);
        src.rowBytes = CVPixelBufferGetBytesPerRow(pixelBuffer);
        src.data = CVPixelBufferGetBaseAddress(pixelBuffer);
        
        vImage_Buffer dest;
        dest.height = pixels.getHeight();
        dest.width = pixels.getWidth();
        dest.rowBytes = pixels.getBytesPerPixel()*pixels.getWidth();
        dest.data = pixels.getPixels();
        
        // Swap pixel channels from BGRA to RGBA.
        const uint8_t map[4] = { 2, 1, 0, 3 };
        vImagePermuteChannels_ARGB8888(&src, &dest, map, kvImageNoFlags);
        
        CVPixelBufferUnlockBaseAddress( pixelBuffer, 0 );
        CFRelease(sbuf);
        return true;
        
	} else {
        return false;
    }

    */
    ofPixels &pixels = pointGrab.getNextPixels();
    if (grabber.copyPixels(pixels)) {
        tex.loadData(pixels);
        pointGrabData data = pointGrab.update();
        
        if (!this->data.bIsGrab && data.bIsGrab) {
            scaleStart = data.scale;
        }
        this->data = data;
        
        if (this->data.bIsTrackable) {
            pos+=0.5*this->data.delta;
        } else {
            pos = ofPoint(ofGetWidth()/2,ofGetHeight()/2);
        }
    }
}

//--------------------------------------------------------------
void testApp::draw(){
    ofPushMatrix();
    float scale = ofGetWidth()/tex.getWidth()/4;
    ofScale(scale, scale);
    ofScale(-1, 1);
    ofTranslate(-tex.getWidth(),0);
    tex.draw(0, 0);
    ofPopMatrix();
    ofPushStyle();
    ofSetColor(0, 255, 0);
    
    if (data.bIsGrab) {
        ofFill();
        ofCircle(pos, 50+50*log(scaleStart/data.scale));
    } else {
        ofNoFill();
        ofCircle(pos,50);
    }
    ofPopStyle();
	
    ofPushMatrix();
    ofTranslate(ofGetWidth()-500, 30);
    ofScale(2.0,2.0);
    ofDrawBitmapString("fps:", 0,0);
    ofDrawBitmapString("video fps:", 0,20);
    ofDrawBitmapString("RefPoint:",0,40);
    ofDrawBitmapString("RectSize:", 0,60);
    ofDrawBitmapString("Scale:", 0,80);
    ofDrawBitmapString("Delta:", 0,100);
    ofDrawBitmapString("IsTrackable:", 0,120);
    ofDrawBitmapString("IsGrab:", 0,140);
    ofPopMatrix();
    
    ofPushMatrix();
    ofTranslate(ofGetWidth()-250, 30);
    ofScale(2.0,2.0);
    ofDrawBitmapString(ofToString(ofGetFrameRate(),2), 0,0);
    ofDrawBitmapString(ofToString(grabber.getFPS(),2), 0,20);
    ofDrawBitmapString(ofToString(data.camRefPoint.x, 2)+" \t"+ofToString(data.camRefPoint.y, 2), 0,40);
    ofDrawBitmapString(ofToString(data.camFramePalmRectSize.x, 2)+" \t"+ofToString(data.camFramePalmRectSize.y, 2), 0,60);
    ofDrawBitmapString(ofToString(data.scale, 2), 0,80);
    ofDrawBitmapString(ofToString(data.delta.x, 2)+" \t"+ofToString(data.delta.y, 2), 0,100);
    ofDrawBitmapString(ofToString(data.bIsTrackable), 0,120);
    ofDrawBitmapString(ofToString(data.bIsGrab), 0,140);
    
    ofPopMatrix();
}

//--------------------------------------------------------------
void testApp::exit(){
    pointGrab.exit();
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

