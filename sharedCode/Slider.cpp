//
//  Slider.cpp
//  slingshot
//
//  Created by Roee Kremer on 5/21/13.
//
//

#include "Slider.h"


#define SLIDER_HEIGHT 10.0f

void Slider::setup(ofRectangle rect) {
    this->rect = rect;
    value = 0.5;
}


void Slider::draw() {
    ofPushStyle();
    ofPushMatrix();
    ofTranslate(rect.x, rect.y);
    ofColor(100);
    ofLine(rect.width/2, SLIDER_HEIGHT/2, rect.width/2, rect.height-SLIDER_HEIGHT/2);
    ofColor(150);
    ofFill();
    ofRect(0,value*rect.height-SLIDER_HEIGHT/2,rect.width,SLIDER_HEIGHT);
    ofPopMatrix();
    ofPopStyle();
}

void Slider::setValue(float value) {
    this->value = value;
}

float Slider::getValue() {
    return value;
}

void Slider::touchDown(ofTouchEventArgs & touch) {
    if (rect.inside(touch.x, touch.y)) {
        bDown = true;
        value = ofMap(touch.y, rect.y, rect.y+rect.height, 0, 1);
        
    }
}

void Slider::touchMoved(ofTouchEventArgs & touch) {
    if (rect.inside(touch.x, touch.y)) {
        value = ofMap(touch.y, rect.y, rect.y+rect.height, 0, 1);
    }
}

void Slider::touchUp(ofTouchEventArgs & touch) {
    
}