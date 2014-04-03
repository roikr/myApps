#include "ofApp.h"

#define STRINGIFY(A) #A

//--------------------------------------------------------------
void ofApp::setup(){
    ofDisableArbTex();
    cam.initGrabber(640, 480);
    fbo.allocate(cam.getWidth(), cam.getHeight());
    
    ofSetWindowShape(cam.getWidth(), cam.getHeight());
    
    
    string vertex = STRINGIFY(
                              \n#version 150\n
                              uniform mat4 modelViewProjectionMatrix;
                              uniform	sampler2D tex0;
                              in vec4 position;
                              out vec4 color;
                              void main() {
                                
                                  
                                  vec2 uDims = textureSize(tex0,0);
                                  vec2 samplePos = vec2(gl_InstanceID%int(uDims.x),gl_InstanceID/uDims.x);
                                  color = texture(tex0,vec2(position.x/uDims.x,position.y/uDims.y));
                                  gl_Position = modelViewProjectionMatrix*position;
                                  
                              }
                              );
    
    string fragment = STRINGIFY(
                                \n#version 150\n
                                                                
                                in vec4 color;
                                out vec4 fragColor;
                                
                                void main(void) {
                                    fragColor = color;
                                }
                                
                                );
    
    
    
    shader.setupShaderFromSource(GL_VERTEX_SHADER, vertex);
    shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragment);
    shader.bindDefaults();
    shader.linkProgram();
    
    shader.begin();
    shader.setUniformTexture("tex0", cam.getTextureReference(), 0);
    shader.end();
   
 
    grid.setMode(OF_PRIMITIVE_POINTS);
    
    for (int i=0;i<cam.getHeight();i++) {
        for (int j=0;j<cam.getWidth();j++) {
            grid.addVertex(ofVec3f(j,i,0));
        }
    }
    
    
    

}

//--------------------------------------------------------------
void ofApp::update(){
    ofSetWindowTitle(ofToString(ofGetFrameRate()));
    cam.update();
    if (cam.isFrameNew()) {
        fbo.begin();
        ofClear(0);
        shader.begin();
        grid.drawVertices();
        shader.end();
        fbo.end();
        
    }
}

//--------------------------------------------------------------
void ofApp::draw(){

    ofBackground(0);
    ofSetColor(255);
    fbo.draw(0, 0);
    
    
    
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){

}

//--------------------------------------------------------------
void ofApp::keyReleased(int key){

}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y ){

}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
