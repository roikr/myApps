#include "testApp.h"
#include "ofxXmlSettings.h"

//--------------------------------------------------------------
void testApp::setup(){
    ofxXmlSettings xml;
    ofHideCursor();
    bool bLoaded = xml.loadFile("slideshow.xml");
    xml.pushTag("slideshow");
    reset = xml.getValue("reset", 600);
    delay = xml.getValue("delay", 3);
    width = xml.getValue("width", 1920);
    height = xml.getValue("height", 1020);
    
    tex.allocate(width, height, GL_RGB);
    
    xml.pushTag("english");
    for (int i=0;i<xml.getNumTags("slide");i++) {
//        ofImage image;
//        image.setUseTexture(false);
//        image.loadImage("images/"+xml.getValue("slide", "",i));
//        ofTexture tex;
//        tex.allocate(image.getPixelsRef());
//        tex.loadData(image.getPixelsRef());
//        english.push_back(tex);
//        english.push_back("images/"+xml.getValue("slide", "",i));
        ofImage image;
        image.setUseTexture(false);
        string filename = xml.getValue("slide", "",i);
        cout << "loading " << filename << endl;
        image.loadImage("images/"+filename);
        english.push_back(image.getPixelsRef());
    }
    xml.popTag();
    xml.pushTag("hebrew");
    for (int i=0;i<xml.getNumTags("slide");i++) {
//        ofImage image;
//        image.setUseTexture(false);
//        image.loadImage("images/"+xml.getValue("slide", "",i));
//        ofTexture tex;
//        tex.allocate(image.getPixelsRef());
//        tex.loadData(image.getPixelsRef());
//        hebrew.push_back(tex);
//        hebrew.push_back("images/"+xml.getValue("slide", "",i));
        
        ofImage image;
        image.setUseTexture(false);
        string filename = xml.getValue("slide", "",i);
        cout << "loading " << filename << endl;
        image.loadImage("images/"+filename);
        hebrew.push_back(image.getPixelsRef());
    }
    xml.popTag();
    xml.popTag();
    
    
    bLangEng = true;
    int start = 0;
    int current = 0;
    bManual = false;
    delayTimer = ofGetElapsedTimef();
//    loadImage();
    loadTexture();
    
}

//--------------------------------------------------------------
void testApp::update(){
    if (bManual) {
        if ((ofGetElapsedTimef()-resetTimer)>reset) {
            bManual = false;
            delayTimer = ofGetElapsedTimef();
        }
    } else {
        if (ofGetElapsedTimef()-delayTimer>delay) {
            current++;
//            loadImage();
            loadTexture();
            delayTimer=ofGetElapsedTimef();
        }
    }
}

//--------------------------------------------------------------
void testApp::draw(){
    ofBackground(255);
    ofColor(255);
    ofPushMatrix();
    ofMultMatrix(mat);
    
    tex.draw(0, 0);

//    if (bLangEng) {
//        english[current % english.size()].draw(0, 0);
//    } else {
//        hebrew[current % hebrew.size()].draw(0, 0);
//    }
    ofPopMatrix();
}

//--------------------------------------------------------------
//void testApp::exit(){
//    cout << "release textures: ";
//    for (vector<ofTexture>::iterator titer=english.begin(); titer!=english.end(); titer++) {
//        cout << titer->getTextureData().textureID << "\t";
//        titer->clear();
//    }
//    for (vector<ofTexture>::iterator titer=hebrew.begin(); titer!=hebrew.end(); titer++) {
//        cout << titer->getTextureData().textureID << "\t";
//        titer->clear();
//    }
//    cout << endl;
//    
//}

//void testApp::loadImage() {
//    if (bLangEng) {
//        image.loadImage(english[current % english.size()]);
//    } else {
//       image.loadImage(hebrew[current % hebrew.size()]);
//    }
//}



void testApp::loadTexture() {
    if (bLangEng) {
        tex.loadData(english[current % english.size()]);
    } else {
        tex.loadData(hebrew[current % english.size()]);
    }
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    
    if (key=='t') {
        bLangEng=!bLangEng;
        current=0;
//        loadImage();
        loadTexture();
    } else if (bManual) {
        current++;
//        loadImage();
        loadTexture();
        
        if ((bLangEng && current-start>=english.size()) || (!bLangEng && current-start>=hebrew.size())) {
            bManual = false;
            delayTimer=ofGetElapsedTimef();
        }
        
    } else {
        bManual = true;
        start = current;
    }
    
    
    
    resetTimer = ofGetElapsedTimef();
        
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
    float scale = min((float)ofGetWidth()/(float)width,(float)ofGetHeight()/height);
    
    mat = ofMatrix4x4::newTranslationMatrix((ofGetWidth()-scale*width)/2,(ofGetHeight()-scale*height)/2, 0);
    mat.preMult(ofMatrix4x4::newScaleMatrix(scale, scale, 1.0f));
}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}