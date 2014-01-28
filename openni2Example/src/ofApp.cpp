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

    windowResized(ofGetWidth(), ofGetHeight());
}

//--------------------------------------------------------------
void ofApp::update(){
    cam.update();
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(0);
    ofPushMatrix();
    ofMultMatrix(mat);
    shader.begin();
    cam.depthTexture.draw(ofPoint());
    shader.end();
    ofPopMatrix();
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
