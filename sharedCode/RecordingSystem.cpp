//
//  RecordingSystem.cpp
//  recordSystem
//
//  Created by Roee Kremer on 5/12/13.
//
//

#include "RecordingSystem.h"

#define TOTAL_LENGTH 10000 // 10 minutes
#define SILENCE_LENGTH 170 // 1.0 sec
#define MIN_LENGTH 85 // 0.5 sec
#define MAX_LENGTH 1000

#define ATTACK_THRESH 0.05f
#define RELEASE_THRESH 0.01f

//--------------------------------------------------------------
void RecordingSystem::setup(){
    
    
	
	
	record.assign(TOTAL_LENGTH*BUFFER_SIZE, 0.0);
    start = silence = current = 0;
    markers.push_back(0);
    
	volHistory.assign(400, 0.0);
	
	smoothedVol     = 0.0;
	scaledVol		= 0.0;
    
    bRecording = false;
    speed = 1;
    bPlaying = false;
    
	lastTime = 0;
   
}

//--------------------------------------------------------------
void RecordingSystem::update(){
	//lets scale the vol up to a 0-1 range
	scaledVol = ofMap(smoothedVol, 0.0, 0.17, 0.0, 1.0, true);
    
	//lets record the volume into an array
	volHistory.push_back( scaledVol );
	
	//if we are bigger the the size we want to record - lets drop the oldest value
	if( volHistory.size() >= 400 ){
		volHistory.erase(volHistory.begin(), volHistory.begin()+1);
	}
}

//--------------------------------------------------------------
void RecordingSystem::draw(){
	
	ofSetColor(225);
	ofDrawBitmapString("AUDIO INPUT EXAMPLE", 32, 32);
	ofDrawBitmapString("press 's' to unpause the audio\n'e' to pause the audio", 31, 92);
	
	
    ofFill();
	
	ofPushStyle();
    ofPushMatrix();
    ofTranslate(0,120);
    float scale = (float)ofGetWidth()/(float)(2000);
    ofScale(scale, scale);
    ofSetColor(255,0,0);
    ofRect(start%2000, 0, 10, 10);
    ofSetColor(0,255,0);
    ofRect(current%2000, 10, 10, 10);
    ofSetColor(0,0,255);
    ofRect(silence%2000, 20, 10, 10);
    ofPopMatrix();
    ofPopStyle();
    
    ofDrawBitmapString(ofToString(markers.size()), 31, 150);
	
    
    ofNoFill();
	
	
	// draw the average volume:
	ofPushStyle();
    ofPushMatrix();
    ofTranslate(565, 170, 0);
    
    ofSetColor(225);
    ofDrawBitmapString("Scaled average vol (0-100): " + ofToString(scaledVol * 100.0, 0), 4, 18);
    ofRect(0, 0, 400, 400);
    
    ofSetColor(245, 58, 135);
    ofFill();
    
    
    //lets draw the volume history as a graph
    ofBeginShape();
    for (int i = 0; i < volHistory.size(); i++){
        if( i == 0 ) ofVertex(i, 400);
        
        ofVertex(i, 400 - volHistory[i] * 70);
        
        if( i == volHistory.size() -1 ) ofVertex(i, 400);
    }
    ofEndShape(false);
    
    ofPopMatrix();
	ofPopStyle();
    
    ofPushStyle();
    ofPushMatrix();
    ofTranslate(10, 170,0);
    ofSetColor(245, 58, 135);
    ofFill();
    ofRect(0, 400.0f*(1-smoothedVol), 50,400.0f*smoothedVol);
    
    ofSetColor(255);
    ofLine(0, 400.0f*(1-ATTACK_THRESH), 50, 400.0f*(1-ATTACK_THRESH));
    ofLine(0, 400.0f*(1-RELEASE_THRESH), 50, 400.0f*(1-RELEASE_THRESH));
    
    ofPopMatrix();
    ofPopStyle();
    
	
	
    
}

void RecordingSystem::trigger(int num) {
    if (markers.size()>1) {
        bPlaying = true;
        playPos = markers[num % (markers.size()-1)]*BUFFER_SIZE;
        endPos = markers[num % (markers.size()-1)+1]*BUFFER_SIZE;
        cout << "playPos: " << playPos << "\tendPos: " << endPos << endl;
    }
}

void RecordingSystem::triggerLast() {
    if (markers.size()>1) {
        bPlaying = true;
        playPos = markers[markers.size()-2]*BUFFER_SIZE;
        endPos = markers[markers.size()-1]*BUFFER_SIZE;
        cout << "playPos: " << playPos << "\tendPos: " << endPos << endl;
        
    }
}


bool RecordingSystem::getNumRecordings() {
    return markers.size()-1;
}


bool RecordingSystem::getIsPlaying() {
    return bPlaying;
}

float RecordingSystem::getLastRecordingTime() {
    return lastTime;
}
//--------------------------------------------------------------
void RecordingSystem::audioIn(float * input, int bufferSize, int nChannels){
	
	float curVol = 0.0;
	
	//lets go through each sample and calculate the root mean square which is a rough way to calculate volume
	for (int i = 0; i < bufferSize; i++){
		curVol += input[i] * input[i];
	}
    
    
	
	//this is how we get the mean of rms :)
	curVol /= (float)bufferSize;
	
	// this is how we get the root of rms :)
	curVol = sqrt( curVol );
	
	smoothedVol *= 0.93;
	smoothedVol += 0.07 * curVol;
    
    
    
    if (!bRecording && smoothedVol>ATTACK_THRESH) {
        bRecording = true;
        current = silence = start;
        
        
    }
    
    if (bRecording) {
        for (int i = 0; i < bufferSize; i++){
            record[(current % TOTAL_LENGTH)*bufferSize+i] = input[i];
        }
        
        current++;
        
        if (smoothedVol>RELEASE_THRESH) {
            silence = current;
        }
        
        if (current-start>MAX_LENGTH) {
            bRecording = false;
            current = silence = start;
        } else if (current-silence>SILENCE_LENGTH) {
            bRecording = false;
            lastTime = ofGetElapsedTimef();
            start = silence = current = current - SILENCE_LENGTH;
            markers.push_back(current);
            while (!markers.empty() && ((markers.front()-current)%TOTAL_LENGTH+TOTAL_LENGTH)%TOTAL_LENGTH<MAX_LENGTH) {
                markers.pop_front();
            }
        }
        
    }
    
   
	
}

void RecordingSystem::audioOut(float * output, int bufferSize, int nChannels) {
    memset(output, 0, bufferSize*nChannels*sizeof(float));
    
    if (bPlaying) {
        int i=0;
        while (playPos+(int)floor((float)i*speed)<endPos && i<bufferSize) {
        
            output[i] = record[(playPos+(int)floor((float)i*speed))%(TOTAL_LENGTH*BUFFER_SIZE)];
            i++;
            
        }
        
        playPos+=floor(bufferSize*speed);
        
        
        if (playPos>=endPos) {
            bPlaying = false;
        }
        
        
    }
    
}
