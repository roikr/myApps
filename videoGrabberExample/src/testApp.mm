#include "testApp.h"



//--------------------------------------------------------------
void testApp::setup(){	
	iPhoneSetOrientation(OFXIPHONE_ORIENTATION_PORTRAIT);
    
	ofSetFrameRate(30);

    grabber.setDeviceID(1);
	grabber.initGrabber(640, 480, OF_PIXELS_BGRA);
	tex.allocate(grabber.getWidth(), grabber.getHeight(), GL_RGB);
	
	pix = new unsigned char[ (int)( grabber.getWidth() * grabber.getHeight() * 3.0) ];
}

//--------------------------------------------------------------
void testApp::update(){
	ofBackground(255,255,255);	
	
	grabber.update();
	
	unsigned char * src = grabber.getPixels();
	int totalPix = grabber.getWidth() * grabber.getHeight() * 3;
	
	for(int k = 0; k < totalPix; k+= 3){
		pix[k  ] = 255 - src[k];
		pix[k+1] = 255 - src[k+1];
		pix[k+2] = 255 - src[k+2];		
	}
	
	tex.loadData(pix, grabber.getWidth(), grabber.getHeight(), GL_RGB);
}

//--------------------------------------------------------------
void testApp::draw(){	
	
	ofSetColor(255);
    ofPushMatrix();
    ofRotate(-90);
    ofTranslate(-grabber.getWidth(),0);
	grabber.draw(0, 0);
    
    
	
	tex.draw(0, 0, tex.getWidth() / 4, tex.getHeight() / 4);
    ofPopMatrix();
    ofSetColor(255, 0, 0);
    ofCircle(50, 100, 25);
}

//--------------------------------------------------------------
void testApp::exit(){
    
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


