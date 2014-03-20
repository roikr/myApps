#include "ofApp.h"

#define CAM_WIDTH 640
#define CAM_HEIGHT 480

#define STRINGIFY(A) #A

//--------------------------------------------------------------
void ofApp::setup(){
//    const char* version = (const char*)glGetString(GL_VERSION);
//    cout << version << endl;
//    shader.load("shaders/shader.vert", "shaders/shader.frag");
//    ofEnableAlphaBlending();
    ofDisableArbTex();
    
    string vertex = STRINGIFY(
                              \n#version 150\n
                              uniform mat4 modelViewProjectionMatrix;
                              
                              in vec4  position;
                              in vec2  texcoord;
                              
                              out vec2 texCoordVarying;
                              
                              void main()
                              {
                                  texCoordVarying = texcoord;
                                  gl_Position = modelViewProjectionMatrix * position;
                              }
                              );
    
    string fragment = STRINGIFY(
                                \n#version 150\n
                                
                                uniform sampler2D tex0;
                                //uniform sampler2D maskTex;
                                uniform float minEdge;
                                uniform float maxEdge;
                                
                                
                                in vec2 texCoordVarying;
                                out vec4 fragColor;
                                
                                
                                void main(void)
                                {
                                    
                                    float dist = texture(tex0, texCoordVarying).r;
                                    if (dist>0) {
                                        vec3 color = vec3(1.0-(dist-minEdge)/(maxEdge-minEdge));
                                        fragColor = vec4(color,1.0);
                                    } else {
                                        discard;
                                    }
                                    
                                   
                                    
                                    
                                    //float mask = step(minEdge,color)-step(maxEdge,color);
                                    //fragColor = vec4( vec3(1.0-c) , mask);
                                    //fragColor =  vec4(vec3(texture(tex0,pos).r),1.0);
                                    
                                }
                                );
    
    shader.setupShaderFromSource(GL_VERTEX_SHADER, vertex);
    shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragment);

 
//    shader.setupShaderFromFile(GL_FRAGMENT_SHADER, "shaders/shader.frag");
    shader.bindDefaults();
    shader.linkProgram();
    minEdge = 0.010625;
    maxEdge = 0.014688;
    shader.begin();
    shader.setUniform1f("minEdge",minEdge);
    shader.setUniform1f("maxEdge",maxEdge);
    shader.end();
    
    
    cam.setup();
    cam.listDepthModes();
    cam.setDepthMode(4);
    
    fbo.allocate(cam.depthWidth, cam.depthHeight);
    ping.allocate(cam.depthWidth, cam.depthHeight);
    pong.allocate(cam.depthWidth, cam.depthHeight);
    
    blur = 3;
    
    string blurX = STRINGIFY(
                             \n#version 150\n
                             
                             uniform sampler2D tex0;
                             uniform sampler2D distTex;
                             uniform float blur;
                             
                             in vec2 texCoordVarying;
                             out vec4 fragColor;
                             
                             
                             
                             void main(void)
                             {
                                 
                                 float blurAmnt =blur*(1.0-texture(distTex,texCoordVarying).r)/textureSize(tex0,0).x;
                                 
                                 vec4 color=vec4(0);
                                 
                                 color += 1.0 * texture(tex0, texCoordVarying + vec2(blurAmnt * -4.0, 0.0));
                                 color += 2.0 * texture(tex0, texCoordVarying + vec2(blurAmnt * -3.0, 0.0));
                                 color += 3.0 * texture(tex0, texCoordVarying + vec2(blurAmnt * -2.0, 0.0));
                                 color += 4.0 * texture(tex0, texCoordVarying + vec2(blurAmnt * -1.0, 0.0));
                                 
                                 color += 5.0 * texture(tex0, texCoordVarying + vec2(blurAmnt, 0));
                                 
                                 color += 4.0 * texture(tex0, texCoordVarying + vec2(blurAmnt * 1.0, 0.0));
                                 color += 3.0 * texture(tex0, texCoordVarying + vec2(blurAmnt * 2.0, 0.0));
                                 color += 2.0 * texture(tex0, texCoordVarying + vec2(blurAmnt * 3.0, 0.0));
                                 color += 1.0 * texture(tex0, texCoordVarying + vec2(blurAmnt * 4.0, 0.0));
                                 
                                 color /= 25.0;
                                 
                                 fragColor = color;//texture(tex0, texCoordVarying);
                                 
                             }
                             );
    
    shaderBlurX.setupShaderFromSource(GL_VERTEX_SHADER, vertex);
    shaderBlurX.setupShaderFromSource(GL_FRAGMENT_SHADER, blurX);
    shaderBlurX.bindDefaults();
    shaderBlurX.linkProgram();
    shaderBlurX.begin();
    shaderBlurX.setUniformTexture("distTex", fbo.getTextureReference(), 1);
    shaderBlurX.setUniform1f("blur", blur);
    shaderBlurX.end();
    
    
    string blurY = STRINGIFY(
                             \n#version 150\n
                             
                             uniform sampler2D tex0;
                             uniform sampler2D distTex;
                             uniform float blur;
                             
                             in vec2 texCoordVarying;
                             out vec4 fragColor;
                             
                             
                             
                             void main(void)
                             {
                                 
                                 float blurAmnt =blur*(1.0-texture(distTex,texCoordVarying).r)/textureSize(tex0,0).y;
                                 
                                 vec4 color=vec4(0);
                                 
                                 color += 1.0 * texture(tex0, texCoordVarying + vec2(0.0,blurAmnt * -4.0));
                                 color += 2.0 * texture(tex0, texCoordVarying + vec2(0.0,blurAmnt * -3.0));
                                 color += 3.0 * texture(tex0, texCoordVarying + vec2(0.0,blurAmnt * -2.0));
                                 color += 4.0 * texture(tex0, texCoordVarying + vec2(0.0,blurAmnt * -1.0));
                                 
                                 color += 5.0 * texture(tex0, texCoordVarying + vec2(0.0,blurAmnt));
                                 
                                 color += 4.0 * texture(tex0, texCoordVarying + vec2(0.0,blurAmnt * 1.0));
                                 color += 3.0 * texture(tex0, texCoordVarying + vec2(0.0,blurAmnt * 2.0));
                                 color += 2.0 * texture(tex0, texCoordVarying + vec2(0.0,blurAmnt * 3.0));
                                 color += 1.0 * texture(tex0, texCoordVarying + vec2(0.0,blurAmnt * 4.0));
                                 
                                 color /= 25.0;
                                 
                                 fragColor = color;//texture(tex0, texCoordVarying);
                                 
                             }
                             );
    
    shaderBlurY.setupShaderFromSource(GL_VERTEX_SHADER, vertex);
    shaderBlurY.setupShaderFromSource(GL_FRAGMENT_SHADER, blurY);
    shaderBlurY.bindDefaults();
    shaderBlurY.linkProgram();
    shaderBlurY.begin();
    shaderBlurY.setUniformTexture("distTex", fbo.getTextureReference(), 1);
    shaderBlurY.setUniform1f("blur", blur);
    shaderBlurY.end();
    
    
    string coolFrag = STRINGIFY(
                             \n#version 150\n
                             
                             uniform sampler2D tex0;
//                             uniform sampler2D distTex;
                            
                             
                             in vec2 texCoordVarying;
                             out vec4 fragColor;
                             
                            
                            vec3 hsv2rgb(vec3 c)
                            {
                                vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
                                vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
                                return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
                            }
                             
                             void main(void)
                             {
                                 
                                 vec3 c;
                                 c.x = 0.3;
                                 c.y = 1-texture(tex0,texCoordVarying).r;
                                 c.z = texture(tex0,texCoordVarying).r;
//                                 float c = blur*(1.0-)/textureSize(tex0,0).y;;
                                 
                                 fragColor = vec4(hsv2rgb(c),1.0);//texture(tex0, texCoordVarying);
                                 
                             }
                             );
    
    shaderCool.setupShaderFromSource(GL_VERTEX_SHADER, vertex);
    shaderCool.setupShaderFromSource(GL_FRAGMENT_SHADER, coolFrag);
    shaderCool.bindDefaults();
    shaderCool.linkProgram();
//    shaderCool.begin();
//    shaderCool.setUniformTexture("distTex", fbo.getTextureReference(), 1);
//    shaderCool.setUniform1f("blur", blur);
//    shaderCool.end();

    
    
    fileName = "testMovie";
    fileExt = ".mov";
    
    recorder.setPixelFormat("gray");
    bRecording = false;

    windowResized(ofGetWidth(), ofGetHeight());
    ofSetWindowPosition(1680, 100);
}


//--------------------------------------------------------------
void ofApp::update(){
    cam.update();
    
    if(cam.bNewDepth){
        ofSetColor(255);
        fbo.begin();
        ofClear(0);
        shader.begin();
        cam.depthTexture.draw(ofPoint());
        shader.end();
        
        fbo.end();
        
        
        ping.begin();
        ofClear(0);
        shaderCool.begin();
        fbo.draw(ofPoint());
        shaderCool.end();
        ping.end();
        
        pong.begin();
        ofClear(0);
        shaderBlurY.begin();
        ping.draw(ofPoint());
        shaderBlurY.end();
        pong.end();
        
        ping.begin();
        ofClear(0);
        shaderBlurX.begin();
        pong.draw(ofPoint());
        shaderBlurX.end();
        ping.end();
        
        if (bRecording) {
            ofPixels pixels;
            fbo.readToPixels(pixels);
            pixels.setImageType(OF_IMAGE_GRAYSCALE);
            recorder.addFrame(pixels);
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(0);
    ofPushMatrix();
    ofMultMatrix(mat);
    ping.draw(ofPoint());
    ofPopMatrix();
    
    stringstream ss;
    ss << fixed << "range: " << minEdge << " - " << maxEdge << endl
    << "blur: " << blur << endl
    << "video queue size: " << recorder.getVideoQueueSize() << endl
    << "audio queue size: " << recorder.getAudioQueueSize() << endl
    << "FPS: " << ofGetFrameRate() << endl
    << (bRecording?"pause":"start") << " recording: r" << endl
    << (bRecording?"close current video file: c":"") << endl;
    
    ofSetColor(0,0,0,100);
    ofRect(0, 0, 260, 75);
    ofSetColor(255, 255, 255);
    ofDrawBitmapString(ss.str(),15,15);
    
    if(bRecording){
        ofSetColor(255, 0, 0);
        ofCircle(ofGetWidth() - 20, 20, 5);
    }
    
}

void ofApp::exit() {
    recorder.close();
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    switch (key) {
        case 't':
            ofToggleFullscreen();
            break;
        case 's':
            shader.setupShaderFromFile(GL_FRAGMENT_SHADER, "shaders/shader.frag");
            shader.linkProgram();
            break;
        case 'r':
            bRecording = !bRecording;
            if(bRecording && !recorder.isInitialized()) {
                recorder.setup(fileName+ofGetTimestampString()+fileExt, cam.depthWidth, cam.depthHeight, 30);
            }
            break;
        case 'c':
            bRecording = false;
            recorder.close();
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
    ofVec2f pos = imat.preMult(ofVec3f(x,y));
    ofVec2f dim = imat.preMult(ofVec3f(ofGetWidth(),ofGetHeight()));
    float diff = ((pos-downPos)/dim.x).x;
    
    shader.begin();
    switch (button) {
        case 0:
             shader.setUniform1f("minEdge",ofClamp(minEdge+0.2*diff, 0.0,maxEdge));
            break;
        case 2:
            shader.setUniform1f("maxEdge",ofClamp(maxEdge+0.2*diff, minEdge,1.0));
            break;
            
        default:
            break;
    }
    shader.end();
    
    
    if (button == 1) {
        shaderBlurX.begin();
        shaderBlurX.setUniform1f("blur", blur+diff);
        shaderBlurX.end();
        shaderBlurY.begin();
        shaderBlurY.setUniform1f("blur", blur+diff);
        shaderBlurY.end();
        
    }
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    downPos = imat.preMult(ofVec3f(x,y));
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
    ofVec2f pos = imat.preMult(ofVec3f(x,y));
    ofVec2f dim = imat.preMult(ofVec3f(ofGetWidth(),ofGetHeight()));
    float diff = ((pos-downPos)/dim.x).x;
    
    switch (button) {
        case 0:
            minEdge = ofClamp(minEdge+0.2*diff, 0.0,maxEdge);
            break;
        case 2:
            maxEdge = ofClamp(maxEdge+0.2*diff, minEdge,1.0);
            break;
            
        default:
            break;
    }
   
    if (button == 1) {
        blur+=diff;
    }

    //cout << minEdge << '\t' << maxEdge << endl;
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    float scale = MIN((float)w/(float)CAM_WIDTH,(float)h/(float)CAM_HEIGHT);
    
    mat = ofMatrix4x4::newTranslationMatrix(0.5*(ofVec2f(w,h)-scale*ofVec2f(CAM_WIDTH,CAM_HEIGHT)));
    mat.preMult(ofMatrix4x4::newScaleMatrix(scale, scale, 1.0));
    imat = mat.getInverse();
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
