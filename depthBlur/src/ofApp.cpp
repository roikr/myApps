#include "ofApp.h"
#include "Shaders.h"

#define STRINGIFY(A) #A

//--------------------------------------------------------------
void ofApp::setup(){
    gui.setup("panel");
    gui.add(fps.set("fps",""));
    gui.add(minEdge.set("minEdge", 0.0, 0.0, 0.5));
    gui.add(maxEdge.set("maxEdge", 0.5, 0.0, 0.5));
    gui.add(variance.set("variance", .143,0,10));
    gui.add(radius.set("radius", 7,0,20)); // fps drop above 14
    gui.add(scale.set("scale", 1.0,0.1,10)); 
    gui.loadFromFile("settings.xml");
    
    ofDisableArbTex();

    
    createDepthShader(depthShader);
    
    
    cam.setup();
    cam.listDepthModes();
    cam.setDepthMode(9);
    
    
    tex.allocate(cam.depthWidth, cam.depthHeight, GL_R16 );
    
    ping.allocate(tex.getWidth(), tex.getHeight());
    pong.allocate(tex.getWidth(), tex.getHeight());
    
    bShowGui = true;
    
    
    createDepthBlurShader(blurShader, radius, variance);
    
    

}

//--------------------------------------------------------------
void ofApp::update(){
    fps = ofToString(ofGetFrameRate());
    cam.update();
    if (cam.bNewDepth) {
        tex.loadData(cam.getDepth(), cam.depthWidth, cam.depthHeight,GL_RED);
        ping.begin();
        depthShader.begin();
        depthShader.setUniform1f("minEdge", minEdge);
        depthShader.setUniform1f("maxEdge", maxEdge);
        tex.draw(0, 0);
        depthShader.end();
        ping.end();
        
        pong.begin();
        blurShader.begin();
        blurShader.setUniform2f("dir", scale/tex.getWidth(), 0);
        
        ping.draw(0,0);
        blurShader.end();
        pong.end();
        
        ping.begin();
        blurShader.begin();
        blurShader.setUniform2f("dir", 0, scale/tex.getHeight());
        
        pong.draw(0,0);
        blurShader.end();
        ping.end();

    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofClear(0);
    ofSetColor(255);
    
    ping.draw(0,0);
    
    
    if (bShowGui) {
        gui.draw();
    }
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key){
//    bShowGui=!bShowGui;
    createDepthBlurShader(blurShader, radius, variance);
    
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
