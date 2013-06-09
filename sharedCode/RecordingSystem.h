//
//  RecordSystem.h
//  recordSystem
//
//  Created by Roee Kremer on 5/12/13.
//
//

#pragma once

#include "ofMain.h"

#define BUFFER_SIZE 512
#define SAMPLE_RATE 44100

class RecordingSystem{
	
public:
    
    void setup();
    void update();
    void draw();
    
    void trigger(int num);
    void triggerLast();
    
    bool getNumRecordings();
    bool getIsPlaying();
    float getLastRecordingTime();
        
    void audioIn(float * input, int bufferSize, int nChannels);
    void audioOut(float * output, int bufferSize, int nChannels);
    
    
private:
    
    vector <float> volHistory;
    
    float smoothedVol;
    float scaledVol;
    
    vector<float> record;
    int totalLength;
    int current;
    int start;
    int silence;
    
    deque<int> markers;
    
    bool bRecording;
    
    bool bPlaying;
    int playPos;
    int endPos;
    float speed;
    
    float lastTime;
};
