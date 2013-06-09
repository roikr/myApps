#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	ofBackground(255,255,255);
	ofSetVerticalSync(true);
	

	// Uncomment this to show movies with alpha channels
	// fingerMovie.setPixelFormat(OF_PIXELS_RGBA);

	fingerMovie.loadMovie("movies/fingers.mov");
    cout << fingerMovie.getTotalNumFrames() << endl;
    for (int i=0;i<fingerMovie.getTotalNumFrames();i++) {
        ofFbo fbo;
        frames.push_back(fbo);
        frames.back().allocate(fingerMovie.getWidth(),fingerMovie.getHeight());
		frames.back().begin();
		ofClear(0, 0, 0, 0);
		frames.back().end();
        
    }
    
    fingerMovie.play();
    fingerMovie.setPaused(true);
    fingerMovie.firstFrame();
    currentFrame = 1;
}

//--------------------------------------------------------------
void testApp::update(){
    fingerMovie.update();
    
    if ( currentFrame && fingerMovie.isFrameNew() ) {
        cout << currentFrame << endl;
        ofFbo &frame = frames[currentFrame-1];
       
        frame.begin();
        fingerMovie.draw(0, 0);
        frame.end();
        
        currentFrame++;
        
        if (currentFrame > fingerMovie.getTotalNumFrames()) {
            currentFrame = 0;
            fingerMovie.stop();
        } else {
            
            fingerMovie.nextFrame();
        }
        
        
    }
}

//--------------------------------------------------------------
void testApp::draw(){

	ofSetHexColor(0xFFFFFF);
    int frameNum = floor((float)(frames.size())*(float)ofGetMouseX()/(float)ofGetWidth());
    if (frameNum>=0 && frameNum<frames.size()) {
        frames[frameNum].draw(0, 0);
    }

}

//--------------------------------------------------------------
void testApp::keyPressed  (int key){
    
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y ){
	
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
	
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
