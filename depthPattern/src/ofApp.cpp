#include "ofApp.h"

#define STRINGIFY(A) #A

#define PARAM_MIN 0.097
#define PARAM_MAX 0.103

//#define PARAM_MIN 0.0097
//#define PARAM_MAX 0.01025

//#define PARAM_MIN 0.0195
//#define PARAM_MAX 0.02

//#define PARAM_MIN 0.0148
//#define PARAM_MAX 0.0152

//--------------------------------------------------------------
void ofApp::setup(){
    ofDisableArbTex();
    cam.setup();
    cam.listDepthModes();
    cam.setDepthMode(9);
    depthTexture.allocate(cam.depthWidth, cam.depthHeight, GL_R16);
    
    ofFbo::Settings settings;
    settings.width = cam.depthWidth;
    settings.height = cam.depthHeight;
    settings.internalformat = GL_R16; // GL_R8 is not used in ofGetImageTypeFromGLType()
    
    patternsFbo.allocate(settings);
    shapesFbo.allocate(settings);
    
    patternsImg.allocate(cam.depthWidth, cam.depthHeight);
    shapesImg.allocate(cam.depthWidth, cam.depthHeight);
    
    ofSetWindowShape(cam.depthWidth*2, cam.depthHeight);
    
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
                                uniform int inv;
                                uniform int thresh;
                                uniform float threshVal;
                                
                                in vec2 texCoordVarying;
                                out vec4 fragColor;
                                
                                void main(void) {
                                    float sample = texture(tex0,texCoordVarying).r;
                                    float dist = (sample-minEdge)/(maxEdge-minEdge);
                                    float invDist = mix(1-dist,dist,inv);
                                    float threshStep = mix(step(sample,threshVal),step(threshVal,sample),inv);
                                    float color =   mix(invDist,threshStep,thresh)*(step(minEdge,sample)-step(maxEdge,sample));
                                    
                                    
                                    fragColor = vec4(vec3(color),1.0);
                                    
                                }
                                
                                );
    
    
    
    shader.setupShaderFromSource(GL_VERTEX_SHADER, vertex);
    shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragment);
    shader.bindDefaults();
    shader.linkProgram();
    
    gui.setup("panel");
    
    gui.add(fps.set("fps",""));
    gui.add(patternsInv.set("patternsInv",false));
    gui.add(patternsThresh.set("patternsThresh",false));
    gui.add(patternsThreshVal.set("patternsThreshVal", (PARAM_MIN+PARAM_MAX)/2.0, PARAM_MIN, PARAM_MAX));
    gui.add(patternsMin.set("patternsMin", PARAM_MIN, PARAM_MIN, PARAM_MAX));
    gui.add(patternsMax.set("patternsMax", PARAM_MAX, PARAM_MIN, PARAM_MAX));
    
    
    gui.add(shapesInv.set("shapesInv",false));
    gui.add(shapesThresh.set("shapesThresh",false));
    gui.add(shapesThreshVal.set("shapesThreshVal", (PARAM_MIN+PARAM_MAX)/2.0, PARAM_MIN, PARAM_MAX));
    gui.add(shapesMin.set("shapesMin",PARAM_MIN, PARAM_MIN, PARAM_MAX));
    gui.add(shapesMax.set("shapesMax", PARAM_MAX, PARAM_MIN, PARAM_MAX));
    
    gui.add(minArea.set("minArea",0.0,0,0.1));
    gui.add(maxArea.set("maxArea", 0.1, 0, 0.1));

    
    gui.loadFromFile("settings.xml");
}

//--------------------------------------------------------------
void ofApp::update(){
    fps = ofToString(ofGetFrameRate());
    cam.update();
    
    if (cam.bNewDepth) {
        ofSetColor(255);
        
        float size = cam.depthWidth*cam.depthHeight;
        
        depthTexture.loadData(cam.getDepth(), cam.depthWidth, cam.depthHeight,GL_RED);
        
        patternsFbo.begin();
        ofClear(0);
        shader.begin();
        shader.setUniform1f("threshVal", patternsThreshVal);
        shader.setUniform1f("minEdge", patternsMin);
        shader.setUniform1f("maxEdge",patternsMax);
        shader.setUniform1i("inv", patternsInv);
        shader.setUniform1i("thresh", true);
        
        depthTexture.draw(0,0);
        shader.end();
        patternsFbo.end();
        
        ofPixels pixels;
        patternsFbo.readToPixels(pixels);
        patternsImg.setFromPixels(pixels);
        patternsBlobs.findContours(patternsImg, minArea*size, maxArea*size, 10, false);
        
        
        shapesFbo.begin();
        ofClear(0);
        shader.begin();
        shader.setUniform1f("threshVal", shapesThreshVal);
        shader.setUniform1f("minEdge", shapesMin);
        shader.setUniform1f("maxEdge",shapesMax);
        shader.setUniform1i("inv", shapesInv);
        shader.setUniform1i("thresh", true);
        
        
        depthTexture.draw(0,0);
        shader.end();
        shapesFbo.end();
        
        shapesFbo.readToPixels(pixels);
        shapesImg.setFromPixels(pixels);
        shapesBlobs.findContours(shapesImg, minArea*size, maxArea*size, 10, false);
        
    }

}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(0);
    ofSetColor(255);
    
    
    
    shader.begin();
    shader.setUniform1f("threshVal", patternsThreshVal);
    shader.setUniform1f("minEdge", patternsMin);
    shader.setUniform1f("maxEdge",patternsMax);
    shader.setUniform1i("inv", patternsInv);
    shader.setUniform1i("thresh", patternsThresh);
    
    depthTexture.draw(0,0);
    shader.end();
    patternsBlobs.draw(0,0);
    
    ofPushMatrix();
    ofTranslate(depthTexture.getWidth(), 0);
    shader.begin();
    shader.setUniform1f("threshVal", shapesThreshVal);
    shader.setUniform1f("minEdge", shapesMin);
    shader.setUniform1f("maxEdge",shapesMax);
    shader.setUniform1i("inv", shapesInv);
    shader.setUniform1i("thresh", shapesThresh);
    
    
    depthTexture.draw(0,0);
    shader.end();
    shapesBlobs.draw(0,0);
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
