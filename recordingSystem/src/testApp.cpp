#include "testApp.h"





//--------------------------------------------------------------
void testApp::setup(){
    
    
	
	ofSetVerticalSync(true);
	ofSetCircleResolution(80);
	ofBackground(54, 54, 54);	
	
	// 0 output channels, 
	// 2 input channels
	// 44100 samples per second
	// 256 samples per buffer
	// 4 num buffers (latency)
	
	recordingSystem.setup();

	soundStream.listDevices();
	
	//if you want to set a different device id 
	soundStream.setDeviceID(4); //bear in mind the device id corresponds to all audio devices, including  input-only and output-only devices.
    
    bufferCounter	= 0;
	drawCounter		= 0;
    
	soundStream.setup(this, 1, 1, SAMPLE_RATE, BUFFER_SIZE, 2);
	

    
}

//--------------------------------------------------------------
void testApp::update(){
	recordingSystem.update();
}

//--------------------------------------------------------------
void testApp::draw(){
	
	recordingSystem.draw();
    
    drawCounter++;
	
	ofSetColor(225);
	string reportString = "buffers received: "+ofToString(bufferCounter)+"\ndraw routines called: "+ofToString(drawCounter)+"\nticks: " + ofToString(soundStream.getTickCount());
	ofDrawBitmapString(reportString, 32, 589);
	
			
}

//--------------------------------------------------------------
void testApp::audioIn(float * input, int bufferSize, int nChannels){	
	
	recordingSystem.audioIn(input, bufferSize, nChannels);
    bufferCounter++;

}

void testApp::audioOut(float * output, int bufferSize, int nChannels) {
    recordingSystem.audioOut(output, bufferSize, nChannels);
}

//--------------------------------------------------------------
void testApp::keyPressed  (int key){ 
	if( key == 's' ){
		soundStream.start();
	}
	
	if( key == 'e' ){
		soundStream.stop();
	}
    
    if (key == 'p') {
        recordingSystem.triggerLast();
    }
    
    if (key>='1' && key<='9') {
        recordingSystem.trigger(key-'1');
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

