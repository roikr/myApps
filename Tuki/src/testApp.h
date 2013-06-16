/*
 * Copyright (c) 2012 Dan Wilcox <danomatika@gmail.com>
 * for Golan Levin & the CMU Studio for Creative Inquiry
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * See https://github.com/danomatika/PitchShifter for documentation
 *
 */
#pragma once

#include "ofMain.h"

#include "AppCore.h"
#include "RecordingSystem.h"
#include "ofxSndFile.h"
#include "ofxOsc.h"



struct parrot {
	vector<pair<int,int> > segments;
	int current;
	vector<ofxSndFile> sounds;
	float minShift;
	float maxShift;
    bool bRepeat;
    float speed;
    
    int state;
    
    float volume;
    float pan;
    
    ofxOscSender *sender;
    int numFrames;
    
    int startFrame;
    float startTime;
    float fps;
    
    bool bLooping; // playing video in segment loop
    int endPos;
    
    

};

/// a desktop os app wrapper
class testApp : public ofBaseApp{

	public:

		void setup();
		void update();
		void draw();
        void exit();

		void keyPressed  (int key);
		void mouseMoved(int x, int y );
		void mouseDragged(int x, int y, int button);
		void mousePressed(int x, int y, int button);
		void mouseReleased(int x, int y, int button);
		void windowResized(int w, int h);
		
		void audioReceived(float * input, int bufferSize, int nChannels);
		void audioRequested(float * output, int bufferSize, int nChannels);
		
		AppCore core;
        ofSoundStream soundStream;
        RecordingSystem recordingSystem;
    
        vector<parrot> parrots;
    
        vector<float> input;
    
        bool bCursorIsHidden;
};
