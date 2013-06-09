//
//  utils.h
//  mediaManager
//
//  Created by Roee Kremer on 6/7/13.
//
//

#pragma once

#include "ofMain.h"

void intRect(ofRectangle &rect);
ofPoint getCorner(ofRectangle rect,int i);
ofRectangle transformRect(ofRectangle rect,ofMatrix4x4 mat);
void findHomography(ofPoint src[4], ofPoint dst[4], float homography[16]);