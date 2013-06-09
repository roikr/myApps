//
//  player.h
//  videoPlayerExample
//
//  Created by Roee Kremer on 4/27/13.
//
//

#pragma once
#include "ofMain.h"

struct playerSequence {
    vector<ofTexture> images;
    
};


class player {
public:
    void setup(string setNam);
    void update();
    void draw();
	
    void trigger(int num);
    
    vector<playerSequence> sequences;
    vector<int> bindings;
    float triggerTime;
    bool bPlaying;
    int currentSeqNum;
    int currentImageNum;
    
    ofPoint pos;
    
};