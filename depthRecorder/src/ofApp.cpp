#include "ofApp.h"

#define CAM_WIDTH 640
#define CAM_HEIGHT 480


//--------------------------------------------------------------
void ofApp::setup(){
//    const char* version = (const char*)glGetString(GL_VERSION);
//    cout << version << endl;
//    shader.load("shaders/shader.vert", "shaders/shader.frag");
    ofEnableAlphaBlending();
    shader.setupShaderFromFile(GL_FRAGMENT_SHADER, "shaders/shader.frag");
    shader.linkProgram();
    minEdge = 0;
    maxEdge = 1;
    shader.begin();
    shader.setUniform1f("minEdge",minEdge);
    shader.setUniform1f("maxEdge",maxEdge);
    shader.end();
    cam.setup();
    cam.listDepthModes();
    cam.setDepthMode(4);
    
    fbo.allocate(cam.depthWidth, cam.depthHeight);//,GL_LUMINANCE);
    
    fileName = "testMovie";
    fileExt = ".mov";
    
    recorder.setPixelFormat("gray");
    bRecording = false;

    windowResized(ofGetWidth(), ofGetHeight());
}


//--------------------------------------------------------------
void ofApp::update(){
    cam.update();
    
    if(cam.bNewDepth){
        ofSetColor(255);
        fbo.begin();
        ofClear(0);
        shader.begin();
        cam.depthTexture.draw(ofPoint());
        shader.end();
        
        fbo.end();
        
        if (bRecording) {
            ofPixels pixels;
            fbo.readToPixels(pixels);
            pixels.setImageType(OF_IMAGE_GRAYSCALE);
            recorder.addFrame(pixels);
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(0);
    ofPushMatrix();
    ofMultMatrix(mat);
    fbo.draw(ofPoint());
    ofPopMatrix();
    
    stringstream ss;
    ss << "video queue size: " << recorder.getVideoQueueSize() << endl
    << "audio queue size: " << recorder.getAudioQueueSize() << endl
    << "FPS: " << ofGetFrameRate() << endl
    << (bRecording?"pause":"start") << " recording: r" << endl
    << (bRecording?"close current video file: c":"") << endl;
    
    ofSetColor(0,0,0,100);
    ofRect(0, 0, 260, 75);
    ofSetColor(255, 255, 255);
    ofDrawBitmapString(ss.str(),15,15);
    
    if(bRecording){
        ofSetColor(255, 0, 0);
        ofCircle(ofGetWidth() - 20, 20, 5);
    }
    
}

void ofApp::exit() {
    recorder.close();
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    switch (key) {
        case 't':
            ofToggleFullscreen();
            break;
        case 's':
            shader.setupShaderFromFile(GL_FRAGMENT_SHADER, "shaders/shader.frag");
            shader.linkProgram();
            break;
        case 'r':
            bRecording = !bRecording;
            if(bRecording && !recorder.isInitialized()) {
                recorder.setup(fileName+ofGetTimestampString()+fileExt, cam.depthWidth, cam.depthHeight, 30);
            }
            break;
        case 'c':
            bRecording = false;
            recorder.close();
            break;
        default:
            break;
    }
    
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){
    
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){
    ofVec2f pos = imat.preMult(ofVec3f(x,y));
    ofVec2f dim = imat.preMult(ofVec3f(ofGetWidth(),ofGetHeight()));
    float diff = 0.2*((pos-downPos)/dim.x).x;
    
    shader.begin();
    switch (button) {
        case 0:
             shader.setUniform1f("minEdge",ofClamp(minEdge+diff, 0.0,1.0));
            break;
        case 2:
            shader.setUniform1f("maxEdge",ofClamp(maxEdge+diff, 0.0,1.0));
            break;
            
        default:
            break;
    }
    shader.end();
    
    
    
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    downPos = imat.preMult(ofVec3f(x,y));
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    ofVec2f pos = imat.preMult(ofVec3f(x,y));
    ofVec2f dim = imat.preMult(ofVec3f(ofGetWidth(),ofGetHeight()));
    float diff = 0.2*((pos-downPos)/dim.x).x;
    
    switch (button) {
        case 0:
            minEdge = ofClamp(minEdge+diff, 0.0,1.0);
            break;
        case 2:
            maxEdge = ofClamp(maxEdge+diff, 0.0,1.0);
            break;
            
        default:
            break;
    }
   
//    cout << minEdge << '\t' << maxEdge << endl;
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    float scale = MIN((float)w/(float)CAM_WIDTH,(float)h/(float)CAM_HEIGHT);
    
    mat = ofMatrix4x4::newTranslationMatrix(0.5*(ofVec2f(w,h)-scale*ofVec2f(CAM_WIDTH,CAM_HEIGHT)));
    mat.preMult(ofMatrix4x4::newScaleMatrix(scale, scale, 1.0));
    imat = mat.getInverse();
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
