//
//  Shaders.h
//  depthBlur
//
//  Created by Roee Kremer on 4/18/14.
//
//

#pragma once

#include "ofMain.h"

string getSimpleVertex();
void createDepthShader(ofShader &shader);
void createColor2GrayShader(ofShader &shader);
void createFastBlurShader(ofShader &shader,int radius,double variance);
void createDepthBlurShader(ofShader &shader,int radius,double variance);
void createVarDepthBlurShader(ofShader &shader,int radius,double variance);
void createThresholdShader(ofShader &shader);
void createScreenShader(ofShader &shader);
void createHSLShader(ofShader &shader);
void createEchoShader(ofShader &shader);