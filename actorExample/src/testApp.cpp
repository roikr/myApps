#include "testApp.h"
#include "ofxXmlSettings.h"



//--------------------------------------------------------------
void testApp::setup(){
    
    ofSetLogLevel(OF_LOG_VERBOSE);
    
	ofxGamepadHandler::get()->enableHotplug();
	
	//CHECK IF THERE EVEN IS A GAMEPAD CONNECTED
	if(ofxGamepadHandler::get()->getNumPads()>0){
        ofxGamepad* pad = ofxGamepadHandler::get()->getGamepad(0);
        ofAddListener(pad->onAxisChanged, this, &testApp::axisChanged);
        ofAddListener(pad->onButtonPressed, this, &testApp::buttonPressed);
        ofAddListener(pad->onButtonReleased, this, &testApp::buttonReleased);
	}
    
    background.loadImage("background.png");
    
    tom.setup("GTR_ROCK");
    dolly.setup("VOC_CORE");
    blank.setup("DRM_OLDSCHOOL");
    
    ofxXmlSettings xml;
    
    vector<sample> samples;
    xml.loadFile("VOC_BOY_VIDEO.xml");
    
    xml.pushTag("samples");
    for (int i=0;i<xml.getNumTags("sample");i++) {
        xml.pushTag("sample",i);
        
            sample s;
            s.seqname = xml.getAttribute("video", "name", "");
            
            xml.pushTag("video");
                for (int j=0;j<xml.getNumTags("frame");j++) {
                    s.frames.push_back(xml.getAttribute("frame", "use", 0,j));
                }
            xml.popTag();
            
            xml.pushTag("lipsync");
                for (int j=0;j<xml.getNumTags("frame");j++) {
                    s.lips.push_back(xml.getAttribute("frame", "use", 0,j));
                }
            xml.popTag();
        
            if (s.frames.size())
                samples.push_back(s);
            
        
        xml.popTag();
    }
    xml.popTag();
    
    cout << "numSamples: " << samples.size() << endl;
    
    dolly.setupSamples(samples);
    
}



//--------------------------------------------------------------
void testApp::update(){
    
    
    tom.update();
    dolly.update();
    blank.update();
    
    
}

//--------------------------------------------------------------
void testApp::draw(){
    background.draw(0,0);
    tom.draw();
    dolly.draw();
    blank.draw();
    
}



//--------------------------------------------------------------
void testApp::keyPressed(int key){
    
    dolly.trigger(key-'0');
    //tom.trigger(key-'0');
    
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

//--------------------------------------------------------------

void testApp::axisChanged(ofxGamepadAxisEvent& e)
{
	cout << "AXIS " << e.axis << " VALUE " << ofToString(e.value) << endl;
}

void testApp::buttonPressed(ofxGamepadButtonEvent& e)
{
	cout << "BUTTON " << e.button << " PRESSED" << endl;
    dolly.trigger(e.button+1);
}

void testApp::buttonReleased(ofxGamepadButtonEvent& e)
{
	cout << "BUTTON " << e.button << " RELEASED" << endl;
}