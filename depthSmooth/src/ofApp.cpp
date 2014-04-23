#include "ofApp.h"

#define STRINGIFY(A) #A

//--------------------------------------------------------------
void ofApp::setup(){

    gui.setup("panel");
    gui.add(fps.set("fps",""));
    
    //gui.loadFromFile("settings.xml");
    
    
    ofDisableArbTex();
    
    
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
                                uniform sampler2D tex1;
                                uniform sampler2D tex2;
                                uniform sampler2D tex3;
                                uniform int counter;
                                
                                in vec2 texCoordVarying;
                                out vec4 fragColor;
                                
                                void main(void) {
                                    float c[4];
                                    c[0]= texture(tex0,texCoordVarying).r;
                                    c[1] = texture(tex1,texCoordVarying).r;
                                    c[2] = texture(tex2,texCoordVarying).r;
                                    c[3] = texture(tex3,texCoordVarying).r;
                                    
                                    
                                    fragColor = vec4(0.5*c[(counter-1) % 4 ]+0.3*c[(counter-2) % 4]+0.15*c[(counter-3) % 4]+0.05*c[(counter-4) % 4]);
                                    
                                }
                                
                                );
    
    
    
    shader.setupShaderFromSource(GL_VERTEX_SHADER, vertex);
    shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragment);
    shader.bindDefaults();
    shader.linkProgram();
    
    cam.setup();
    cam.listDepthModes();
    cam.setDepthMode(9);
    textures.assign(4, ofTexture());
    
    shader.begin();
    for (int i=0; i<textures.size(); i++) {
        textures[i].allocate(cam.depthWidth, cam.depthHeight, GL_R16 );
        shader.setUniformTexture("tex"+ofToString(i), textures[i], i+1);
        cout << i << endl;
    }
    shader.end();

    
    
    counter = 1;
    

}

//--------------------------------------------------------------
void ofApp::update(){
    cam.update();
    if (cam.bNewDepth) {
        textures[counter % textures.size()].loadData(cam.getDepth(), cam.depthWidth, cam.depthHeight,GL_RED);
        counter++;
    }
    
    bShowGui=true;
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofClear(0);
    ofSetColor(255);
    ofFill();
    shader.begin();
    shader.setUniform1i("counter", counter);

    textures[(counter-1)%textures.size()].draw(0,0);
    shader.end();
    
    if (bShowGui) {
        gui.draw();
    }
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    bShowGui=!bShowGui;
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
