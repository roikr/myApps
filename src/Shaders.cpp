//
//  Shaders.cpp
//  depthBlur
//
//  Created by Roee Kremer on 4/18/14.
//
//

#include "Shaders.h"

#define STRINGIFY(A) #A

string getSimpleVertex() {
    return STRINGIFY(
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
}

void createDepthShader(ofShader &shader) {
    
    
    string fragment = STRINGIFY(
                                 \n#version 150\n
                                 uniform sampler2D tex0;
                                 uniform float minEdge;
                                 uniform float maxEdge;
                                 
                                 in vec2 texCoordVarying;
                                 out vec4 fragColor;
                                 
                                 void main(void) {
                                     float sample = texture(tex0,texCoordVarying).r;
                                     float dist = (sample-minEdge)/(maxEdge-minEdge);
                                     float color = (1-dist)*(step(minEdge,sample)-step(maxEdge,sample));
                                     fragColor = vec4(vec3(color),1.0);
                                     
                                 }
                                 
                                 );
    
    
    
    shader.setupShaderFromSource(GL_VERTEX_SHADER, getSimpleVertex());
    shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragment);
    shader.bindDefaults();
    shader.linkProgram();
}

void createColor2GrayShader(ofShader &shader) {
    
    string fragment = STRINGIFY(
                                \n#version 150\n
                                uniform sampler2D tex0;
                                
                                
                                in vec2 texCoordVarying;
                                out vec4 fragColor;
                                
                                
                                
                                void main(void) {
                                    fragColor = vec4(vec3(dot(texture(tex0,texCoordVarying).rgb,vec3(0.299, 0.587, 0.114))),1.0);
                                    
                                }
                                
                                );
    
    
    
    shader.setupShaderFromSource(GL_VERTEX_SHADER, getSimpleVertex());
    shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragment);
    shader.bindDefaults();
    shader.linkProgram();
}

void createFastBlurShader(ofShader &shader,int radius,double variance) {
    
    vector<double> coefs;
    double sum =0;
    
    for (int i=0; i<radius*2+1; i++) {
        double x = ofMap(i, 0, radius - 1, -1, 1);
        double y = (1. / sqrt(TWO_PI * variance)) * exp(-(x * x) / (2 * variance));
        sum+=y;
        coefs.push_back(y);
    }
    
    for(vector<double>::iterator iter=coefs.begin();iter!=coefs.end();iter++) {
        *iter/=sum;
        
    }
    
    
    
    stringstream blurVert;
    blurVert <<  STRINGIFY(
                           \n#version 150\n
                           uniform mat4 modelViewProjectionMatrix;
                           uniform vec2 dir;
                           in vec4 position;
                           in vec2 texcoord;

                           );
    
    blurVert << "out vec2 texCoordVarying[" << radius*2+1 << "];";
    
    blurVert <<  STRINGIFY(
                           
                           void main()
                           
                           );
    
    blurVert << " { "  ;
    
    blurVert <<  STRINGIFY(
                           gl_Position = modelViewProjectionMatrix * position;
                           );
    
    for (int i=0;i<radius*2+1;i++) {
        blurVert << "texCoordVarying[" << i << "] = texcoord + " << i-radius << " * dir;" ;
    }
    
    blurVert << "}" ;
    
    
    
    
    stringstream blurFrag;
    blurFrag << STRINGIFY(
                          \n#version 150\n
                          uniform sampler2D tex0;
                          );
    
    blurFrag << "in vec2 texCoordVarying[" << radius*2+1 << "];";
    
    blurFrag <<  STRINGIFY(
                           
                           
                           
                           out vec4 fragColor;
                           
                           
                           void main(void)
                           );
    
    blurFrag << "{ fragColor = vec4(0.0);";
    
    vector<float> coef;
    
    for (int i=0; i<radius*2+1; i++) {
        blurFrag << "fragColor += texture(tex0, texCoordVarying[" << i << "])*" << coefs[i] << ";";
    }
    
    blurFrag << "}";
    
    shader.setupShaderFromSource(GL_VERTEX_SHADER, blurVert.str());
    shader.setupShaderFromSource(GL_FRAGMENT_SHADER, blurFrag.str());
    shader.bindDefaults();
    shader.linkProgram();
    
    
}



void createDepthBlurShader(ofShader &shader,int radius,double variance) {
    
    vector<double> coefs;
    double sum =0;
    
    for (int i=0; i<radius*2+1; i++) {
        double x = ofMap(i, 0, radius - 1, -1, 1);
        double y = (1. / sqrt(TWO_PI * variance)) * exp(-(x * x) / (2 * variance));
        sum+=y;
        coefs.push_back(y);
    }
    
    for(vector<double>::iterator iter=coefs.begin();iter!=coefs.end();iter++) {
        *iter/=sum;
        
    }
    
    
    
    stringstream blurFrag;
    blurFrag << STRINGIFY(
                          \n#version 150\n
                          uniform sampler2D tex0;
                          in vec2 texCoordVarying;
                          uniform vec2 dir;
                          );
    
    
    blurFrag <<  STRINGIFY(
                           
                           
                           
                           out vec4 fragColor;
                           
                           
                           void main(void)
                           );
    
    blurFrag << "{ float color = 0.0;";
    
    
    for (int i=0; i<radius*2+1; i++) {
        blurFrag << "color += texture(tex0,texCoordVarying + " << i-radius << " * dir).r*" << coefs[i] << ";";
    }
    
    blurFrag << "fragColor=vec4(vec3(color),1.0);}";
    
    shader.setupShaderFromSource(GL_VERTEX_SHADER, getSimpleVertex());
    shader.setupShaderFromSource(GL_FRAGMENT_SHADER, blurFrag.str());
    shader.bindDefaults();
    shader.linkProgram();
    
    
}

void createVarDepthBlurShader(ofShader &shader,int radius,double variance) {
    
    vector<double> coefs;
    double sum =0;
    
    for (int i=0; i<radius*2+1; i++) {
        double x = ofMap(i, 0, radius - 1, -1, 1);
        double y = (1. / sqrt(TWO_PI * variance)) * exp(-(x * x) / (2 * variance));
        sum+=y;
        coefs.push_back(y);
    }
    
    for(vector<double>::iterator iter=coefs.begin();iter!=coefs.end();iter++) {
        *iter/=sum;
        
    }
    
    
    
    stringstream blurFrag;
    blurFrag << STRINGIFY(
                          \n#version 150\n
                          uniform sampler2D tex0;
                          uniform sampler2D depthTex;
                          in vec2 texCoordVarying;
                          uniform vec2 dir;
                          uniform float scale;
                          uniform float offset;
                          
                          out vec4 fragColor;
                          
                          
                          void main(void)
                          );
    
    
    blurFrag << "{ fragColor = vec4(0.0);";
    blurFrag << "float dist = 1-texture(depthTex,texCoordVarying).r;";
    blurFrag << "float factor=min(step(offset,dist)*scale,1);"; // (dist-offset)*
    
    
    
    
    
    for (int i=0; i<radius*2+1; i++) {
        blurFrag << "fragColor += texture(tex0,texCoordVarying + " << i-radius << " * dir*factor)*" << coefs[i] << ";";
    }
    
    blurFrag << "}";
    
    shader.setupShaderFromSource(GL_VERTEX_SHADER, getSimpleVertex());
    shader.setupShaderFromSource(GL_FRAGMENT_SHADER, blurFrag.str());
    shader.bindDefaults();
    shader.linkProgram();
    
    
}


void createThresholdShader(ofShader &shader) {
    
    string fragment = STRINGIFY(
                                \n#version 150\n
                                uniform sampler2D tex0;
                                uniform float edge0;
                                uniform float edge1;
                                
                                in vec2 texCoordVarying;
                                out vec4 fragColor;
                                
                               
                                
                                void main(void) {
                                    float c = texture(tex0,texCoordVarying).r;
                                    
                                    fragColor = vec4(vec3(smoothstep(edge0,edge1,c)*c),1.0);
//                                    fragColor = vec4(vec3(texture(tex0,texCoordVarying).r),1.0);
                                    
                                }
                                
                                );
    
    
    
    shader.setupShaderFromSource(GL_VERTEX_SHADER, getSimpleVertex());
    shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragment);
    shader.bindDefaults();
    shader.linkProgram();

}

void createScreenShader(ofShader &shader) {
    
    
    string fragment = STRINGIFY(
                                \n#version 150\n
                                uniform sampler2D tex0;
                                uniform sampler2D tex1;
                                
                                
                                in vec2 texCoordVarying;
                                out vec4 fragColor;
                                
                                void main(void) {
                                    vec4 col0 = texture(tex0,texCoordVarying);
                                    vec4 col1 = texture(tex1,texCoordVarying);
                                    fragColor = 1-(1-col0)*(1-col1);
                                    
                                }
                                
                                );
    
    
    
    shader.setupShaderFromSource(GL_VERTEX_SHADER, getSimpleVertex());
    shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragment);
    shader.bindDefaults();
    shader.linkProgram();

}

void createHSLShader(ofShader &shader) {
    
    
    string fragment = STRINGIFY(
                                \n#version 150\n
                                uniform sampler2D tex0;
                                uniform float hue;
                                uniform float sat;
                                uniform float offset;
                                
                                in vec2 texCoordVarying;
                                out vec4 fragColor;
                                
                                void main(void) {
                                    
                                    float l = texture(tex0,texCoordVarying).r+offset;
                                    
                                    float c= (1-abs(2*l-1))*sat;
                                    
                                    vec3 y;
                                    y.r = abs(hue * 6 - 3) - 1;
                                    y.g = 2 - abs(hue * 6 - 2);
                                    y.b = 2 - abs(hue * 6 - 4);
                                    fragColor = vec4((clamp(y,0,1)-0.5)*c+l,1.0);

                                    
                                }
                                
                                );
    
    
    
    shader.setupShaderFromSource(GL_VERTEX_SHADER, getSimpleVertex());
    shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragment);
    shader.bindDefaults();
    shader.linkProgram();

}

void createEchoShader(ofShader &shader) {
    
    string fragment = STRINGIFY(
                                \n#version 150\n
                                uniform sampler2D tex0;
                                uniform sampler2D tex1;
                                uniform float alpha;
                                
                                
                                in vec2 texCoordVarying;
                                out vec4 fragColor;
                                
                                void main(void) {
                                    fragColor = vec4(mix(texture(tex0,texCoordVarying).rgb,texture(tex1,texCoordVarying).rgb,alpha),1.0);
                                    
                                }
                                
                                );
    
    
    
    shader.setupShaderFromSource(GL_VERTEX_SHADER, getSimpleVertex());
    shader.setupShaderFromSource(GL_FRAGMENT_SHADER, fragment);
    shader.bindDefaults();
    shader.linkProgram();
}

