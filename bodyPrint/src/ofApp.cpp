#include "ofApp.h"
#include "Shaders.h"

#define STAGE_WIDTH 1024
#define STAGE_HEIGHT 768

#define LAYERS_NUMBER 3

#define MAX_POSITION 5000.0
#define MIN_POSITION -5000.0

#define STRINGIFY(A) #A

enum {
    STATE_ORIENTATION,
    STATE_CALIB,
    STATE_BACKGROUND,
    STATE_IDLE,
    STATE_RECORD,
    
};


//--------------------------------------------------------------
void ofApp::setup(){
    ofDisableSetupScreen();
//    ofHideCursor();
//    ofDirectory dir;
//    dir.allowExt("mov");
//    dir.listDir(ofToDataPath("."));
//    for (int i=0;i<dir.numFiles();i++) {
//        memories.push_back(dir.getName(i));        
//    }
    
    recSound.loadSound("camera memory sound 10 sec 48.wav");
    
    ofSetWindowShape(STAGE_WIDTH, STAGE_HEIGHT);
    
    ofDisableArbTex();
    
    for (int i=0;i<2;i++) {
        cam[i].params.setName("cam"+ofToString(i));
        cam[i].params.add(cam[i].minEdge0.set("minEdge0", 0.0, 0.0, 1.0));
        cam[i].params.add(cam[i].maxEdge0.set("maxEdge0", 1.0, 0.0, 1.0));
        cam[i].params.add(cam[i].minEdge1.set("minEdge1", 0.0, 0.0, 2.0));
        cam[i].params.add(cam[i].maxEdge1.set("maxEdge1", 2.0, 0.0, 2.0));
        cam[i].params.add(cam[i].position.set("position", ofVec3f(0), ofVec3f(MIN_POSITION), ofVec3f(MAX_POSITION)));
        cam[i].params.add(cam[i].cameraRotation.set("cameraRotation", ofVec3f(0), ofVec3f(-180), ofVec3f(180)));
        cam[i].params.add(cam[i].sceneRotation.set("sceneRotation", ofVec3f(0), ofVec3f(-180), ofVec3f(180)));
    }
    
    gui.setup("panel");
    gui.add(fps.set("fps",""));
    gui.add(pointSize.set("pointSize",3,1,10));
    gui.add(cam[0].params);
    gui.add(cam[1].params);
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
    
    cam[0].sensor.setup(devices[1]);
    cam[0].sensor.setDepthMode(5);
    cam[1].sensor.setup(devices[0]);
    cam[1].sensor.setDepthMode(5);
    
    createCloudShader(cloudShader);
    
    ofFbo::Settings s;
    s.width = STAGE_WIDTH;
    s.height = STAGE_HEIGHT;
    s.internalformat = GL_R16; // GL_R8 is not used in ofGetImageTypeFromGLType()
    
    depthFbo.allocate(s);
    recordFbo.allocate(s);
    backgroundFbo.allocate(s);
    blobFbo.allocate(s);
    
    recordFbo.begin();
    ofClear(0);
    recordFbo.end();
    

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
    
    state = STATE_ORIENTATION;
    bShowGui = true;
    
    
   
    bFirstIdle = false;
}


void ofApp::updateMesh(camera &cam) {
    
    cam.mesh.clear();
    cam.mesh.setMode(OF_PRIMITIVE_POINTS);
    
    int rows=cam.sensor.depthHeight;
    int columns=cam.sensor.depthWidth;
    
    int minE = cam.minEdge0*USHRT_MAX;
    int maxE = cam.maxEdge0*USHRT_MAX;
    
    for(int iy = 0; iy < rows; iy++) {
        for(int ix = 0; ix < columns; ix++) {
            short unsigned int depth = cam.sensor.getDepth()[iy*columns+ix];
            if (depth && depth> minE && depth<maxE) {
                cam.mesh.addVertex(cam.sensor.getWorldCoordinateAt(ix, iy, depth));
                cam.mesh.addVertex(ofVec3f(ix,iy,depth));
            }
            
            
        }
    }
    
    
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


void ofApp::renderCam(camera &cam) {
    ofPushMatrix();
    
    
    ofRotateX(cam.sceneRotation->x);
	ofRotateY(cam.sceneRotation->y);
	ofRotateZ(cam.sceneRotation->z);
	ofTranslate(cam.position->x, cam.position->y, cam.position->z);
	ofRotateX(cam.cameraRotation->x);
	ofRotateY(cam.cameraRotation->y);
	ofRotateZ(cam.cameraRotation->z);
    //    /    ofLoadMatrix(mat);
    glPointSize(pointSize);
    ofEnableDepthTest();
    ofScale(0.3,0.3, 0.3); // make y point down
    
    cam.mesh.drawVertices();
    ofPopMatrix();
    
    ofDisableDepthTest();
}

//--------------------------------------------------------------
void ofApp::update(){
    
    ofSetupScreenPerspective(1024,768,60,3,-3);
    fps = ofToString(ofGetFrameRate());
    
    
    cam[0].sensor.update();
    cam[1].sensor.update();
    
    
    if (cam[0].sensor.bNewDepth && cam[1].sensor.bNewDepth) {
       
        
        
        
        depthFbo.begin();
        ofClear(0);
        cloudShader.begin();
        for (int i=0; i<2; i++) {
            
            cloudShader.setUniform1f("minEdge", cam[i].minEdge1);
            cloudShader.setUniform1f("maxEdge",cam[i].maxEdge1);
            updateMesh(cam[i]);
            renderCam(cam[i]);
            
        }
        cloudShader.end();
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
                        recSound.play();
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
    
//    ofPushMatrix();
//    ofMultMatrix(mat);
    
    switch (state) {
        case STATE_ORIENTATION:
            for (int i=0; i<2; i++) {
                renderCam(cam[i]);
            }
            
            
            break;
        case STATE_CALIB:
            depthFbo.draw(0, 0);
            for (int i = 0; i < contour.nBlobs; i++){
                contour.blobs[i].draw(0,0);
            }
            ofSetColor(255);
            break;
        case STATE_BACKGROUND:
            
            blobFbo.draw(0, 0);
            for (int i = 0; i < contour.nBlobs; i++){
                contour.blobs[i].draw(0,0);
            }
            ofSetColor(255);
            break;
        
            
        default:
            compFbo.draw(0, 0);
            break;
    }
    
    
    
//    ofPopMatrix();
    if (bShowGui) {
        gui.draw();
    }
    
    
}

void ofApp::exit() {
    cam[0].sensor.exit();
    cam[1].sensor.exit();
    
    for (vector<ofVideoPlayer>::iterator iter=players.begin();iter!=players.end();iter++) {
        iter->close();
    }
    
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    switch (key) {
        case ' ':
            bShowGui=!bShowGui;
            break;
        case '1':
            state = STATE_ORIENTATION;
            break;
        case '2':
            state = STATE_CALIB;
            
            break;
        case '3':
            state = STATE_BACKGROUND;
            break;
        
            
        case '4':
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
            ofClear(0);
            cloudShader.begin();
            for (int i=0; i<2; i++) {
                
                cloudShader.setUniform1f("minEdge", cam[i].minEdge1);
                cloudShader.setUniform1f("maxEdge",cam[i].maxEdge1);
                renderCam(cam[i]);
                
            }
            cloudShader.end();
            backgroundFbo.end();
           
            break;
        case 'p':
            recSound.play();
            break;
        case 'c':
            ofShowCursor();
            break;
        case 'h':
            ofHideCursor();
            break;
//        case 's':
//            fadeTime = ofGetElapsedTimef();
//            
//            break;
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



/*
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
 
 }
 */
