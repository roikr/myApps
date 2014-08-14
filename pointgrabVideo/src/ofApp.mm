#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){	
    [UIApplication sharedApplication].idleTimerDisabled = NO;
    [UIApplication sharedApplication].idleTimerDisabled = YES;
    
     ofSetOrientation(OF_ORIENTATION_90_LEFT);
    ofBackground(0);
    
    data.bIsTrackable = false;
    data.bIsGrab = false;
    pointGrab.setup(ofToDataPath(""),true);
    
    cam.initGrabber(640, 480);
    cam.setOrientation(OF_ORIENTATION_90_LEFT,true);
    cam.setRotation(0);
    tex.allocate(640, 480, GL_LUMINANCE);
   
    float scale = (float)ofGetHeight()/(5.0*480.0);
    camMat.scale(scale, scale, 1.0);
    camMat.translate(ofVec3f(ofGetWidth()-scale*640.0-20.0,ofGetHeight()-scale*480.0-20,0));
    camiMat = camMat.getInverse();
    bShowCam = true;

    ofPixels pixels;
    ofLoadImage(pixels, "Mute.png");
    mute.loadData(pixels);
    
    ofDirectory dir;
    dir.listDir(ofToDataPath("videos"));
    for (int i=0;i<dir.size();i++ ) {
        videos.push_back(dir.getPath(i) );
    }
    currentVideo = 0;
    
    if (!videos.empty()) {
        video.loadMovie(videos.front());
        video.play();
    }
    
    bPaused = false;
    volume = 1.0;
    video.setVolume(volume);
    bMute = false;
    step = 0;
    bScrub = false;
    
    scale = (float)ofGetWidth()/1920;
    mat.scale(scale,scale,1.0);
    mat.translate(0.5*(ofVec2f(ofGetWidth(),ofGetHeight())-scale*ofVec2f(1920,1080)));
    
    
    ofXml xml;
    if (xml.load("pointgrab.xml")) {
        xml.setTo("//pointgrab/icon[0]");
        do {
            cout << xml.getValue() << endl;
            
            string attr = xml.getAttribute("gesture");
            if (!attr.empty()) {
                gesturesIndices[ofToInt(attr)] = icons.size();
                cout << "gesture: " << attr << endl;
            }
            
            ofPixels pixels;
            ofLoadImage(pixels, xml.getValue());
            ofTexture tex;
            tex.loadData(pixels);
            icons.push_back(tex);
        } while (xml.setToSibling());
    }

    
    scale = (float)ofGetHeight()/(5.0*(float)icons.front().getHeight());
    iconMat.scale(scale, scale, 1.0);
    iconMat.translate(ofVec3f(20,ofGetHeight()-scale*icons.front().getHeight()-20,0));
    iconTime = ofGetElapsedTimef();
    
//    float scale = MIN((float)w/(float)STAGE_WIDTH,(float)h/(float)STAGE_HEIGHT);
    
    
    
    
    
    
    
}


void ofApp::showIcon(int index) {
    iconIndex = index;
    iconTime = ofGetElapsedTimef()+1;
}



//--------------------------------------------------------------
void ofApp::update(){
    
    if (video.isLoaded()) {
        video.update();
        
        if (video.getIsMovieDone()) {
            cout << "play again" << endl;
            video.play();
        }
        
    }
    
    ofPixels &pixels = pointGrab.getNextPixels();
    if (cam.copyPixels(pixels)) {
        if (bShowCam) {
            tex.loadData(pixels);
        }
        pointGrabData data = pointGrab.update();
        

        
        if (data.bIsTrackable) {
        
            switch (data.palmType) {
                case PALM_TYPE_FIST_FOR_TRACK:
                    volume+=data.delta.x/1000;
                    volume = ofClamp(volume, 0.0, 1.0);
                    cout << "delta: " << data.delta.x << endl;
                    
                    video.setVolume(volume);
                    if (data.delta.x >= 1) {
                        bMute = false;
                        showIcon(6);
                    } else if (data.delta.x<=-1) {
                        bMute = false;
                        showIcon(5);
                    } else {
                        showIcon(4);
                    }
                    break;
                    
                case PALM_TYPE_OPEN_HAND:
//                    cout << data.scroll << endl;
                    
                    if (!this->data.bIsTrackable) {
                        showIcon(0);
                    }
                    
                    if (data.scroll==-1 || data.scroll==1) {
                        startTime = ofGetElapsedTimef();
                        speed = 5*data.scroll;
                        scrollCount-=data.scroll;
                        
                        if (video.isLoaded() && bPaused) {
                            video.setPosition(startPos+5*(float)scrollCount/(float)video.getDuration());
                            //                            if (bPaused && ofGetElapsedTimef()-startTime<2.0) {
//                                float delta = speed*(ofGetElapsedTimef()-startTime)/video.getDuration();
//                                video.setPosition(ofClamp(startPos+delta, 0.0, 1.0));
//                            }
                        }
                        
                        showIcon(data.scroll==1 ? 2 : 3);
                        
                    }
                    
                    
                    if (!this->data.bIsGrab && data.bIsGrab) {
                        
                        bPaused=!bPaused;
                        video.setPaused(bPaused);
                        startPos = video.getPosition();
                        speed = 0;
                        scrollCount = 0;
                        showIcon(1);
                        
                        cout << "grab: " << data.bIsGrab << endl;
                    }
                    break;
                
                    
                default:
                    break;
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
                    bMute=!bMute;
                    if (bMute) {
                        video.setVolume(0);
                    } else {
                        video.setVolume(volume);
                    }
                    break;
                case GESTURE_LIKE:
                    break;
                case GESTURE_SWIPE_LEFT:
//                    if (video.getDuration()*video.getPosition()>5) {
//                        video.setPaused(true);
//                        video.setPosition(0);
//                        video.setPaused(false);
//                    } else {
                    video.close();
                    currentVideo=(currentVideo-1+videos.size()) % videos.size();
                    video.loadMovie(videos[currentVideo]);
                    video.play();
                    
                    break;
                case GESTURE_SWIPE_RIGHT:
                    video.close();
                    currentVideo=(currentVideo+1) % videos.size();
                    video.loadMovie(videos[currentVideo]);
                    video.play();
                    break;
                
                default:
                    bShowIcon = false;
                    break;
            }
            if (bShowIcon) {
                showIcon(gesturesIndices[*iter]);
            }
        }
            
        
        this->data = data;
        
        
        
        
    }
    
    
    
    
    
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofSetColor(255);
    
    if (video.isPlaying() || video.isPaused()) {
        ofPushMatrix();
        ofMultMatrix(mat);
        video.getTexture()->draw(0,0);
        if (bMute) {
            mute.draw(20, 20);
        }
        ofPopMatrix();
    }
    
    if (ofGetElapsedTimef()<iconTime) {
        ofPushMatrix();
        ofMultMatrix(iconMat);
        icons[iconIndex].draw(0,0);
        ofPopMatrix();
    }
    
    if (bShowCam) {
        ofPushMatrix();
        ofMultMatrix(camMat);
        tex.draw(0,0);
        ofPopMatrix();
    }
}

//--------------------------------------------------------------
void ofApp::exit(){
    pointGrab.exit();
}

//--------------------------------------------------------------
void ofApp::touchDown(ofTouchEventArgs & touch){
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
