//
//  Slider.h
//  slingshot
//
//  Created by Roee Kremer on 5/21/13.
//
//

#pragma once

#include "ofMain.h"

class Slider {
public:
    void setup(ofRectangle rect);
    void draw();
    
    void setValue(float value);
    float getValue();
    
    void touchDown(ofTouchEventArgs & touch);
    void touchMoved(ofTouchEventArgs & touch);
    void touchUp(ofTouchEventArgs & touch);
    
private:
    ofRectangle rect;
    float value;
    bool bDown;
    


};