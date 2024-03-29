#include "ofApp.h"
#include "Shaders.h"

#define STRINGIFY(A) #A

//--------------------------------------------------------------
void ofApp::setup(){
    gui.setup("panel");
    gui.add(fps.set("fps",""));
    gui.add(edge0.set("edge0", 0.0,0.0,1.0));
    gui.add(edge1.set("edge1", 0.0,0.0,1.0));
    gui.add(alpha.set("alpha", 1.0,0.0,1.0));
    gui.add(variance1.set("variance1", 1.5,0,10));
    gui.add(radius1.set("radius1", 20,0,50)); // fps drop above 14
    gui.add(variance2.set("variance2", 3,0,10));
    gui.add(radius2.set("radius2", 50,0,100)); // fps drop above 14
    gui.add(scale.set("scale", 1,1,10));
    gui.add(hue1.set("hue1", 0.0,0.0,1.0));
    gui.add(hue2.set("hue2", 0.0,0.0,1.0));
    gui.add(sat.set("sat", 0.0,0.0,1.0));
    gui.add(offset.set("offset", 0.0,-0.5,0.5));
    
   
    gui.loadFromFile("settings.xml");
    
    ofDisableArbTex();
    
    video.loadMovie("yoav.mov");
    video.setLoopState(OF_LOOP_NORMAL);
    video.play();
    
    video2.loadMovie("maya.mov");
    video2.setLoopState(OF_LOOP_NORMAL);
    video2.play();
    
    ofFbo::Settings s;
    s.width = video.getWidth();
    s.height = video.getHeight();
    s.internalformat = GL_R16; // GL_R8 is enough but GL_R8 is not supported in ofGetImageTypeFromGLType()
    depthFbo.allocate(s);
    layer1.allocate(s);
    layer2.allocate(s);
    layer3.allocate(s);
    ping.allocate(s);
    
    echo1.allocate(s);
    src1.allocate(s);
    echo2.allocate(s);
    src2.allocate(s);
    
    echo1.begin();
    ofClear(0, 0, 0,1);
    echo1.end();
    
    echo2.begin();
    ofClear(0, 0, 0,1);
    echo2.end();
    
    bShowGui = true;
    
    createColor2GrayShader(depthShader);
    createThresholdShader(threshShader);
    createEchoShader(echoShader);
    
    
    
    
    createDepthBlurShader(blur1Shader, radius1, variance1);
    createDepthBlurShader(blur2Shader, radius2, variance2);
    createScreenShader(screenShader);
    
    
    
    string fragment = STRINGIFY(
                                \n#version 150\n
                                uniform sampler2D tex0;
                                uniform sampler2D tex1;
                                uniform float hue1;
                                uniform float hue2;
                                uniform float sat;
                                uniform float offset;
                                
                                in vec2 texCoordVarying;
                                out vec4 fragColor;
                                
                                vec3 hsl2rgb(float hue,float sat,float l) {
                                    
                                    
                                    float c= (1-abs(2*l-1))*sat;
                                    
                                    vec3 y;
                                    y.r = abs(hue * 6 - 3) - 1;
                                    y.g = 2 - abs(hue * 6 - 2);
                                    y.b = 2 - abs(hue * 6 - 4);
                                    
                                    return (clamp(y,0,1)-0.5)*c+l;
                                }
                                
                                void main(void) {
                                    
                                    vec3 c0 = hsl2rgb(hue1,sat,texture(tex0,texCoordVarying).r+offset);
                                    vec3 c1 = hsl2rgb(hue2,sat,texture(tex1,texCoordVarying).r+offset);
                                    
                                    
//                                    fragColor = vec4(1-(1-c0)*(1-c1),1.0);
                                        fragColor = vec4(max(c0,c1),1);
//                                    fragColor = vec4(clamp(c0+c1,0,1),1);
                                    
                                    
                                }
                                
                                );
    
    
    
    shader.setupShaderFromSource(GL_VERTEX_SHADER, getSimpleVertex());
    shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragment);
    shader.bindDefaults();
    shader.linkProgram();

}

void ofApp::updateSource(ofFbo &src,ofFbo &echo) {
    
    
    layer1.begin();
    threshShader.begin();
    threshShader.setUniform1f("edge0", edge0);
    threshShader.setUniform1f("edge1", edge1);
    depthFbo.draw(0,0);
    threshShader.end();
    layer1.end();
    
    ping.begin();
    echoShader.begin();
    echoShader.setUniformTexture("tex0", echo.getTextureReference(), 1);
    echoShader.setUniform1f("alpha", alpha);
    depthFbo.draw(0, 0);
    echoShader.end();
    ping.end();
    
    echo.begin();
    ping.draw(0, 0);
    echo.end();
    
    
    ping.begin();
    blur1Shader.begin();
    blur1Shader.setUniform2f("dir", 1.0/video.getWidth(), 0);
    echo.draw(0,0);
    blur1Shader.end();
    ping.end();
    
    layer2.begin();
    blur1Shader.begin();
    blur1Shader.setUniform2f("dir", 0, 1.0/video.getHeight());
    ping.draw(0,0);
    blur1Shader.end();
    layer2.end();
    
    
    ping.begin();
    blur2Shader.begin();
    blur2Shader.setUniform2f("dir", scale/video.getWidth(), 0);
    depthFbo.draw(0,0);
    blur2Shader.end();
    ping.end();
    
    layer3.begin();
    blur2Shader.begin();
    blur2Shader.setUniform2f("dir", 0, scale/video.getHeight());
    ping.draw(0,0);
    blur2Shader.end();
    layer3.end();
    
    ping.begin();
    blur2Shader.begin();
    blur2Shader.setUniform2f("dir", scale/video.getWidth()*cos(HALF_PI/2), scale/video.getHeight()*sin(HALF_PI/2));
    layer3.draw(0,0);
    blur2Shader.end();
    ping.end();
    
    layer3.begin();
    blur2Shader.begin();
    blur2Shader.setUniform2f("dir", scale/video.getWidth()*cos(3*HALF_PI/2), scale/video.getHeight()*sin(3*HALF_PI/2));
    ping.draw(0,0);
    blur2Shader.end();
    layer3.end();
    
    
    ping.begin();
    screenShader.begin();
    screenShader.setUniformTexture("tex1", layer3.getTextureReference(), 2);
    layer2.draw(0, 0);
    screenShader.end();
    ping.end();
    
    src.begin();
    screenShader.begin();
    screenShader.setUniformTexture("tex1", ping.getTextureReference(), 3);
    layer1.draw(0, 0);
    screenShader.end();
    src.end();
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
        
        updateSource(src1, echo1);
        
        
    }
    
    
    video2.update();
    if (video2.isFrameNew()) {
        depthFbo.begin();
        depthShader.begin();
        video2.draw(0, 0);
        depthShader.end();
        depthFbo.end();
        
        updateSource(src2, echo1);
        
        
    }

}

//--------------------------------------------------------------
void ofApp::draw(){
    ofClear(0);
    ofSetColor(255);
    
    shader.begin();
    shader.setUniformTexture("tex0", src1.getTextureReference(), 4);
    shader.setUniform1f("hue1", hue1);
    shader.setUniform1f("hue2", hue2);
    shader.setUniform1f("sat", sat);
    shader.setUniform1f("offset", offset);
    src2.draw(0, 0);
    shader.end();

    
    
    
    if (bShowGui) {
        gui.draw();
    }
}


//--------------------------------------------------------------
void ofApp::keyPressed(int key){
//    bShowGui=!bShowGui;
    createDepthBlurShader(blur1Shader, radius1, variance1);
    createDepthBlurShader(blur2Shader, radius2, variance2);
    
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
