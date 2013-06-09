#include "testApp.h"

//--------------------------------------------------------------
void testApp::setup(){
	
    
    int start = ofGetElapsedTimeMillis();
    
    for (int i=0;i<100;i++) {
        stringstream str;
        str << setfill('0') << setw(5) << i+1;
        ofImage image;
        image.setUseTexture(false);
        image.loadImage("miami_bmp/"+str.str()+".bmp");
        images.push_back(image);
    }
    cout << ofGetElapsedTimeMillis() - start << endl;
    
    current = 0;
    tex.allocate(images[current].getPixelsRef());
    tex.loadData(images[current].getPixelsRef());
    
    
    
}

//--------------------------------------------------------------
void testApp::update(){
	ofBackground(255);
    int frame = floor(ofGetElapsedTimef()*25.0)+1;
    if (frame!=current && frame<images.size()) {
        tex.loadData(images[frame].getPixelsRef());
        current = frame;
    }
}

//--------------------------------------------------------------
void testApp::draw(){	
	tex.draw(0,0);
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){

}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y){

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
