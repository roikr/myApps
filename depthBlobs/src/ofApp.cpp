#include "ofApp.h"

#define STRINGIFY(A) #A

//--------------------------------------------------------------
void ofApp::setup(){
    ofDisableArbTex();
    
    cam.setup();
    cam.listDepthModes();
    cam.setDepthMode(4);
    depthTexture.allocate(cam.depthWidth, cam.depthHeight, GL_R16 );
    
    
    fbo.allocate(cam.depthWidth, cam.depthHeight);
    thresh.allocate(cam.depthWidth, cam.depthHeight,GL_RGB);
    colorImg.allocate(cam.depthWidth, cam.depthHeight);
    grayImg.allocate(cam.depthWidth, cam.depthHeight);
    
    ofSetWindowShape(fbo.getWidth(), fbo.getHeight());
    
    
    
    string vertex = STRINGIFY(
                              \n#version 150\n
                              uniform mat4 modelViewProjectionMatrix;
                              in vec4 position;
                              in vec2 texcoord;
                              
                              out vec2 texCoordVarying;
                              
                              void main() {
                                  texCoordVarying = texcoord;
                                  gl_Position = modelViewProjectionMatrix * position;
                              }
                              );
    
    string fragment = STRINGIFY(
                                \n#version 150\n
                                uniform sampler2D tex0;
                                uniform float minEdge;
                                uniform float maxEdge;
                                
                                in vec2 texCoordVarying;
                                out vec4 fragColor;
                                
                                void main(void) {
                                    float sample = texture(tex0,texCoordVarying).r;
                                    float dist = 1-(sample-minEdge)/(maxEdge-minEdge);
                                    fragColor = vec4(vec3(dist*(step(minEdge,sample)-step(maxEdge,sample))),1.0);
                                    
                                }
    
                                );
    
    
    
    shader.setupShaderFromSource(GL_VERTEX_SHADER, vertex);
    shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragment);
    shader.bindDefaults();
    shader.linkProgram();
    
    
    string thresholdFrag = STRINGIFY(
                                \n#version 150\n
                                uniform sampler2D tex0;
                                uniform float minEdge;
                                uniform float maxEdge;
                                
                                in vec2 texCoordVarying;
                                out vec4 fragColor;
                                
                                void main(void) {
                                    float sample = texture(tex0,texCoordVarying).r;
                                    float dist = step(minEdge,sample)-step(maxEdge,sample);
                                    fragColor = vec4(vec3(dist),1.0);
                                    
                                }
                                
                                );
    
    threshold.setupShaderFromSource(GL_VERTEX_SHADER, vertex);
    threshold.setupShaderFromSource(GL_FRAGMENT_SHADER, thresholdFrag);
    threshold.bindDefaults();
    threshold.linkProgram();
    
    gui.setup("panel");
    gui.add(minEdge.set("minEdge", 0.003, 0.0, 0.01));
    gui.add(maxEdge.set("maxEdge", 0.007, 0.0, 0.02));
    gui.add(minArea.set("minArea",0.1,0,1));
    gui.add(maxArea.set("maxArea", 0.5, 0, 1));
    gui.loadFromFile("settings.xml");
}

//--------------------------------------------------------------
void ofApp::update(){
    cam.update();
    
    if (cam.bNewDepth) {
        ofSetColor(255);
        
        depthTexture.loadData(cam.getDepth(), cam.depthWidth, cam.depthHeight,GL_RED);
        
        fbo.begin();
        ofClear(0);
        shader.begin();
        shader.setUniform1f("minEdge", minEdge);
        shader.setUniform1f("maxEdge",maxEdge);
        
        depthTexture.draw(0,0);
        shader.end();
        fbo.end();
        
    
        thresh.begin();
        ofClear(0);
        threshold.begin();
        threshold.setUniform1f("minEdge", minEdge);
        threshold.setUniform1f("maxEdge",maxEdge);
        depthTexture.draw(0,0);
        threshold.end();
        thresh.end();
        
        ofPixels pixels;
        thresh.readToPixels(pixels);
        colorImg.setFromPixels(pixels);
        grayImg = colorImg;
        
        /*
        ofPixels pixels;
        thresh.readToPixels(pixels);
        pixels.setImageType(OF_IMAGE_GRAYSCALE);
        grayImg.setFromPixels(pixels);
        */
        float size = grayImg.getWidth()*grayImg.getHeight();
        contour.findContours(grayImg, minArea*size, maxArea*size, 10, false);
        
    }
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(0);
    
    ofPushMatrix();
    ofScale(0.5, 0.5);
    
    
    ofSetColor(255);
    fbo.draw(0, 0);
    thresh.draw(cam.depthWidth,0);
    
    
    
    for (int i = 0; i < contour.nBlobs; i++){
        contour.blobs[i].draw(cam.depthWidth,0);
    }
    
    
    ofPopMatrix();
    
    gui.draw();
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
