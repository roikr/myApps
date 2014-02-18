#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup(){	
    [UIApplication sharedApplication].idleTimerDisabled = NO;
    [UIApplication sharedApplication].idleTimerDisabled = YES;
    
     ofSetOrientation(OF_ORIENTATION_90_LEFT);
    ofBackground(0);
    
    data.bIsTrackable = false;
    data.bIsGrab = false;
    scaleStart = 1.0f;
    pointGrab.setup(ofToDataPath(""),true);
    
    cam.initGrabber(640, 480);
    cam.setOrientation(OF_ORIENTATION_90_LEFT,true);
    tex.allocate(640, 480, GL_LUMINANCE);
    
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
    step = 0;
    bScrub = false;
    
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
            ofLoadImage(pixels, "images/"+xml.getValue());
            ofTexture tex;
            tex.loadData(pixels);
            icons.push_back(tex);
        } while (xml.setToSibling());
    }

    iconScale = (float)ofGetHeight()/(5.0*(float)icons.front().getHeight());
    
//    float scale = MIN((float)w/(float)STAGE_WIDTH,(float)h/(float)STAGE_HEIGHT);
    
    
    
    float scale = (float)ofGetWidth()/1920;
    mat = ofMatrix4x4::newTranslationMatrix(0.5*(ofVec2f(ofGetWidth(),ofGetHeight())-scale*ofVec2f(1920,1080)));
    mat.preMult(ofMatrix4x4::newScaleMatrix(scale, scale, 1.0));

    ofBackground(0);
}


void ofApp::showIcon(int index) {
    iconIndex = index;
    iconTime = ofGetElapsedTimef()+1;
}

//--------------------------------------------------------------
void ofApp::update(){
    ofPixels &pixels = pointGrab.getNextPixels();
    if (cam.copyPixels(pixels)) {
        tex.loadData(pixels);
        pointGrabData data = pointGrab.update();
        
        if (!this->data.bIsGrab && data.bIsGrab) {
            scaleStart = data.scale;
            
        }
        
        if (this->data.bIsGrab != data.bIsGrab) {
            if (data.palmType==PALM_TYPE_OPEN_HAND) {
                if (data.bIsGrab) {
                    bPaused=!bPaused;
                    video.setPaused(bPaused);
                    speed = 0;
                    showIcon(1);
                }
            }
            cout << "grab: " << data.bIsGrab << endl;
        }
        
        if (this->data.bIsTrackable != data.bIsTrackable) {
            cout << "trackable: " << data.bIsTrackable << endl;
            
            if (data.bIsTrackable) {
                cout << "type: " << data.palmType << endl;
                posStart = data.camRefPoint;
                
                switch (data.palmType) {
                    case PALM_TYPE_FIST_FOR_TRACK:
                        showIcon(2);
                        break;
                    case PALM_TYPE_OPEN_HAND:
                        showIcon(0);
                        break;
                        
                    default:
                        break;
                }
            }
        }
        
        
        
        
        for(vector<int>::iterator iter=data.gestures.begin();iter!=data.gestures.end();iter++) {
            // two in a row protection
            if (find(this->data.gestures.begin(), this->data.gestures.end(), *iter)!=this->data.gestures.end()) {
                continue;
            }
            cout << "gesture: " << *iter << endl;
            switch (*iter) {
                case GESTURE_MUTE:
                    volume = 0;
                    video.setVolume(0);
                    break;
                case GESTURE_SWIPE_LEFT:
                    if (video.getDuration()*video.getPosition()>5) {
                        video.setPaused(true);
                        video.setPosition(0);
                        video.setPaused(false);
                    } else {
                        video.close();
                        currentVideo=(currentVideo-1+videos.size()) % videos.size();
                        video.loadMovie(videos[currentVideo]);
                        video.play();
                    }
                    break;
                case GESTURE_SWIPE_RIGHT:
                    video.close();
                    currentVideo=(currentVideo+1) % videos.size();
                    video.loadMovie(videos[currentVideo]);
                    video.play();
                    break;
                
                case GESTURE_CLOCKWISE_CIRCLE:
                case GESTURE_ANTICLOCKWISE_CIRCLE:
                    speed = *iter==GESTURE_CLOCKWISE_CIRCLE ? 5 : -5 ;
                    startTime = ofGetElapsedTimef();
                    startPos = video.getPosition();
                    bPaused = true;
                    video.setPaused(true);
                    break;
                    
                default:
                    break;
            }
            showIcon(gesturesIndices[*iter]);
        }
            
        
        this->data = data;
        
        
        if (this->data.bIsTrackable) {
            switch (data.palmType) {
                case PALM_TYPE_FIST_FOR_TRACK:
                    volume+=this->data.delta.x/1000;
                    volume = ofClamp(volume, 0.0, 1.0);
                    cout << "volume: " << volume << endl;
                    video.setVolume(volume);
                    break;
                    
                default:
                    break;
            }
        }
        
    }
    
    
    
    if (video.isLoaded()) {
        video.update();
        
        if (bPaused && speed!=0) {
            float delta = speed*(ofGetElapsedTimef()-startTime)/video.getDuration();
            video.setPosition(ofClamp(startPos+delta, 0.0, 1.0));
        }
        
        if (video.getIsMovieDone()) {
            cout << "play again" << endl;
            video.play();
        }
        
        
        
    }
    
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofSetColor(255);
	ofPushMatrix();
    ofMultMatrix(mat);
	video.getTexture()->draw(0,0);
    ofPopMatrix();
    
    if (ofGetElapsedTimef()<iconTime) {
        ofPushMatrix();
        ofScale(iconScale, iconScale);
        icons[iconIndex].draw(20,20);
        ofPopMatrix();
    }
}

//--------------------------------------------------------------
void ofApp::exit(){
    pointGrab.exit();
}

//--------------------------------------------------------------
void ofApp::touchDown(ofTouchEventArgs & touch){

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
