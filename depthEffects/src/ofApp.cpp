#include "ofApp.h"

#define CAM_WIDTH 640
#define CAM_HEIGHT 480

#define STRINGIFY(A) #A

#define OFFLINE 1

//--------------------------------------------------------------
void ofApp::setup(){
//    const char* version = (const char*)glGetString(GL_VERSION);
//    cout << version << endl;
    ofDisableArbTex();
    
#ifdef OFFLINE
    memory1.loadMovie("maya.mov");
    memory1.setLoopState(OF_LOOP_NORMAL);
    memory1.play();
    memory2.loadMovie("yoav.mov");
    memory2.setLoopState(OF_LOOP_NORMAL);
    memory2.play();
    
    
    ofFbo::Settings s;
    s.width = memory1.getWidth();
    s.height = memory1.getHeight();
    s.internalformat = GL_R16; // GL_R8 is enough but GL_R8 is not supported in ofGetImageTypeFromGLType()
    fbo.allocate(s);
    
    
#else
    cam.setup();
    cam.listDepthModes();
    cam.setDepthMode(9);
    fbo.allocate(cam.depthWidth, cam.depthHeight);
    depthTexture.allocate(cam.depthWidth, cam.depthHeight, GL_R16);//, GL_RED, OF_PIXELS_MONO);
    
#endif
    
    
    
    ofSetWindowShape(fbo.getWidth(), fbo.getHeight());
//    ofSetWindowPosition(1680, 100);
    
    
    flowSolver.setup(fbo.getWidth(), fbo.getHeight(), 0.35, 5, 10, 1, 3, 2.25, false, false);
    
    
    fbo.begin();
    ofClear(0);
    fbo.end();
    
    
    ping.allocate(fbo.getWidth(),fbo.getHeight());
    pong.allocate(fbo.getWidth(),fbo.getHeight());
    echo1.allocate(fbo.getWidth(),fbo.getHeight());
    echo2.allocate(fbo.getWidth(),fbo.getHeight());

    
    echo1.begin();
    ofClear(0,0,0,255);
    echo1.end();
    
    echo2.begin();
    ofClear(0,0,0,255);
    echo2.end();
    
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
    
    
    
    string blurX = STRINGIFY(
                             \n#version 150\n
                             
                             uniform sampler2D tex0;
                             uniform sampler2D distTex;
                             
                             uniform float x0;
                             uniform float y0;
                             uniform float x1;
                             uniform float y1;
                             
                             
                             in vec2 texCoordVarying;
                             out vec4 fragColor;
                             
                             
                             
                             void main(void)
                             {
                                 
                                 float dist = 1.0-texture(distTex,texCoordVarying).r;
                                 float blurAmnt = clamp(y0+(dist-x0)*(y1-y0)/(x1-x0),y0,y1)/textureSize(tex0,0).x;
                                 //float blurAmnt =step(blurOffset,dist)*(dist-blurOffset)*blur/textureSize(tex0,0).x;
                                 
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
    shaderBlurX.end();
    
    
    string blurY = STRINGIFY(
                             \n#version 150\n
                             
                             uniform sampler2D tex0;
                             uniform sampler2D distTex;
                             uniform float x0;
                             uniform float y0;
                             uniform float x1;
                             uniform float y1;
                             
                             in vec2 texCoordVarying;
                             out vec4 fragColor;
                             
                             
                             
                             void main(void)
                             {
                                 
                                 float dist = 1.0-texture(distTex,texCoordVarying).r;
//                                 float blurAmnt =step(blurOffset,dist)*(dist-blurOffset)*blur/textureSize(tex0,0).x;
                                 float blurAmnt = clamp(y0+(dist-x0)*(y1-y0)/(x1-x0),y0,y1)/textureSize(tex0,0).y;
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
    shaderBlurY.end();
    
    
    string coolFrag = STRINGIFY(
                             \n#version 150\n
                             
                             uniform sampler2D tex0;
//                             uniform sampler2D distTex;
                                uniform float hue;
                                uniform float satOffset;
                             
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
                                 float sat = 1-texture(tex0,texCoordVarying).r;
                                 c.x = hue;
                                 c.y = step(satOffset,sat)*(sat-satOffset);
                                 c.z = texture(tex0,texCoordVarying).r;
//                                 float c = blur*(1.0-)/textureSize(tex0,0).y;;
                                 
                                 fragColor = vec4(hsv2rgb(c),1.0);//texture(tex0, texCoordVarying);
                                 
                             }
                             );
    
    shaderCool.setupShaderFromSource(GL_VERTEX_SHADER, vertex);
    shaderCool.setupShaderFromSource(GL_FRAGMENT_SHADER, coolFrag);
    shaderCool.bindDefaults();
    shaderCool.linkProgram();


    string echoFrag = STRINGIFY(
                                \n#version 150\n
                                
                                uniform sampler2D tex0;
                                uniform sampler2D tex1;
                                uniform sampler2D distTex;
                                uniform float opacityIn;
                                uniform float feedback;
                                uniform float feedbackOffset;
                                
                                in vec2 texCoordVarying;
                                out vec4 fragColor;
                                
                                
                                void main(void)
                                {
                                    
                                    vec4 c0=texture(tex0, texCoordVarying);
                                    vec4 c1=texture(tex1, texCoordVarying);
                                    
                                    float dist = 0.5;//1.0-texture(distTex,texCoordVarying).r;
                                    float feedbackAmnt = feedback ;//* step(feedbackOffset,dist);
                                    vec3 c;
                                    
                                    for (int i=0;i<3;i++) {
                                        c[i]=max(opacityIn*c0[i],feedbackAmnt*c1[i]);
                                    }
                                    fragColor = vec4(c,1.0);
                                    
                                    
                                }
                                );
    
   
    
    shaderEcho.setupShaderFromSource(GL_VERTEX_SHADER, vertex);
    shaderEcho.setupShaderFromSource(GL_FRAGMENT_SHADER, echoFrag);
    shaderEcho.bindDefaults();
    shaderEcho.linkProgram();
    shaderEcho.begin();
    shaderEcho.setUniformTexture("distTex", fbo.getTextureReference(), 2);
    shaderEcho.end();
    
    
    string blendFrag = STRINGIFY(
                                 \n#version 150\n
                                 
                                 uniform sampler2D tex0;
                                 uniform sampler2D tex1;
                                 
                                 
                                 
                                 in vec2 texCoordVarying;
                                 out vec4 fragColor;
                                 
                                 vec3 hsv2rgb(vec3 c)
                                 {
                                     vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
                                     vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
                                     return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
                                 }
                                 
                                 vec3 rgb2hsv(vec3 c)
                                 {
                                     vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
                                     vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
                                     vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));
                                     
                                     float d = q.x - min(q.w, q.y);
                                     float e = 1.0e-10;
                                     return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
                                 }
                                 
                                 void main(void)
                                 {
//
                                     vec4 rgb0= texture(tex0, texCoordVarying);
                                     vec4 rgb1= texture(tex1, texCoordVarying);
//                                     vec3 hsv0 = rgb2hsv(rgb0.rgb);
//                                     vec3 hsv1 = rgb2hsv(rgb1.rgb);
//                                     fragColor = mix(rgb0,rgb1,int(hsv1.z>hsv0.z));
                                     fragColor = max(rgb0,rgb1);
//                                     fragColor =texture(tex0, texCoordVarying)+texture(tex1, texCoordVarying);
                                     
                                 }
                                 );
    
    blend.setupShaderFromSource(GL_VERTEX_SHADER, vertex);
    blend.setupShaderFromSource(GL_FRAGMENT_SHADER, blendFrag);
    
    
    //    shader.setupShaderFromFile(GL_FRAGMENT_SHADER, "shaders/shader.frag");
    blend.bindDefaults();
    blend.linkProgram();
    
    
    fileName = "testMovie";
    fileExt = ".mov";
    
//    recorder.setPixelFormat("gray");
    bRecording = false;
    
    
    bHide = false;
    gui.setup("panel");
    gui.add(fps.set("fps",""));
    gui.add(minEdge.set("minEdge",0.010625,0.3,0.4));
    gui.add(maxEdge.set("maxEdge",0.014688,0.4,0.5));
    gui.add(b0.set("b0", ofVec2f(0,0), ofVec2f(0,0), ofVec2f(1,10)));
    gui.add(b1.set("b1", ofVec2f(1,10), ofVec2f(0,0), ofVec2f(1,10)));
    gui.add(hue1.set("hue1",0.5,0,1));
    gui.add(hue2.set("hue2",0.5,0,1));
    gui.add(satOffset.set("satOffset",0,0,1));
    gui.add(opacityIn.set("opacityIn",1.0,0,1));
    gui.add(feedback.set("feedback", 0.5, 0.5, 1));
    gui.add(feedbackOffset.set("feedbackOffset", 0, 0, 1));
    gui.loadFromFile("settings.xml");
    
    ofSetLineWidth(1.5);
}


//--------------------------------------------------------------
void ofApp::update(){
    
    fps = ofToString(ofGetFrameRate(),0);
    
#ifdef OFFLINE
    memory1.update();
    if (memory1.isFrameNew()) {
        fbo.begin();
        ofPushMatrix();
//        ofScale(0.5, 0.5);
//        ofTranslate(video.getHeight(),0);
//        ofRotate(90);
        //        ofTranslate(0, -video.getHeight());
        memory1.draw(0, 0);
        ofPopMatrix();
        fbo.end();
        
        
        

    
#else
    
    cam.update();
    if(cam.bNewDepth){
        ofSetColor(255);
        depthTexture.loadData(cam.getDepth(), cam.depthWidth, cam.depthHeight, GL_RED);
        fbo.begin();
        ofClear(0);
        shader.begin();
        shader.setUniform1f("minEdge",minEdge);
        shader.setUniform1f("maxEdge",maxEdge);
        depthTexture.draw(0,0);
        shader.end();
        fbo.end();
    }
#endif
    
        
        ping.begin();
        ofClear(0);
        shaderCool.begin();
        shaderCool.setUniform1f("hue", hue1);
        shaderCool.setUniform1f("satOffset", satOffset);
        fbo.draw(ofPoint());
        shaderCool.end();
        ping.end();
        
        
        ofPixels pixels;
        fbo.readToPixels(pixels);
        flowSolver.update(pixels.getPixels(),pixels.getWidth(),pixels.getHeight(),OF_IMAGE_GRAYSCALE);
        
        pong.begin();
        ofClear(0);
        
        
        ping.draw(0, 0);
        
        ofColor c;
        c.setHsb(hue1*255, 100, 255);
        
        ofSetColor(c);
        ofPoint vel;
        int res = 10;
        float lineScale = 4;
        for(int x=0; x<fbo.getWidth(); x+=res) {
            for(int y=0; y<fbo.getHeight(); y+=res) {
                
                ofVec2f vel (flowSolver.getVelAtPixel(x, y));
                
                if(vel.length() < 1) {  // smaller then 1 pixel, no point drawing.
                    continue;
                }
                
                ofLine(x, y, x - vel.x * lineScale, y - vel.y * lineScale);
            }
        }
        
//        flowSolver.draw(pixels.getWidth(),pixels.getHeight(),4,10);
        //        pong.draw(0, 0);
        
        pong.end();
        
        
         ofSetColor(255);
        
        ping.begin();
        ofClear(0);
        shaderBlurX.begin();
        shaderBlurX.setUniform1f("x0", b0->x);
        shaderBlurX.setUniform1f("y0", b0->y);
        shaderBlurX.setUniform1f("x1", b1->x);
        shaderBlurX.setUniform1f("y1", b1->y);
        pong.draw(ofPoint());
        shaderBlurX.end();
        ping.end();
        
        
        pong.begin();
        ofClear(0);
        shaderBlurY.begin();
        shaderBlurY.setUniform1f("x0", b0->x);
        shaderBlurY.setUniform1f("y0", b0->y);
        shaderBlurY.setUniform1f("x1", b1->x);
        shaderBlurY.setUniform1f("y1", b1->y);
        ping.draw(ofPoint());
        shaderBlurY.end();
        pong.end();
        
    
        
        
//        ping.begin();
//        shaderEcho.begin();
//        shaderEcho.setUniformTexture("tex1", echo1.getTextureReference(), 1);
//        shaderEcho.setUniform1f("opacityIn", opacityIn);
//        shaderEcho.setUniform1f("feedback", feedback);
//        shaderEcho.setUniform1f("feedbackOffset", feedbackOffset);
//        pong.draw(0, 0);
//        shaderEcho.end();
//        ping.end();
      
        
        echo1.begin();
        pong.draw(0, 0);
        echo1.end();
        
    }
    
    
    
    
    memory2.update();
    if (memory2.isFrameNew()) {
        fbo.begin();
        ofPushMatrix();
        //ofScale(0.5, 0.5);
        //        ofTranslate(video.getHeight(),0);
        //        ofRotate(90);
        //        ofTranslate(0, -video.getHeight());
        memory2.draw(0, 0);
        ofPopMatrix();
        fbo.end();
        
        
        ping.begin();
        ofClear(0);
        shaderCool.begin();
        shaderCool.setUniform1f("hue", hue2);
        shaderCool.setUniform1f("satOffset", satOffset);
        fbo.draw(ofPoint());
        shaderCool.end();
        ping.end();
        
        ping.begin();
        ofClear(0);
        shaderBlurX.begin();
        shaderBlurX.setUniform1f("x0", b0->x);
        shaderBlurX.setUniform1f("y0", b0->y);
        shaderBlurX.setUniform1f("x1", b1->x);
        shaderBlurX.setUniform1f("y1", b1->y);
        pong.draw(ofPoint());
        shaderBlurX.end();
        ping.end();
        
        
        pong.begin();
        ofClear(0);
        shaderBlurY.begin();
        shaderBlurY.setUniform1f("x0", b0->x);
        shaderBlurY.setUniform1f("y0", b0->y);
        shaderBlurY.setUniform1f("x1", b1->x);
        shaderBlurY.setUniform1f("y1", b1->y);
        ping.draw(ofPoint());
        shaderBlurY.end();
        pong.end();
        
        pong.begin();
        shaderEcho.begin();
        shaderEcho.setUniformTexture("tex1", echo2.getTextureReference(), 1);
        shaderEcho.setUniform1f("opacityIn", opacityIn);
        shaderEcho.setUniform1f("feedback", feedback);
        shaderEcho.setUniform1f("feedbackOffset", feedbackOffset);
        ping.draw(0, 0);
        shaderEcho.end();
        pong.end();
        
        echo2.begin();
        pong.draw(0, 0);
        echo2.end();
    }
    
    
    if (bRecording) {
        ofPixels pixels;
        echo1.readToPixels(pixels);
        pixels.setImageType(OF_IMAGE_COLOR);
        recorder.addFrame(pixels);
    }
    
    
    
    
    
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(0);
    ofSetColor(255);
    

    ofPushMatrix();
    ofMultMatrix(mat);
//    blend.begin();
//    blend.setUniformTexture("tex1", echo2.getTextureReference(), 1);
    echo1.draw(ofPoint());
//    blend.end();
    ofPopMatrix();
    
    if (!bHide) {
        gui.draw();
    }
    
    stringstream ss;
//    ss << fixed << "range: " << minEdge << " - " << maxEdge << endl
//    << "blur: " << blur << endl
 ss   << "video queue size: " << recorder.getVideoQueueSize() << endl
    << "audio queue size: " << recorder.getAudioQueueSize() << endl
    << "FPS: " << ofGetFrameRate() << endl
    << (bRecording?"pause":"start") << " recording: r" << endl
    << (bRecording?"close current video file: c":"") << endl;
//
//    ofSetColor(0,0,0,100);
//    ofRect(0, 0, 260, 75);
//    ofSetColor(255, 255, 255);
//    ofDrawBitmapString(ss.str(),15,15);
//    
//    if(bRecording){
//        ofSetColor(255, 0, 0);
//        ofCircle(ofGetWidth() - 20, 20, 5);
//    }
    
}

void ofApp::exit() {
    recorder.close();
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key){
    switch (key) {
        case 'h':
            bHide = !bHide;
            break;
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
                recorder.setup(fileName+ofGetTimestampString()+fileExt, echo1.getWidth(), echo1.getHeight(), 30);
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
//    ofVec2f pos = imat.preMult(ofVec3f(x,y));
//    ofVec2f dim = imat.preMult(ofVec3f(ofGetWidth(),ofGetHeight()));
//    float diff = ((pos-downPos)/dim.x).x;
//    
//    shader.begin();
//    switch (button) {
//        case 0:
//             shader.setUniform1f("minEdge",ofClamp(minEdge+0.2*diff, 0.0,maxEdge));
//            break;
//        case 2:
//            shader.setUniform1f("maxEdge",ofClamp(maxEdge+0.2*diff, minEdge,1.0));
//            break;
//            
//        default:
//            break;
//    }
//    shader.end();
//    
//    
//    if (button == 1) {
//        shaderBlurX.begin();
//        shaderBlurX.setUniform1f("blur", blur+diff);
//        shaderBlurX.end();
//        shaderBlurY.begin();
//        shaderBlurY.setUniform1f("blur", blur+diff);
//        shaderBlurY.end();
//        
//    }
    
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button){
    //downPos = imat.preMult(ofVec3f(x,y));
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button){
//    ofVec2f pos = imat.preMult(ofVec3f(x,y));
//    ofVec2f dim = imat.preMult(ofVec3f(ofGetWidth(),ofGetHeight()));
//    float diff = ((pos-downPos)/dim.x).x;
//    
//    switch (button) {
//        case 0:
//            minEdge = ofClamp(minEdge+0.2*diff, 0.0,maxEdge);
//            break;
//        case 2:
//            maxEdge = ofClamp(maxEdge+0.2*diff, minEdge,1.0);
//            break;
//            
//        default:
//            break;
//    }
//   
//    if (button == 1) {
//        blur+=diff;
//    }

    //cout << minEdge << '\t' << maxEdge << endl;
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h){
    float scale = MIN((float)w/(float)fbo.getWidth(),(float)h/(float)fbo.getHeight());
    
    mat = ofMatrix4x4::newTranslationMatrix(0.5*(ofVec2f(w,h)-scale*ofVec2f(fbo.getWidth(),fbo.getHeight())));
    mat.preMult(ofMatrix4x4::newScaleMatrix(scale, scale, 1.0));
    imat = mat.getInverse();
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo){ 

}
