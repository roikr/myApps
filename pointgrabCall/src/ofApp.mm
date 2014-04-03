#include "ofApp.h"


float calcTime(string time) {
    vector<string> strs = ofSplitString(time, ";");
//    cout << ofToInt(strs[0])+(float)ofToInt(strs[1])/30.0 << endl;
    return ofToInt(strs[0])+(float)ofToInt(strs[1])/30.0;
}

//--------------------------------------------------------------
void ofApp::setup(){	
    [UIApplication sharedApplication].idleTimerDisabled = NO;
    [UIApplication sharedApplication].idleTimerDisabled = YES;
    
//    ofSetOrientation(OF_ORIENTATION_90_LEFT);
    ofBackground(0);
    
    data.bIsTrackable = false;
    data.bIsGrab = false;
    pointGrab.setup(ofToDataPath(""),true);
    
//    cam.setDeviceID(1);
//    cam.initGrabber(640, 480,OF_PIXELS_BGRA);

    //
    
    cam.initGrabber(640, 480);
    cam.setOrientation(OF_ORIENTATION_90_LEFT,true);
    tex.allocate(640,480,GL_LUMINANCE);
    
    float scale = (float)ofGetWidth()/(5.0*640.0);
    camMat.scale(scale, scale, 1.0);
    camMat.translate(ofVec3f(ofGetWidth()-scale*640.0-20.0,ofGetHeight()-scale*480.0-20,0));
    camiMat = camMat.getInverse();
    bShowCam = true;
    
//    fbo.allocate(640, 480);
    
    pixels.allocate(640,480,OF_IMAGE_COLOR_ALPHA);
    
    
//    scale = 480.0/640.0;
//    fboMat.scale(scale, scale, 1.0);
//    fboMat.translate(ofVec3f(0.5*(640.0-480.0*scale),0,0));
    
    ofPixels pixels;
    ofLoadImage(pixels, "Background.png");
    background.loadData(pixels);
    
    scale = MIN((float)ofGetWidth()/(float)background.getWidth(),(float)ofGetHeight()/(float)background.getHeight());
    mat.scale(scale, scale, 1.0);
    mat.translate(0.5*(ofVec2f(ofGetWidth(),ofGetHeight())-scale*ofVec2f(background.getWidth(),background.getHeight())));
   
    video.loadMovie("call.mp4");
    
    ofXml xml;
    if (xml.load("pointgrab.xml")) {
        xml.setTo("//pointgrab/clips/clip[0]");
        do {
            clip c;
            c.name = xml.getAttribute("name");
            c.start = calcTime(xml.getAttribute("start"));
            c.end = calcTime(xml.getAttribute("end"));
            c.bMute = false;
            
            if (xml.exists("marker")) {
                xml.setTo("marker[0]");
                do {
                    c.markers.push_back(calcTime(xml.getAttribute("time")));
                } while (xml.setToSibling());
                xml.setToParent();
            }
            
            clips.push_back(c);
            
            cout << c.name << '\t' << c.start << '\t' << c.end << endl;
            
            
        } while (xml.setToSibling());
        
        xml.setTo("//pointgrab/icons/icon[0]");
        do {
            cout << xml.getValue() << endl;
            
            string attr = xml.getAttribute("gesture");
            if (!attr.empty()) {
                gesturesIndices[ofToInt(attr)] = icons.size();
                cout << "gesture: " << attr << endl;
            }
            
            attr = xml.getAttribute("palm_type");
            if (!attr.empty()) {
                palmTypeIndices[ofToInt(attr)] = icons.size();
                cout << "palm_type: " << attr << endl;
            }

            
            ofPixels pixels;
            ofLoadImage(pixels, xml.getValue());
            ofTexture tex;
            tex.loadData(pixels);
            icons.push_back(tex);
        } while (xml.setToSibling());
    }

    current = clips.end();
    
    scale = (float)ofGetWidth()/(5.0*(float)icons.front().getWidth());
    iconMat.scale(scale, scale, 1.0);
    iconMat.translate(ofVec3f(20,ofGetHeight()-scale*icons.front().getHeight()-20,0));
    
    
    
    
    
    
    iconTime = ofGetElapsedTimef();
}

/*
static void neon_asm_convert(uint8_t * __restrict dest, uint8_t * __restrict src, int numPixels)
{
	__asm__ volatile("lsr          %2, %2, #3      \n"
					 "# build the three constants: \n"
					 "mov         r4, #28          \n" // Blue channel multiplier
					 "mov         r5, #151         \n" // Green channel multiplier
					 "mov         r6, #77          \n" // Red channel multiplier
					 "vdup.8      d4, r4           \n"
					 "vdup.8      d5, r5           \n"
					 "vdup.8      d6, r6           \n"
					 "0:						   \n"
					 "# load 8 pixels:             \n"
					 "vld4.8      {d0-d3}, [%1]!   \n"
					 "# do the weight average:     \n"
					 "vmull.u8    q7, d0, d4       \n"
					 "vmlal.u8    q7, d1, d5       \n"
					 "vmlal.u8    q7, d2, d6       \n"
					 "# shift and store:           \n"
					 "vshrn.u16   d7, q7, #8       \n" // Divide q3 by 256 and store in the d7
					 "vst1.8      {d7}, [%0]!      \n"
					 "subs        %2, %2, #1       \n" // Decrement iteration count
					 "bne         0b            \n" // Repeat unil iteration count is not zero
					 :
					 : "r"(dest), "r"(src), "r"(numPixels)
					 : "r4", "r5", "r6"
					 );
}
*/
void ofApp::play(string name) {
    cout << "play: " << name << endl;
    vector<clip>::iterator iter = clips.begin();
    while (iter!=clips.end() && iter->name!=name) {
        iter++;
    }
    
    if (iter!=clips.end()) {
        current = iter;
        current->bMute = false;
        video.play();
        video.setPosition(current->start/(float)video.getDuration());
    }
}

//--------------------------------------------------------------
void ofApp::update(){
//    long time=ofGetElapsedTimeMillis();
//    cam.update();
//    cout << ofGetElapsedTimeMillis()-time << endl;
    ofSetColor(255);
    ofFill();
    
    
    
    /*
    if (cam.isFrameNew()) {
        fbo.begin();
        ofClear(0,0,0,255);
        ofPushMatrix();
        ofMultMatrix(fboMat);
        cam.draw(0, 0);
        ofPopMatrix();
        fbo.end();
        
        
        
        fbo.bind();
        
        glReadPixels(0,0,fbo.getWidth(), fbo.getHeight(), GL_RGBA, GL_UNSIGNED_BYTE, pixels.getPixels());
        
        fbo.unbind();
        
        neon_asm_convert(pointGrab.getNextPixels().getPixels(), pixels.getPixels(), pixels.getWidth()*pixels.getHeight());
     */
    ofPixels &pixels = pointGrab.getNextPixels();
//    long time=ofGetElapsedTimeMillis();
    if (cam.copyPixels(pixels)) {
//        cout << ofGetElapsedTimeMillis()-time << endl;
        if (bShowCam) {
            tex.loadData(pixels);
        }
        
        pointGrabData data = pointGrab.update();
        
        if (this->data.bIsTrackable != data.bIsTrackable) {
            cout << "trackable: " << data.bIsTrackable << endl;
            
            if (data.bIsTrackable) {
                cout << "type: " << data.palmType << endl;
                
                bool bShowIcon = true;
                switch (data.palmType) {
                    case PALM_TYPE_OPEN_HAND:
                        play("wife");
                        break;
                    case PALM_TYPE_VSHAPE_FOR_TRACK:
                        play("home");
                        break;
                    default:
                        bShowIcon = false;
                        break;
                }
                if (bShowIcon) {
                    iconIndex = palmTypeIndices[data.palmType];
                    iconTime = ofGetElapsedTimef()+1;
                }
                
                
            }
        }
        
        
            
        for(vector<int>::iterator iter=data.gestures.begin();iter!=data.gestures.end();iter++) {
            // two in a row protection
            if (find(this->data.gestures.begin(), this->data.gestures.end(), *iter)!=this->data.gestures.end()) {
                continue;
            }
            cout << "gesture: " << *iter << endl;
            bool bShowIcon = true;
            switch (*iter) {
                case GESTURE_MUTE:
                   
                    if (current!=clips.end() && !current->markers.empty()) {
                        
                        
                        current->bMute = !current->bMute;
                        float pos = current->bMute ? current->markers[0] : current->markers[1];
                        video.setPosition(pos/(float)video.getDuration());
                        
                    }
                    
                    break;
                case GESTURE_SWIPE_LEFT:
                case GESTURE_SWIPE_RIGHT:
                    if (current!=clips.end()) {
                        if (!current->markers.empty()) {
                           
                            video.setPosition(current->markers[2]/(float)video.getDuration());
                            
                        } else {
                            if (current->name == "call") {
                                play("decline");
                            }
                        }
                    }
                    break;
                case GESTURE_LIKE:
                    if (current!=clips.end() && current->name=="call") {
                        play("incoming");
                    } else {
                        play("call");
                    }
                    break;
                default:
                    bShowIcon = false;
                    break;
            }
            if (bShowIcon) {
                iconIndex = gesturesIndices[*iter];
                iconTime = ofGetElapsedTimef()+1;
            }
        }

        
        

        this->data = data;
    }
    
    

    
    if (video.isLoaded()) {
        video.update();
        if (video.isPlaying() && current!=clips.end()) {
//            cout << video.getPosition() << '\t' << current->end/video.getDuration() << endl;
            if (video.getPosition()>=current->end/(float)video.getDuration()) {
                video.stop();
                current = clips.end();
            }
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
	ofSetColor(255);
    ofPushMatrix();
    ofMultMatrix(mat);
    
    float playhead = video.getPosition()*(float)video.getDuration();
    if (video.isPlaying() && current!=clips.end() && ((playhead>=current->start && playhead<current->end) || current->name=="decline" || current->name=="incoming")) {
        video.getTexture()->draw(0,0);
        
    } else {
        background.draw(0,0);
    }

  
    ofPopMatrix();
    
    if (ofGetElapsedTimef()<iconTime) {
        ofPushMatrix();
        ofMultMatrix(iconMat);
        icons[iconIndex].draw(0,0);
        ofPopMatrix();
    }
    
    if (bShowCam) {
        ofPushMatrix();
        ofMultMatrix(camMat);
//        ofScale(-1, 1);
//        ofTranslate(-cam.getWidth(), 0);
        tex.draw(0,0);
        ofPopMatrix();
    }
}

//--------------------------------------------------------------
void ofApp::exit(){

}

//--------------------------------------------------------------
void ofApp::touchDown(ofTouchEventArgs & touch){
    
    /*
    if (video.isPaused()) {
        current = clips.begin();
    } else {
        current++;
    }
    video.setPaused(true);
    if (current!=clips.end()) {
        video.setPosition(current->start/(float)video.getDuration());
        video.setPaused(false);
    }
     */
    
    if (ofRectangle(0, 0, 480, 640).inside(camiMat.preMult(ofVec3f(touch)))) {
        bShowCam = !bShowCam;
    }
    
}

//--------------------------------------------------------------
void ofApp::touchMoved(ofTouchEventArgs & touch){

}

//--------------------------------------------------------------
void ofApp::touchUp(ofTouchEventArgs & touch){

}

//--------------------------------------------------------------
void ofApp::touchDoubleTap(ofTouchEventArgs & touch){

}

//--------------------------------------------------------------
void ofApp::touchCancelled(ofTouchEventArgs & touch){
    
}

//--------------------------------------------------------------
void ofApp::lostFocus(){

}

//--------------------------------------------------------------
void ofApp::gotFocus(){

}

//--------------------------------------------------------------
void ofApp::gotMemoryWarning(){

}

//--------------------------------------------------------------
void ofApp::deviceOrientationChanged(int newOrientation){

}
