#include "testApp.h"
#include "ofxXmlSettings.h"
#include <cstdio>

enum {
    RESET_STATE_IDLE,
    RESET_STATE_DOWN,
    RESET_STATE_ACTIVE
};


ofRectangle readRect(ofxXmlSettings &xml,int i=0);
ofRectangle readRect(ofxXmlSettings &xml,int i) {
    ofRectangle rect;
    rect.x = xml.getAttribute("rect", "x", 0,i);
    rect.y = xml.getAttribute("rect", "y", 0,i);
    rect.width = xml.getAttribute("rect", "width", 0,i);
    rect.height = xml.getAttribute("rect", "height", 0,i);
    char str[256];
    sprintf(str,"<rect x=\"%.0f\" y=\"%.0f\" width=\"%.0f\" height=\"%.0f\"/>",rect.x,rect.y,rect.width,rect.height);
    cout << str << endl;
    return rect;
}

//--------------------------------------------------------------
void testApp::setup(){
    ofDisableArbTex();
    ofxXmlSettings xml;
    ofHideCursor();
    bool bLoaded = xml.loadFile("trivia.xml");
    assert(bLoaded);
    xml.pushTag("trivia");
    
    delay = xml.getValue("delay", 10);
    reset = xml.getValue("reset", 5);
    
    for (int i=0;i<xml.getNumTags("slideshow");i++) {
        xml.pushTag("slideshow",i);
        slideshow ss;
        xml.pushTag("next");
        ss.next = readRect(xml);
        xml.popTag();
        xml.pushTag("previous");
        ss.previous = readRect(xml);
        xml.popTag();
        
        for (int j=0;j<xml.getNumTags("layout");j++) {
            xml.pushTag("layout",j);
            layout l;
            for (int k=0;k<xml.getNumTags("rect");k++) {
                l.rects.push_back(readRect(xml, k));
            }
            ss.layouts.push_back(l);
            xml.popTag();
        }
        
        
        for (int j=0;j<xml.getNumTags("slide");j++) {
            xml.pushTag("slide",j);
            slide s;
            for (int k=0;k<xml.getNumTags("layer");k++) {
                
                ofImage image;
                image.setUseTexture(false);
                image.loadImage("images/"+xml.getValue("layer", "",k));
                ofTexture tex;
                tex.allocate(image.getPixelsRef());
                tex.loadData(image.getPixelsRef());
                s.layers.push_back(tex);
//                s.layers.push_back(image.getPixelsRef());
                if (xml.attributeExists("layer", "correct",k)) {
                    s.correct = k;
                }
            }
            
            ss.slides.push_back(s);
            xml.popTag();
        }
        slideshows.push_back(ss);
        xml.popTag();
    }
    xml.popTag();
    
    
    
    
    
    currentSlideshow = 0;
    current = 0;
    resetState = RESET_STATE_IDLE;
    
    fingerprint.loadImage("fingerprint.png");
    
}

//--------------------------------------------------------------
void testApp::update(){
    slideshow &ss = slideshows[currentSlideshow % slideshows.size()];
    slide &s = ss.slides[current % ss.slides.size()];
    
    if (!layers.empty() && layers.front() == s.correct) {
        if (ofGetElapsedTimef()>delayTimer) {
            current++;
            layers.clear();
        }
    }
 
    if (resetState==RESET_STATE_DOWN && ofGetElapsedTimef()>resetTimer) {
        resetState = RESET_STATE_ACTIVE;
        cout << "RESET_STATE_ACTIVE" << endl;
    }
}

//--------------------------------------------------------------
void testApp::draw(){
    ofBackground(255);
    ofSetColor(255);
   
    ofPushMatrix();
    ofMultMatrix(mat);

//    ofTranslate(pos);
//    ofScale(scale, scale);
    
    slideshow &ss = slideshows[currentSlideshow % slideshows.size()];
    slide &s = ss.slides[current % ss.slides.size()];
//    ofImage image;
//    image.setFromPixels(s.layers.back());
//    image.draw(0, 0);
    s.layers.back().draw(0,0);
    
    ofEnableAlphaBlending();
    for (vector<int>::iterator iter=layers.begin(); iter!=layers.end(); iter++) {
//        ofImage image;
//        image.setFromPixels( s.layers[*iter]);
//        image.draw(0, 0);
        s.layers[*iter].draw(0,0);
    }
    ofDisableAlphaBlending();
    ofPopMatrix();
    
    if (resetState!=RESET_STATE_IDLE) {
        ofEnableAlphaBlending();
        ofSetColor(resetState==RESET_STATE_DOWN ? 255 :0, resetState==RESET_STATE_ACTIVE ? 255 : 0, 0);
        fingerprint.draw(ofGetWidth()-fingerprint.width, 0);
        ofDisableAlphaBlending();
    }
}

//--------------------------------------------------------------
void testApp::exit(){
    cout << "release textures: ";
    for (vector<slideshow>::iterator ssiter=slideshows.begin();ssiter!=slideshows.end();ssiter++) {
        for (vector<slide>::iterator siter=ssiter->slides.begin();siter!=ssiter->slides.end();siter++) {
            for (vector<ofTexture>::iterator titer=siter->layers.begin(); titer!=siter->layers.end(); titer++) {
                cout << titer->getTextureData().textureID << "\t";
                titer->clear();
            }
        }
    }
    cout << endl;
    
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    
    slideshow &ss = slideshows[currentSlideshow % slideshows.size()];
    
   
    switch (key) {
        case OF_KEY_RIGHT:
            current++;
            layers.clear();
            break;
        case OF_KEY_LEFT:
            current = (current+ss.slides.size()-1);
            layers.clear();
            break;
        case OF_KEY_UP:
            currentSlideshow++;
            layers.clear();
            break;
        case OF_KEY_DOWN:
            currentSlideshow=currentSlideshow+slideshows.size()-1;
            layers.clear();
            break;
        case 't':
            ofToggleFullscreen();
            break;
        case 'h':
            ofHideCursor();
            break;
        case 's':
            ofShowCursor();
            break;
        default:
            break;
    }
    
    slide &s = ss.slides[current % ss.slides.size()];
    
    if (layers.empty() || layers.front() != s.correct) {
        if (key-'1' >=0 && key-'1' <=s.layers.size()-2 ) {
            if (key-'1'==s.correct) {
                layers.clear();
                delayTimer = ofGetElapsedTimef()+delay;
            }
            layers.push_back(key-'1');
        }
    }
    
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){

}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y){

}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
    if (resetState==RESET_STATE_ACTIVE) {
        if (y/64 % slideshows.size() != currentSlideshow) {
            currentSlideshow = y/64 % slideshows.size();
        }
        
    }
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
   
    resetState = RESET_STATE_IDLE;
//    downPos.push_back(ofVec3f(x,y,0));
    
    ofVec3f pos = imat.preMult(ofVec3f(x,y,0));
    cout << "mousePressed: " << pos << "(" << ofVec3f(x,y,0) << ")" <<  endl;
    slideshow &ss = slideshows[currentSlideshow % slideshows.size()];
    
    if (ss.next.inside(pos)) {
        current++;
        layers.clear();
    }
    
    if (ss.previous.inside(pos)) {
        current = (current+ss.slides.size()-1);
        layers.clear();
    }
    
    slide &s = ss.slides[current % ss.slides.size()];
    
    if (layers.empty() || layers.front() != s.correct) {   
        for (vector<layout>::iterator liter=ss.layouts.begin();liter!=ss.layouts.end();liter++) {
            if (liter->rects.size() == s.layers.size()-1) { // get num possible answers - ignore background;
                for (vector<ofRectangle>::iterator riter=liter->rects.begin();riter!=liter->rects.end();riter++) {
                    if (riter->inside(pos)) {
                        int num = distance(liter->rects.begin(), riter);
                        if (num==s.correct) {
                            layers.clear();
                             delayTimer = ofGetElapsedTimef()+delay;
                        } 
                        layers.push_back(num);
                        
                        break;
                    }
                }
            }
        }
    }
   
    if (ofRectangle(ofGetWidth()-50,0,50,50).inside(x, y)) {
        resetState = RESET_STATE_DOWN;
        resetTimer = ofGetElapsedTimef()+reset;
        cout << "RESET_STATE_DOWN" << endl;
    }
    
    
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
    
    resetState = RESET_STATE_IDLE;
//    ofVec3f tl = imat.preMult(downPos.front());
//    ofVec3f size = imat.preMult(ofVec3f(x,y,0)) - tl;
//    
//    char str[256];
//    sprintf(str,"<rect x=\"%.0f\" y=\"%.0f\" width=\"%.0f\" height=\"%.0f\"/>",tl.x,tl.y,size.x,size.y);
//    cout << str << endl;
//    downPos.clear();
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){
    ofTexture &tex = slideshows[0].slides[0].layers[0];
//    ofPixels &tex = slideshows[0].slides[0].layers[0];
    float scale = min((float)ofGetWidth()/(float)tex.getWidth(),(float)ofGetHeight()/(float)tex.getHeight());
    
    mat = ofMatrix4x4::newTranslationMatrix((ofGetWidth()-scale*tex.getWidth())/2,(ofGetHeight()-scale*tex.getHeight())/2, 0);
    mat.preMult(ofMatrix4x4::newScaleMatrix(scale, scale, 1.0f));
    imat = mat.getInverse();
    //cout << "windowResized: " << mat << endl;
}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}