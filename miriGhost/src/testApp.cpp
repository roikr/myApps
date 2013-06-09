#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){

	
    vidGrabber.setVerbose(true);
    vidGrabber.initGrabber(320,240);
	
    vidPlayer.setPixelFormat(OF_PIXELS_RGBA);
    vidPlayer.loadMovie("Test alpha straight.mov");
    vidPlayer.setLoopState(OF_LOOP_NORMAL);
    vidPlayer.play();
	

    colorImg.allocate(320,240);
	grayImage.allocate(320,240);
	grayBg.allocate(320,240);
	grayDiff.allocate(320,240);

	bLearnBakground = true;
	threshold = 20;
    
    ofSetCircleResolution(36);
    
    vidRect = ofRectangle(700, 20, 320, 240);
}

//--------------------------------------------------------------
void testApp::update(){
	ofBackground(100,100,100);

    bool bNewFrame = false;

	
       vidGrabber.update();
	   bNewFrame = vidGrabber.isFrameNew();
    
        vidPlayer.update();
//        bNewFrame = vidPlayer.isFrameNew();
	

	if (bNewFrame){

		
        colorImg.setFromPixels(vidGrabber.getPixels(), 320,240);
	    
        //colorImg.setFromPixels(vidPlayer.getPixels(), 320,240);
        

        grayImage = colorImg;
		if (bLearnBakground == true){
			grayBg = grayImage;		// the = sign copys the pixels from grayImage into grayBg (operator overloading)
			bLearnBakground = false;
		}

		// take the abs value of the difference between background and incoming and then threshold:
		grayDiff.absDiff(grayBg, grayImage);
		grayDiff.threshold(threshold);

		// find contours which are between the size of 20 pixels and 1/3 the w*h pixels.
		// also, find holes is set to true so we will get interior contours as well....
		contourFinder.findContours(grayDiff, 200, (340*240)/3, 10, false);	// find holes
	}

}

//--------------------------------------------------------------
void testApp::draw(){

	// draw the incoming, the grayscale, the bg and the thresholded difference
	ofSetHexColor(0xffffff);
	colorImg.draw(20,20);
	grayImage.draw(360,20);
	grayBg.draw(20,280);
	grayDiff.draw(360,280);
    
    vidPlayer.draw(vidRect);
    
   
    
    ofSetColor(255);
    ofNoFill();
    ofRect(rect);
    
	// then draw the contours:

	ofFill();
	ofSetHexColor(0x333333);
	ofRect(360,540,320,240);
	ofSetHexColor(0xffffff);

	// we could draw the whole contour finder
	//contourFinder.draw(360,540);

	// or, instead we can draw each blob individually from the blobs vector,
	// this is how to get access to them:
    for (int i = 0; i < contourFinder.nBlobs; i++){
        contourFinder.blobs[i].draw(360,540);
		
		// draw over the centroid if the blob is a hole
//		ofSetColor(255);
        
//		if(contourFinder.blobs[i].hole){
//			ofDrawBitmapString("hole",
//				contourFinder.blobs[i].boundingRect.getCenter().x + 360,
//				contourFinder.blobs[i].boundingRect.getCenter().y + 540);
//		}
        
    }
    
    ofSetColor(255);
    ofNoFill();
    ofPushMatrix();
    ofTranslate(360, 540);
    
    ofVec2f center = rect.getCenter() - vidRect.getTopLeft();
    float radius = min(rect.width,rect.height)/2;
    
    float minDist = 1000;
    
    
    
    ofCircle(center,radius);
    for (int i = 0; i < contourFinder.nBlobs; i++){
        ofRectangle bb = contourFinder.blobs[i].boundingRect;
        float blobRad = max(bb.width,bb.height)/2;
        ofVec2f blobCenter = contourFinder.blobs[i].centroid;
    
        ofCircle(blobCenter, blobRad);
        
        float dist = (blobCenter-center).length() - (blobRad+radius);
        
        if (dist < minDist) {
            minDist = dist;
        }
    }
    ofPopMatrix();
    
    
    grayImage.draw(700,280, 320,240);
    ofEnableAlphaBlending();
    ofSetColor(255, 255, 255, ofMap(minDist, -50, 50, 100, 255,true));
    vidPlayer.draw(700,280, 320,240);
    ofDisableAlphaBlending();
    

	// finally, a report:
	ofSetHexColor(0xffffff);
	stringstream reportStr;
	reportStr << "bg subtraction and blob detection" << endl
			  << "press ' ' to capture bg" << endl
			  << "threshold " << threshold << " (press: +/-)" << endl
			  << "num blobs found " << contourFinder.nBlobs << ", fps: " << ofGetFrameRate();
	ofDrawBitmapString(reportStr.str(), 20, 600);

}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

	switch (key){
        case 's':
        case 'S':
            vidGrabber.videoSettings();
            break;
		case ' ':
			bLearnBakground = true;
			break;
		case '+':
			threshold ++;
			if (threshold > 255) threshold = 255;
			break;
		case '-':
			threshold --;
			if (threshold < 0) threshold = 0;
			break;
	}
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
    if (vidRect.inside(x, y)) {
        rect.width = x-rect.x;
        rect.height =y-rect.y;
    }
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
    if (vidRect.inside(x, y)) {
        rect.x = x;
        rect.y = y;
        rect.width = 0;
        rect.height = 0;
    }
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){

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
