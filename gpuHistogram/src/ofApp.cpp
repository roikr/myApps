#include "ofApp.h"

#define STRINGIFY(A) #A

//--------------------------------------------------------------
void ofApp::setup(){
    ofDisableArbTex();
    cam.initGrabber(1280, 720);
    cout << cam.getWidth() << '\t' << cam.getHeight() << endl;
    
    ofFbo::Settings s;
    s.width			= 256;
    s.height			= 1;
    s.internalformat   =GL_R32F; // GL_R16

    fbo.allocate(s);
    
//    fbo.allocate(256,1 );
    
    ofSetWindowShape(cam.getWidth(), cam.getHeight());
    
// references:
//      http://www.shaderwrangler.com/publications/histogram/histogram_cameraready.pdf
//  GPUImage histogram filter
    
    string vertex = STRINGIFY(
                              \n#version 150\n
                              uniform mat4 modelViewProjectionMatrix;
                              uniform	sampler2D tex0;
                              in vec4 position;
                              out vec4 color;
                              void main() {
                                
                                  vec2 uDims = textureSize(tex0,0);
//                                  vec2 samplePos = vec2(gl_InstanceID%int(uDims.x),gl_InstanceID/uDims.x);
                                  
                                  float luminance = dot(texture(tex0,vec2(position.x/uDims.x,position.y/uDims.y)).rgb,vec3(0.2125, 0.7154, 0.0721));
                                  color = vec4(vec3(luminance),1);
                                  
                                  vec4 pos = vec4(luminance*255,1,0,1.0);
                                  
                                  gl_Position = modelViewProjectionMatrix*pos;
                                  
                              }
                              );
    
    string fragment = STRINGIFY(
                                \n#version 150\n
                                                                
                                in vec4 color;
                                out vec4 fragColor;
                                uniform int height;
                                uniform int width;
                                uniform float scale;
                                
                                void main(void) {
                                    
                                    fragColor = vec4(vec3(scale/(width*height)),1.0);

                                    
                                }
                                
                                );
    
    
    
    shader.setupShaderFromSource(GL_VERTEX_SHADER, vertex);
    shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragment);
    shader.bindDefaults();
    shader.linkProgram();
    shader.begin();
    shader.setUniformTexture("tex0", cam.getTextureReference(), 0);
    shader.setUniform1i("width", cam.getWidth());
    shader.setUniform1i("height", cam.getHeight());
    shader.setUniform1f("scale", 10.0);
    shader.end();
    
    
    string histVertex = STRINGIFY(
                              \n#version 150\n
                              uniform mat4 modelViewProjectionMatrix;
                              in vec4 position;
                              in vec2 texcoord;
                                  
                              out vec2 texCoordVarying;
                              out float heightVarying;
                             
                              void main() {
                                  
                                  
                                  texCoordVarying = vec2(texcoord.x,0.5);
                                  heightVarying = 1.0 - texcoord.y;
                                  gl_Position = modelViewProjectionMatrix*position;
                                  
                              }
                              );
    
    string histFragment = STRINGIFY(
                                \n#version 150\n
                                
                                uniform sampler2D tex0;
                                in vec2 texCoordVarying;
                                in float heightVarying;
                                    
                                out vec4 fragColor;
                                
                                
                                void main(void) {
                                    float sample = texture(tex0,texCoordVarying).r;
                                    float heightTest = step(heightVarying,sample);
                                    
                                    fragColor = vec4( vec3(heightTest),0.5);
                                    
                                }
                                
                                );
    
    
    
    hist.setupShaderFromSource(GL_VERTEX_SHADER, histVertex);
    hist.setupShaderFromSource(GL_FRAGMENT_SHADER, histFragment);
    hist.bindDefaults();
    hist.linkProgram();
   
    
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
        shader.begin();
        ofClear(0);
        glBlendFunc(GL_ONE, GL_ONE);
        grid.drawVertices();
        //point.drawInstanced(OF_MESH_POINTS, cam.getWidth()*cam.getHeight());
        shader.end();
        fbo.end();
        ofEnableAlphaBlending();
    }
    
}

//--------------------------------------------------------------
void ofApp::draw(){

    ofBackground(0);
    ofSetColor(255);
    cam.draw(0, 0);
    hist.begin();
    fbo.draw(0, 0,ofGetWidth(),ofGetHeight());
    hist.end();

    ofSetColor(10);
    
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
    shader.begin();
    shader.setUniform1f("scale", 10.0*(1-float(y)/(float)ofGetHeight()));
    shader.end();
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
