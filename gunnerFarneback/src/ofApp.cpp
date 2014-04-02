#include "ofApp.h"

#define STRINGIFY(A) #A

//--------------------------------------------------------------
void ofApp::setup(){
    ofDisableArbTex();
    
	cam.setDesiredFrameRate(30);
	cam.initGrabber(640, 480);
    
	flowSolver.setup(cam.getWidth(), cam.getHeight(), 0.35, 5, 10, 1, 3, 2.25, false, false);
    
	ofEnableAlphaBlending();
	ofSetBackgroundAuto(true);
	ofBackground(0);
    
	ofSetWindowShape(cam.getWidth(), cam.getHeight());
    
    tex.allocate(cam.getWidth(), cam.getHeight(), GL_RG32F);
    
    
    string vertex =  STRINGIFY(
                               
                               \n#version 150\n
                               uniform mat4 modelViewProjectionMatrix;
                               in vec4 position;
                               in vec2 texcoord;
                               
                               out vec2 texCoordVarying;
                               
                               
                               void main()
                               {
                                   texCoordVarying = texcoord;
                                   gl_Position = modelViewProjectionMatrix*position;
                               }
                               );
    
    string fragment = STRINGIFY(
                                \n#version 150\n
                                uniform sampler2D tex0;
                                
                                
                                const vec4 lumcoeff = vec4(0.299,0.587,0.114,0.);
                                
                                
                                in vec2 texCoordVarying;
                                out vec4 fragColor;
                                
                                vec3 hsv2rgb(vec3 c)
                                {
                                    vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
                                    vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
                                    return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
                                }
                                
                                void main()
                                {
                                    vec2 vel = texture(tex0,texCoordVarying).xy;
                                    
                                    
                                    float PI = 3.14159265358979323846264;
                                    float hue = (atan(vel.y,vel.x)+PI)/(2*PI);
                                    vec3 rgb = hsv2rgb(vec3(hue,1.0,log(length(vel))));
                                    fragColor = vec4(rgb,1.0);
                                }
                                );
    
    shader.setupShaderFromSource(GL_VERTEX_SHADER, vertex);
	shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragment);
    shader.bindDefaults();
	shader.linkProgram();
}

//--------------------------------------------------------------
void ofApp::update(){
    cam.update();
	if(cam.isFrameNew()) {
		flowSolver.update(cam);
        ofSetPixelStorei(cam.getWidth(),4,2);
        
        tex.loadData(flowSolver.getFlowData(), cam.getWidth(), cam.getHeight(), GL_RG,GL_FLOAT);
        
	}
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofSetColor(255, 255, 255);
	//videoSource.draw(0, 0);
	//flowSolver.drawColored(videoSource.getWidth(), videoSource.getHeight(), 10, 3);
    //flowSolver.draw(videoSource.getWidth(), videoSource.getHeight());
    //tex.draw(0, 0);
    
    shader.begin();
    tex.draw(0, 0);
    shader.end();
    
	stringstream m;
	m << "fps " << ofGetFrameRate() << endl
	<< "pyramid scale: " << flowSolver.getPyramidScale() << " p/P" << endl
	<< "pyramid levels: " << flowSolver.getPyramidLevels() << " l/L" << endl
	<< "averaging window size: " << flowSolver.getWindowSize() << " w/W" << endl
	<< "iterations per level: " << flowSolver.getIterationsPerLevel() << " i/I" << endl
	<< "expansion area: " << flowSolver.getExpansionArea() << " a/A" << endl
	<< "expansion sigma: " << flowSolver.getExpansionSigma() << " s/S" << endl
	<< "flow feedback: " << flowSolver.getFlowFeedback() << " f/F" << endl
	<< "gaussian filtering: " << flowSolver.getGaussianFiltering() << " g/G";
    
	ofDrawBitmapString(m.str(), 20, 20);
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    if(key == 'p') { flowSolver.setPyramidScale(ofClamp(flowSolver.getPyramidScale() - 0.01,0.0,1.0)); }
	else if(key == 'P') { flowSolver.setPyramidScale(ofClamp(flowSolver.getPyramidScale() + 0.01,0.0,1.0)); }
	else if(key == 'l') { flowSolver.setPyramidLevels(MAX(flowSolver.getPyramidLevels() - 1,1)); }
	else if(key == 'L') { flowSolver.setPyramidLevels(flowSolver.getPyramidLevels() + 1); }
	else if(key == 'w') { flowSolver.setWindowSize(MAX(flowSolver.getWindowSize() - 1,1)); }
	else if(key == 'W') { flowSolver.setWindowSize(flowSolver.getWindowSize() + 1); }
	else if(key == 'i') { flowSolver.setIterationsPerLevel(MAX(flowSolver.getIterationsPerLevel() - 1,1)); }
	else if(key == 'I') { flowSolver.setIterationsPerLevel(flowSolver.getIterationsPerLevel() + 1); }
	else if(key == 'a') { flowSolver.setExpansionArea(MAX(flowSolver.getExpansionArea() - 2,1)); }
	else if(key == 'A') { flowSolver.setExpansionArea(flowSolver.getExpansionArea() + 2); }
	else if(key == 's') { flowSolver.setExpansionSigma(ofClamp(flowSolver.getExpansionSigma() - 0.01,0.0,10.0)); }
	else if(key == 'S') { flowSolver.setExpansionSigma(ofClamp(flowSolver.getExpansionSigma() + 0.01,0.0,10.0)); }
	else if(key == 'f') { flowSolver.setFlowFeedback(false); }
	else if(key == 'F') { flowSolver.setFlowFeedback(true); }
	else if(key == 'g') { flowSolver.setGaussianFiltering(false); }
	else if(key == 'G') { flowSolver.setGaussianFiltering(true); }
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
