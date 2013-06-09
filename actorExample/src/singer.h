//
//  singer.h
//  actorExample
//
//  Created by Roee Kremer on 4/25/13.
//
//

#pragma once

#include "ofMain.h"

struct sample {
    string seqname;
    vector<int>frames;
    vector<int>lips;
};

struct singerSequence {
    vector<ofTexture> images;
    vector<ofTexture> lips_images;
    vector<pair<int,int> > track;
};

class singer{
    public:
        void setup(string setName);
        void update();
        void draw();
        
        void setupSamples(vector<sample>&samples);
        void trigger(int num);
        
        vector<singerSequence> sequences;
        
        vector<int> bindings;
        map<string,int> seqmap;
        float triggerTime;
        bool bPlaying;
        
        int currentFrameNum;
        int currentSampleNum;
    
        int currentSequenceNum;
        int currentImageNum;
    
        bool bLips;
        int currentLipsImageNum;
        ofPoint currentLipsPos;
    
    
        vector<sample> samples;
        vector<int> sampvec;
    
        ofPoint pos;
};