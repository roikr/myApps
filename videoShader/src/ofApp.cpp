#include "ofApp.h"
#include "Shaders.h"


//--------------------------------------------------------------
void ofApp::setup(){
    gui.setup("panel");
    gui.add(fps.set("fps",""));
//    gui.add(hue.set("hue", 0.0,0.0,1.0));
//    gui.add(sat.set("sat", 0.0,0.0,1.0));
//    gui.add(offset.set("offset", 0.0,-0.5,0.5));
//    gui.add(var.set("var", 3,0.0,5.0));
//    gui.add(rad.set("rad", 20,0.0,50));
#ifdef ECHO_SHADER
    gui.add(alpha.set("alpha", 1.0, 0.0, 1.0));
#endif
    
    
    gui.loadFromFile("settings.xml");
    
    rad.addListener(this, &ofApp::paramChanged);
    var.addListener(this, &ofApp::paramChanged);
    
    ofDisableArbTex();
    
    video.loadMovie("yoav.mov");
    video.setLoopState(OF_LOOP_NORMAL);
    video.play();
    
    ofFbo::Settings s;
    s.width = video.getWidth();
    s.height = video.getHeight();
    s.internalformat = GL_R16; // GL_R8 is enough but GL_R8 is not supported in ofGetImageTypeFromGLType()
    depthFbo.allocate(s);
    
    ping.allocate(video.getWidth(),video.getHeight());
    pong.allocate(video.getWidth(),video.getHeight());
    
    createColor2GrayShader(depthShader);
    
    //createHSLShader(shader);
//    createDepthBlurShader(shader,rad,var);
#ifdef ECHO_SHADER
    
    createEchoShader(shader);
    shader.begin();
    shader.setUniformTexture("tex0", pong.getTextureReference(), 1);
    shader.end();
#endif
    
}

//--------------------------------------------------------------
void ofApp::update(){
    fps = ofToString(ofGetFrameRate());
    video.update();
    if (video.isFrameNew()) {
        depthFbo.begin();
        depthShader.begin();
        video.draw(0, 0);
        depthShader.end();
        depthFbo.end();

        /*
        ping.begin();
        shader.begin();
        shader.setUniform1f("hue", hue);
        shader.setUniform1f("sat", sat);
        shader.setUniform1f("offset", offset);
        depthFbo.draw(0,0);
        shader.end();
        ping.end();
         
        
        ping.begin();
        shader.begin();
        shader.setUniform2f("dir", 1.0/video.getWidth(), 0);
        depthFbo.draw(0,0);
        shader.end();
        ping.end();
        
        pong.begin();
        shader.begin();
        shader.setUniform2f("dir", 0, 1.0/video.getHeight());
        ping.draw(0,0);
        shader.end();
        pong.end();
         */
#ifdef ECHO_SHADER
        ping.begin();
        shader.begin();
        shader.setUniform1f("alpha",alpha);
        depthFbo.draw(0,0);
        shader.end();
        ping.end();
        
        pong.begin();
        ping.draw(0, 0);
        pong.end();
        
        
#endif
        
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    ping.draw(0, 0);
    gui.draw();
}

void ofApp::paramChanged(float &param) {
    createDepthBlurShader(shader,rad,var);
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
