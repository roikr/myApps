#include "ofApp.h"

#define STRINGIFY(A) #A

//--------------------------------------------------------------
void ofApp::setup(){
    ofDisableArbTex();
    image.loadImage("linzer.png");
    
    mesh.setMode(OF_PRIMITIVE_TRIANGLE_STRIP);
    
    ofVec3f vert;
    ofVec3f normal(0, 0, 1); // always facing forward //
    ofVec2f texcoord;
    
    int rows=480;
    int columns=640;
    // the origin of the plane is the center //
//    float halfW = width/2.f;
//    float halfH = height/2.f;
    // add the vertexes //
    for(int iy = 0; iy < rows; iy++) {
        for(int ix = 0; ix < columns; ix++) {
            
            // normalized tex coords //
            //texcoord.x = ((float)ix/((float)columns-1.f));
            //texcoord.y = ((float)iy/((float)rows-1.f));
            
            //vert.x = texcoord.x * width - halfW;
            //vert.y = texcoord.y * height - halfH;
            vert.x = ix-rows/2;
            vert.y = iy-columns/2;
            
            mesh.addVertex(vert);
            //mesh.addTexCoord(texcoord);
            //mesh.addNormal(normal);
        }
    }
    
    for(int y = 0; y < rows-1; y++) {
        // even rows //
        if((y&1)==0) {
            for(int x = 0; x < columns; x++) {
                mesh.addIndex( (y) * columns + x );
                mesh.addIndex( (y+1) * columns + x);
            }
        } else {
            for(int x = columns-1; x >0; x--) {
                mesh.addIndex( (y+1) * columns + x );
                mesh.addIndex( y * columns + x-1 );
            }
        }
    }
    
    if(rows%2!=0) mesh.addIndex(mesh.getNumVertices()-columns);
    
    
    
    string vertex = STRINGIFY(
                              \n#version 150\n
                              
                              
                              in vec4 position;
                              void main() {
                                  
                                  gl_Position = position;
                              }
                              );
    
    
    string geometry = STRINGIFY(
                                \n#version 150\n
                                
                                uniform	sampler2D tex0;
                                uniform mat4 modelViewProjectionMatrix;
                                
                                uniform float time;
                                
                                layout(triangles) in;
                                layout(triangle_strip, max_vertices=3) out;
                                
                               
                                
                                void main()
                                {
                                    
                                    
                                    bool prim = true;
                                    
                                    for(int i=0; i<3; i++)
                                    {
                                        vec4 pos = gl_in[i].gl_Position;
                                        
                                        vec2 uDims = textureSize(tex0,0);
                                        vec2 texcoord = vec2(pos.x/uDims.x+0.5,pos.y/uDims.y+0.5);
                                        float depth = texture(tex0,texcoord).a;
                                        
                                        if (depth<0.5) {
                                            prim = false;
                                            break;
                                        }
                                    
                                        gl_Position = modelViewProjectionMatrix*vec4(pos.x,pos.y,(depth-0.5)*960,pos.w);
                                        EmitVertex();
                                    }
                                    
                                    if (prim)
                                    {
                                        EndPrimitive();
                                    }
                                }
                                );
    
    string fragment = STRINGIFY(
                                \n#version 150\n
                                
                                //                                in float color;
                                out vec4 fragColor;
                                
                                
                                void main(void) {
                                    fragColor = vec4(vec3(0.5),0.1);
                                    
                                }
                                
                                );
    
    shader.setGeometryInputType(GL_TRIANGLE_STRIP);
	shader.setGeometryOutputType(GL_TRIANGLE_STRIP);
	shader.setGeometryOutputCount(1);
    shader.setupShaderFromSource(GL_VERTEX_SHADER, vertex);
    shader.setupShaderFromSource(GL_GEOMETRY_SHADER, geometry);
    shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragment);
    
    
	
//	printf("Maximum number of output vertices support is: %i\n", shader.getGeometryMaxOutputCount());
	
    
    
    shader.bindDefaults();
    shader.linkProgram();
    shader.begin();
    shader.setUniformTexture("tex0", image.getTextureReference(), 1);
    shader.end();
    

}

//--------------------------------------------------------------
void ofApp::update(){
    ofSetWindowTitle(ofToString(ofGetFrameRate()));
}

//--------------------------------------------------------------
void ofApp::draw(){
    ofBackground(0);
    ofSetColor(255);
    
    cam.begin();
    ofEnableDepthTest();
    ofScale(1, -1, 1); // make y point down
	ofScale(.5, .5, .5); // make everything a bit smaller
    shader.begin();
    shader.setUniform1f("time", ofGetElapsedTimef()*TWO_PI);
//    mesh.drawWireframe();
    mesh.drawFaces();
    shader.end();
    ofDisableDepthTest();
    cam.end();
    
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
