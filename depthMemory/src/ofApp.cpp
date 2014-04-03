#include "ofApp.h"

#define QUEUE_LENGTH 30
#define MIN_DURATION 5
#define MAX_DURATION 30
#define HIDE_DURATION 5


#define STRINGIFY(A) #A


//--------------------------------------------------------------
void ofApp::setup(){
    ofDisableArbTex();
    
    cam.setup();
    cam.listDepthModes();
    cam.setDepthMode(9);
    depthTexture.allocate(cam.depthWidth, cam.depthHeight, GL_R16 );
    
    
    // request single channel fbo (no depth no stencil)
    ofFbo::Settings settings;
    settings.width = cam.depthWidth;
    settings.height = cam.depthHeight;
    settings.internalformat = GL_R16; // GL_R8 is not used in ofGetImageTypeFromGLType()
    fbo.allocate(settings);
    thresholdFbo.allocate(settings);
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
    
    thresholdShader.setupShaderFromSource(GL_VERTEX_SHADER, vertex);
    thresholdShader.setupShaderFromSource(GL_FRAGMENT_SHADER, thresholdFrag);
    thresholdShader.bindDefaults();
    thresholdShader.linkProgram();
    
    gui.setup("panel");
    gui.add(queueSize.set("queueSize",""));
    
    gui.add(minEdge.set("minEdge", 0.0, 0.0, 1.0));
    gui.add(maxEdge.set("maxEdge", 0.0, 0.0, 1.0));
    gui.add(minArea.set("minArea",0.1,0,1));
    gui.add(maxArea.set("maxArea", 0.5, 0, 1));
    gui.add(startArea.set("startArea",0.1,0,1));
    gui.add(stopArea.set("stopArea",0.05,0,1));
    
    gui.loadFromFile("settings.xml");
    
    
    bRecording = false;
    recorder.setPixelFormat("gray");
    
    bReset = false;
    
    ofFile file("memories.csv");
    ofBuffer buffer(file);
    while(!buffer.isLastLine()) {
        string line = buffer.getNextLine();
        vector<string> strs = ofSplitString(line, ",");
        
        memory m;
        m.pos = ofVec2f(ofToFloat(strs[0]),ofToFloat(strs[1]));
        m.filename = strs[2];
        
        memories.push_back(m);
        
        cout << m.pos << ',' << m.filename << endl;
    }
    
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
        
        ofPixels pixels;
        fbo.readToPixels(pixels);
    
        thresholdFbo.begin();
        ofClear(0);
        thresholdShader.begin();
        thresholdShader.setUniform1f("minEdge", minEdge);
        thresholdShader.setUniform1f("maxEdge",maxEdge);
        depthTexture.draw(0,0);
        thresholdShader.end();
        thresholdFbo.end();
    
        ofPixels threshPixels;
        thresholdFbo.readToPixels(threshPixels);
        grayImg.setFromPixels(threshPixels);
        
        float size = grayImg.getWidth()*grayImg.getHeight();
        contour.findContours(grayImg, minArea*size, maxArea*size, 10, false);
        
        if (!bRecording) {
            
            frames.push_front(pixels);
            while (frames.size()>QUEUE_LENGTH) {
                frames.pop_back();
            }
            
            
            if (bReset && !contour.nBlobs) {
                bReset = false;
            }
        
            if (!bReset && contour.nBlobs && contour.blobs[0].area>=startArea*size) {
                
                if(!recorder.isInitialized()) {
                    bRecording = true;
                    startTime = ofGetElapsedTimef();
                    stopTime = startTime + HIDE_DURATION; // safty margin
                    currentMemory.pos = contour.blobs[0].centroid;
                    currentMemory.filename = "testMovie"+ofGetTimestampString()+".mov";
                    recorder.setup(currentMemory.filename, cam.depthWidth, cam.depthHeight, 30);
                    
                    for (deque<ofPixels>::reverse_iterator riter=frames.rbegin();riter!=frames.rend();riter++) {
                        
                        recorder.addFrame(*riter);
                    }
                    frames.clear();
                } else {
                    recorder.close();
                }
            }
            
        } else {
            recorder.addFrame(pixels);
            
            
            if (contour.nBlobs && contour.blobs[0].area>=stopArea*size) {
                stopTime = ofGetElapsedTimef() + HIDE_DURATION;
            }
            
            
            
            if (ofGetElapsedTimef()-startTime >= MAX_DURATION || ofGetElapsedTimef() > stopTime ) {
                bRecording = false;
                recorder.close();
                bReset = ofGetElapsedTimef()-startTime >= MAX_DURATION ;
                
                if (ofGetElapsedTimef()-startTime > MIN_DURATION) {
                    memories.push_back(currentMemory);
                    
                    ofFile file("memories.csv",ofFile::Append);
                    stringstream ss;
                    
                    ss << currentMemory.pos << ',' << currentMemory.filename <<  endl;
                    file << ss.str() ;
                    file.close();
                    
                }
            }
            
        }
        
        queueSize=ofToString(frames.size());
        
    }
    
    
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(0);
    
    ofPushMatrix();
    ofScale(0.5, 0.5);
    
    
    ofSetColor(255);
    fbo.draw(0, 0);
    thresholdFbo.draw(cam.depthWidth,0);
    
    
    
    for (int i = 0; i < contour.nBlobs; i++){
        contour.blobs[i].draw(cam.depthWidth,0);
    }
    
    
    ofPopMatrix();
    
    gui.draw();
    
    
    float area = contour.nBlobs ? contour.blobs[0].area / (contour.getWidth()*contour.getHeight()): 0;
    
    stringstream ss;
    
    ss << "detected area: " << area << endl
    << "video queue size: " << recorder.getVideoQueueSize() << endl
    << "audio queue size: " << recorder.getAudioQueueSize() << endl
    << "FPS: " << ofGetFrameRate() << endl;
    
    if (bRecording) {
        ss << "video length: " << ofGetElapsedTimef() - startTime << endl
        << "hide duration: " << stopTime - ofGetElapsedTimef() << endl;
    }
    
    ofSetColor(255);
    ofPushMatrix();
    ofTranslate(ofGetWidth()-260, 0);
    //ofRect(0, 0, 260, 75);
    //ofSetColor(255, 255, 255);
    ofDrawBitmapString(ss.str(),15,15);
    ofPopMatrix();
    
    if(bRecording){
        ofSetColor(255, 0, 0);
        ofFill();
        ofCircle(ofGetWidth() - 20, 20, 5);
        ofNoFill();
    }
    
}

void ofApp::exit() {
    recorder.close();
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
