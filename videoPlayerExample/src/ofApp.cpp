#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){
	ofBackground(0);
	ofSetVerticalSync(true);
    
    gui.setup("panel");
    gui.add(volume.set("volume",0.5,0,1));
    gui.loadFromFile("settings.xml");
	

    volume.addListener(this, &ofApp::volumeChanged);
	// Uncomment this to show movies with alpha channels
	// fingerMovie.setPixelFormat(OF_PIXELS_RGBA);

    ofDirectory dir;
    dir.listDir(ofToDataPath("movies"));
	video.loadMovie(dir.getPath(0));
    video.setLoopState(OF_LOOP_NORMAL);
	video.play();
    
    
    float scale = MIN((float)ofGetWidth()/video.getWidth(),(float)ofGetHeight()/video.getHeight());
    mat.makeIdentityMatrix();
    mat.scale(scale, scale, 1.0);
    mat.translate(0.5*(ofVec2f(ofGetWidth(),ofGetHeight())-scale*ofVec2f(video.getWidth(),video.getHeight())));
    
    bool bShowGui = false;
    ofHideCursor();
    timer = ofGetElapsedTimef();
    
}

//--------------------------------------------------------------
void ofApp::update(){
    video.update();
    if (bShowGui && ofGetElapsedTimef()>timer) {
        bShowGui = false;
        ofHideCursor();
    }
}

//--------------------------------------------------------------
void ofApp::draw(){

	ofSetHexColor(0xFFFFFF);
    ofPushMatrix();
    ofMultMatrix(mat);
    video.draw(0,0);
    ofPopMatrix();
    
    if (bShowGui) {
        gui.draw();
    }
}

void ofApp::exit() {
    video.stop();
    video.close();
}

void ofApp::volumeChanged(float &volume) {
    video.setVolume(volume);
    gui.saveToFile("settings.xml");
}

//--------------------------------------------------------------
void ofApp::keyPressed  (int key){
//    switch(key){
//        case 'f':
//            frameByframe=!frameByframe;
//            fingerMovie.setPaused(frameByframe);
//        break;
//        case OF_KEY_LEFT:
//            fingerMovie.previousFrame();
//        break;
//        case OF_KEY_RIGHT:
//            fingerMovie.nextFrame();
//        break;
//        case '0':
//            fingerMovie.firstFrame();
//        break;
//    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
	bShowGui = true;
    ofShowCursor();
    timer = ofGetElapsedTimef()+10;
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
	
}


//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
	
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
