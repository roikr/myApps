#include "ofApp.h"
#include "Shaders.h"

#define STAGE_WIDTH 960.0
#define STAGE_HEIGHT 640.0

#define LAYERS_NUMBER 3
#define STRINGIFY(A) #A

enum {
    STATE_CALIB,
    STATE_BACKGROUND,
    STATE_IDLE,
    STATE_RECORD,
    
};
//--------------------------------------------------------------
void ofApp::setup(){
    
    ofDirectory dir;
    dir.allowExt("mov");
    dir.listDir(ofToDataPath("."));
    for (int i=0;i<dir.numFiles();i++) {
        memories.push_back(dir.getName(i));        
    }
    
    recSound.loadSound("camera memory sound 10 sec 48.wav");
    
    ofSetWindowShape(STAGE_WIDTH, STAGE_HEIGHT);
    
    
    ofDisableArbTex();
    
    gui.setup("panel");
    gui.add(fps.set("fps",""));
    gui.add(minEdge0.set("minEdge0", 0.0, 0.0, 1.0));
    gui.add(maxEdge0.set("maxEdge0", 1.0, 0.0, 1.0));
    gui.add(minEdge1.set("minEdge1", 0.0, 0.0, 1.0));
    gui.add(maxEdge1.set("maxEdge1", 1.0, 0.0, 1.0));
    gui.add(tolerance.set("tolerance", 0.1, 0.0, 1.0));
    gui.add(decay0.set("decay0", 0.9, 0.9, 1.0));
    gui.add(decay1.set("decay1", 0.9, 0.9, 1.0));
    gui.add(strobeRate.set("strobeRate", 15, 1, 30));
    gui.add(variance.set("variance", .143,0,10));
    gui.add(radius.set("radius", 7,0,20)); // fps drop above 14
    gui.add(hueRate.set("hueRate", 0.0,0.0,1.0));
    gui.add(sat.set("sat", 0.0,0.0,1.0));
    gui.add(offset.set("offset", 0.0,-0.5,0.5));
    
    gui.add(minArea.set("minArea",0.05,0,0.1));
    gui.add(maxArea.set("maxArea", 0.5, 0, 1));
    gui.add(blobDetection.set("blobDetection",false));
    gui.add(recordTime.set("recordTime",""));
    gui.add(waitTime.set("waitTime",""));
    gui.add(recordDuration.set("recordDuration",10,10,30));
    gui.add(minimumDuration.set("minimumDuration",3,0,5));
    gui.add(freezeDuration.set("freezeDuration",3,0,5));
    gui.add(waitDuration.set("waitDuration",2,0,10));
    gui.add(idleInterval.set("idleInterval",5,2,10));
    
    gui.add(videoQueue.set("videoQueue",""));
    gui.loadFromFile("settings.xml");
    
    recorder.setPixelFormat("gray");
    
#ifdef TARGET_OSX
    
    
    //    recorder.setFfmpegLocation("~/ffmpeg");
    recorder.setVideoCodec("mpeg4");
    recorder.setVideoBitrate("800k");
    
    
   
#else
    
    recorder.setFfmpegLocation(ofFilePath::getAbsolutePath("ffmpeg"));
    
#endif
    ofxOpenNI2::init();
    vector<string> devices = ofxOpenNI2::listDevices();
    
    if (devices.size()<2) {
        std:exit();
    }
    
    cam0.setup(devices[0]);
    cam0.setDepthMode(5);
    cam1.setup(devices[1]);
    cam1.setDepthMode(5);
    
    depthTex0.allocate(cam0.depthWidth, cam0.depthHeight, GL_R16 );
    depthTex1.allocate(cam0.depthWidth, cam0.depthHeight, GL_R16 );
    
    ofFbo::Settings s;
    s.width = depthTex0.getHeight()*2;
    s.height = depthTex0.getWidth();
    s.internalformat = GL_R16; // GL_R8 is not used in ofGetImageTypeFromGLType()
    depthFbo.allocate(s);
    recordFbo.allocate(s);
    backgroundFbo.allocate(s);
    blobFbo.allocate(s);
    
    recordFbo.begin();
    ofClear(0);
    recordFbo.end();
    
    createDepthShader(depthShader);
    createDepthBackgroundSubtractionShader(subtractShader);
    subtractShader.begin();
    subtractShader.setUniformTexture("bgTex", backgroundFbo.getTextureReference(), 1);
    subtractShader.end();
    
    
    string fragment = STRINGIFY(
                                \n#version 150\n
                                
                                uniform sampler2D tex0;
                                uniform sampler2D memTex;
                                
                                
                                uniform int frameNum;
                                uniform int strobeRate;
                                uniform float decay;
                                uniform float sat;
                                uniform float hue;
                                uniform float offset;
                                
                                in vec2 texCoordVarying;
                                out vec4 fragColor;
                                
                                
                                
                                
                                void main(void) {
                                    float c = texture(tex0,texCoordVarying).r;
                                    vec3 mem = texture(memTex,texCoordVarying).rgb;
                                    
                                    bool f = (frameNum % strobeRate) == 0;
                                    
                                    
                                    float lgt = c+offset;
//                                    float hue = float(frameNum/10 % 256)/255.0;
                                    float x= (1-abs(2*lgt-1))*sat;
                                    
                                    float h = fract(hue);
                                    vec3 y;
                                    y.r = abs(h * 6 - 3) - 1;
                                    y.g = 2 - abs(h * 6 - 2);
                                    y.b = 2 - abs(h * 6 - 4);
                                    
                                    vec3 col = (clamp(y,0,1)-0.5)*x+lgt;
                                    
                                    fragColor = vec4(mix(mem*decay,col,vec3(f && c>0)),1.0);
                                }
                                
                                );
    
    strobeShader.setupShaderFromSource(GL_VERTEX_SHADER, getSimpleVertex());
    strobeShader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragment);
    strobeShader.bindDefaults();
    strobeShader.linkProgram();
    
    ping.allocate(depthFbo.getWidth(),depthFbo.getHeight());
    pong.allocate(depthFbo.getWidth(),depthFbo.getHeight());
    
    for (int i=0;i<LAYERS_NUMBER;i++) {
        layer l;
        l.fbo.allocate(depthFbo.getWidth(),depthFbo.getHeight());
        l.fbo.begin();
        ofClear(0);
        l.fbo.end();
        layers.push_back(l);
        
    }
    
    camLayer.fbo.allocate(depthFbo.getWidth(),depthFbo.getHeight());
    camLayer.fbo.begin();
    ofClear(0);
    camLayer.fbo.end();
    camLayer.hueOffset=0;
    
    compFbo.allocate(depthFbo.getWidth(),depthFbo.getHeight());
    compFbo.begin();
    ofClear(0);
    compFbo.end();
    
    
    createBlurShader(blurShader, radius, variance);
    
    grayImg.allocate(depthFbo.getWidth(), depthFbo.getHeight());
    
    ofVideoPlayer p;
    players.assign(LAYERS_NUMBER,p);
    
    string compFragment = STRINGIFY(
                                \n#version 150\n
                                uniform sampler2D tex0;
                                uniform sampler2D tex1;
                                uniform sampler2D tex2;
                                uniform sampler2D tex3;
                                uniform sampler2D tex4;
                                
                                in vec2 texCoordVarying;
                                out vec4 fragColor;
                                
                                void main(void) {
                                    vec3 col0 = texture(tex0,texCoordVarying).rgb;
                                    vec3 col1 = texture(tex1,texCoordVarying).rgb;
                                    vec3 col2 = texture(tex2,texCoordVarying).rgb;
                                    vec3 col3 = texture(tex3,texCoordVarying).rgb;
                                    vec3 col4 = texture(tex4,texCoordVarying).rgb;
                                    
                                    vec3 color = 1-(1-col0)*(1-col1)*(1-col2)*(1-col3)*(1-col4);
                                    fragColor = vec4(color,1.0);
                                }
                                
                                );
    
    
    
    compShader.setupShaderFromSource(GL_VERTEX_SHADER, getSimpleVertex());
    compShader.setupShaderFromSource(GL_FRAGMENT_SHADER, compFragment);
    compShader.bindDefaults();
    compShader.linkProgram();
    
    compShader.begin();
    
    for (int i=0; i<layers.size();i++) {
        compShader.setUniformTexture("tex"+ofToString(i+1), layers[i].fbo.getTextureReference(), i+3);
    }
    compShader.end();
    frameNum = 0;
    
    ofSetColor(255);
    
    state = STATE_BACKGROUND;
    
    
   
    bFirstIdle = false;
}

void ofApp::updateLayer(layer &l,ofFbo &depth,float decay) {
    ping.begin();
    strobeShader.begin();
    strobeShader.setUniformTexture("memTex", l.fbo.getTextureReference(), 2);
    strobeShader.setUniform1f("decay", decay);
    strobeShader.setUniform1i("frameNum", frameNum);
    strobeShader.setUniform1i("strobeRate", strobeRate);
    strobeShader.setUniform1f("hue", l.hueOffset+ofGetElapsedTimef()*hueRate/100);
    strobeShader.setUniform1f("sat", sat);
    strobeShader.setUniform1f("offset", offset);
    depth.draw(0,0);
    strobeShader.end();
    ping.end();
    
    if (frameNum%(strobeRate)==0) {
        
        pong.begin();
        blurShader.begin();
        blurShader.setUniform2f("dir", 1.0/depth.getWidth(), 0);
        ping.draw(0,0);
        blurShader.end();
        pong.end();
        
        l.fbo.begin();
        blurShader.begin();
        blurShader.setUniform2f("dir", 0, 1.0/depth.getHeight());
        pong.draw(0,0);
        blurShader.end();
        l.fbo.end();
    } else {
        l.fbo.begin();
        ping.draw(0,0);
        l.fbo.end();
    }
}

void ofApp::renderDepth() {
    
    int width = depthTex0.getWidth();
    int height = depthTex0.getHeight();
    
    depthShader.begin();
    
    depthShader.setUniform1f("minEdge", minEdge0);
    depthShader.setUniform1f("maxEdge",maxEdge0);
    ofPushMatrix();
    ofTranslate(3*height/2, width/2);
    ofRotate(90);
    ofTranslate(-width/2, -height/2);
    depthTex0.draw(0,0);
    ofPopMatrix();
    
    depthShader.setUniform1f("minEdge", minEdge1);
    depthShader.setUniform1f("maxEdge",maxEdge1);
    ofPushMatrix();
    ofTranslate(height/2, width/2);
    ofRotate(90);
    ofTranslate(-width/2, -height/2);
    depthTex1.draw(0,0);
    ofPopMatrix();
    
    depthShader.end();
    
    /*ofPushMatrix();
    ofTranslate(3*height/2, width/2);
    ofRotate(90);
    ofTranslate(-width/2, -height/2);
    depthTex0.draw(0,0);
    ofPopMatrix();
    ofPushMatrix();
    ofTranslate(height/2, width/2);
    ofRotate(90);
    ofTranslate(-width/2, -height/2);
    depthTex1.draw(0,0);
    ofPopMatrix();
     */
}

//--------------------------------------------------------------
void ofApp::update(){
    
    fps = ofToString(ofGetFrameRate());
    int width = depthTex0.getWidth();
    int height = depthTex0.getHeight();
    
    cam0.update();
    cam1.update();
    if (cam0.bNewDepth && cam1.bNewDepth) {
        depthTex0.loadData(cam0.getDepth(), cam0.depthWidth, cam0.depthHeight,GL_RED);
        depthTex1.loadData(cam1.getDepth(), cam1.depthWidth, cam1.depthHeight,GL_RED);
        
        depthFbo.begin();
        renderDepth();
        depthFbo.end();
        
        blobFbo.begin();
        subtractShader.begin();
        subtractShader.setUniform1f("tolerance", tolerance);
        depthFbo.draw(0, 0);
        subtractShader.end();
        blobFbo.end();
        
        ofPixels blobPixels;
        blobFbo.readToPixels(blobPixels);
        grayImg.setFromPixels(blobPixels);
        
        float size = grayImg.getWidth()*grayImg.getHeight();
        contour.findContours(grayImg, minArea*size, maxArea*size, 10, false);
        
        blobDetection = contour.nBlobs && contour.blobs[0].area>=minArea;
        
        if (blobDetection) {
            waitTimer = ofGetElapsedTimef()+ waitDuration;
        }
        
        recordTime ="";
        waitTime="";

        
        
        
        
        
        switch (state) {
            case STATE_IDLE:
                
                updateLayer(camLayer,recordFbo,decay1);
                
                if (bFirstIdle && ofGetElapsedTimef()>idleTimer) {
                    bFirstIdle = false;
                }
                if (blobDetection && !bFirstIdle) {
                    if (!recorder.isInitialized()) {
                        for (vector<ofVideoPlayer>::iterator iter=players.begin();iter!=players.end();iter++) {
                            iter->setPaused(true);
                        }
                        
                        filename = "testMovie"+ofGetTimestampString()+".mov";
                        recorder.setup(filename, depthFbo.getWidth(), depthFbo.getHeight(), 30);
                        
                        recordTimer = ofGetElapsedTimef()+recordDuration;
                        
                        state = STATE_RECORD;
                        
                    }
                }
                break;

                
            case STATE_RECORD: {
                
                recordTime = ofToString(recordTimer-ofGetElapsedTimef());
                waitTime = ofToString(waitTimer-ofGetElapsedTimef());
                
                
                if (recordTimer-ofGetElapsedTimef()>freezeDuration) {
                    recordFbo.begin();
                    depthFbo.draw(0,0);
                    recordFbo.end();
                }
                
                updateLayer(camLayer,recordFbo,decay0);
                
                ofPixels pixels;
                recordFbo.readToPixels(pixels);
                
                recorder.addFrame(pixels);
                
                videoQueue = ofToString(recorder.getVideoQueueSize());
                if (ofGetElapsedTimef()>recordTimer || ofGetElapsedTimef()>waitTimer) {
                    recorder.close();
                    
                    state = STATE_IDLE;
                    
                    recordFbo.begin();
                    ofClear(0);
                    recordFbo.end();
                    
                    float duration = ofGetElapsedTimef()-(recordTimer-recordDuration);
                    if (duration > minimumDuration) {
                    
                        memories.push_back(filename);
                        
                        players.front().loadMovie(filename);
                        players.front().setLoopState(OF_LOOP_NONE);
                        players.front().play();
                        
                        idleTimer = ofGetElapsedTimef()+duration;
                        bFirstIdle = true;
                    } else {
                        idleTimer = ofGetElapsedTimef();
                    }
                    
                    
                    
                    
                }
            }
                break;
                
            default:
                break;
        }
        
        
        frameNum++;
        
    }
    
    
    
    if (state==STATE_IDLE) {
        
        
        
        if (!memories.empty()) {
            for (vector<ofVideoPlayer>::iterator iter=players.begin();iter!=players.end();iter++) {
                if (!iter->isLoaded()) {
                    iter->loadMovie(memories[(int)ofRandom(10000) % memories.size()]);
                    iter->setLoopState(OF_LOOP_NONE);
                    iter->play();
                    iter->setPaused(true);
                    layers[distance(players.begin(), iter)].hueOffset = ofRandomuf();
                }
            }
            
        }
        
        if (ofGetElapsedTimef()>idleTimer) {
            idleTimer = ofGetElapsedTimef() + idleInterval;
            for (vector<ofVideoPlayer>::iterator iter=players.begin();iter!=players.end();iter++) {
                if (iter->isLoaded() && iter->isPaused()) {
                    iter->setPaused(false);
                    break;
                }
            }
            
        }
       
        
        for (int i=0;i<LAYERS_NUMBER;i++) {
            
            players[i].update();
            
            
            if (players[i].getIsMovieDone()) {
                players[i].close();
                
            }
            
        }
       
    }
    
    if (state==STATE_IDLE || state == STATE_RECORD) {
        for (int i=0;i<LAYERS_NUMBER;i++) {
            
            if (players[i].isLoaded() && !players[i].isPaused()) {
                depthFbo.begin();
                players[i].draw(0, 0);
                depthFbo.end();
                updateLayer(layers[i], depthFbo,decay0);
                
            } else {
                depthFbo.begin();
                ofClear(0);
                depthFbo.end();
                updateLayer(layers[i], depthFbo,decay1);
            }
            
            
            
        }
        
        compFbo.begin();
        compShader.begin();
        for (int i=0; i<layers.size();i++) {
            compShader.setUniformTexture("tex"+ofToString(i+1), layers[i].fbo.getTextureReference(), i+3);
        }
        camLayer.fbo.draw(0, 0);
        compShader.end();
        compFbo.end();
    }
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(0);
    
    ofPushMatrix();
    ofMultMatrix(mat);
    
    switch (state) {
        case STATE_BACKGROUND:
            
            
            blobFbo.draw(0, 0);
            for (int i = 0; i < contour.nBlobs; i++){
                contour.blobs[i].draw(0,0);
            }
            ofSetColor(255);
           break;
        case STATE_CALIB:
            depthFbo.draw(0, 0);
            for (int i = 0; i < contour.nBlobs; i++){
                contour.blobs[i].draw(0,0);
            }
            ofSetColor(255);
            break;
            
        default:
            compFbo.draw(0, 0);
            break;
    }
    
    
    
    ofPopMatrix();
    
    gui.draw();
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    switch (key) {
        case 'c':
            state = STATE_CALIB;
            
            break;
        case 'b':
            state = STATE_BACKGROUND;
            break;
        
            
        case 'i':
            state = STATE_IDLE;
            
            break;
            
        case 't':
            ofToggleFullscreen();
            break;
            
        case 'u':
            createBlurShader(blurShader, radius, variance);
            break;
            
        case 'g':
            
            backgroundFbo.begin();
            renderDepth();
            backgroundFbo.end();
            break;
        case 'p':
            recSound.play();
            break;
        case 's':
            fadeTime = ofGetElapsedTimef();
            
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

}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){

}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    float scale = MIN((float)w/STAGE_WIDTH,(float)h/STAGE_HEIGHT);
    mat.makeIdentityMatrix();
    mat.scale(scale, scale, 1.0);
    mat.translate(0.5*(ofVec2f(w,h)-scale*ofVec2f(STAGE_WIDTH,STAGE_HEIGHT)));
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
