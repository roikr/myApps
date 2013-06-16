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
#include "testApp.h"

#include <Poco/Path.h>
#include "ofxXmlSettings.h"

enum {
    STATE_IDLE,
    STATE_PLAYING,
    STATE_PLAYING_LIVE
};

//--------------------------------------------------------------
void testApp::setup() {
    ofHideCursor();
    bCursorIsHidden = true;
    // setup OF sound stream
	soundStream.listDevices();
   

	
    ofxXmlSettings xml;
    bool bLoaded = xml.loadFile("settings.xml");
    assert(bLoaded);
    
    xml.pushTag("settings");
    soundStream.setDeviceID(xml.getValue("driver", 0));
    
    for (int i=0;i<xml.getNumTags("tuki");i++) {
        xml.pushTag("tuki",i);
        
        parrot p;
        
        
        cout << "host: " << xml.getValue("host", "localhost") << "\tport: " << xml.getValue("port", 3000) << endl;
        p.sender = new ofxOscSender; // must use pointer when contain in vector
        p.sender->setup(xml.getValue("host", "localhost"), xml.getValue("port", 3000));
        p.fps = xml.getValue("fps", 25);
        p.numFrames = xml.getValue("frames", 0);
        p.minShift = xml.getAttribute("shift", "min", 0.7);
        p.maxShift = xml.getAttribute("shift", "max", 1.0);
        p.volume = xml.getValue("volume", 1.0);
        p.pan = xml.getValue("pan", 0.5);
        p.bRepeat = xml.tagExists("repeat");
        p.speed = xml.getAttribute("repeat", "speed", 2.5);
        
        ofDirectory dir;
        dir.listDir(xml.getValue("path", ""));
        for (int j=0;j<dir.numFiles();j++) {
            ofxSndFile sound;
            sound.loadSound(dir.getPath(j));
            p.sounds.push_back(sound);
        }
        cout << p.sounds.size() << " loaded" <<  endl;
                
        xml.pushTag("segments");
        for (int j=0;j<xml.getNumTags("in");j++) {
            p.segments.push_back(make_pair(xml.getValue("in", 0,j), xml.getValue("out", 0,j)));
        }
        xml.popTag();
        
        p.bLooping = true; // just for starting
        p.endPos = 0;
        p.state = STATE_IDLE;
            
        parrots.push_back(p);
        
        xml.popTag();
        
    }
    
    xml.popTag();
    
    
    

    // the number if libpd ticks per buffer,
	// used to compute the audio buffer len: tpb * blocksize (always 64)
	int ticksPerBuffer = BUFFER_SIZE / ofxPd::blockSize();
	// setup the app core
    recordingSystem.setup();
    
    cout << "ticksPerBuffer: " << ticksPerBuffer << endl;

    input.assign(BUFFER_SIZE, 0);
    
	core.setup(2, 1, 44100, ticksPerBuffer);
    
    
    
    soundStream.setup(this, 2, 1, SAMPLE_RATE, BUFFER_SIZE, 3);
    
    
   }

//--------------------------------------------------------------
void testApp::update() {
    recordingSystem.update();
	core.update();
    
    
    for (vector<parrot>::iterator piter=parrots.begin();piter!=parrots.end();piter++) {
        int currentFrame = piter->startFrame+(int)floor((ofGetElapsedTimef()-piter->startTime)*piter->fps);
        
        if ((piter->bLooping && piter->state == STATE_IDLE) || (!piter->bLooping && currentFrame>=piter->numFrames)) {
            piter->startTime = ofGetElapsedTimef();
            piter->startFrame = currentFrame = piter->bLooping ? piter->endPos : 0;
            piter->bLooping = false;
            ofxOscMessage m;
            m.setAddress("/play");
            m.addIntArg(piter->startFrame);
            cout << distance(parrots.begin(), piter) <<  ": /play " << piter->startFrame << endl;
            piter->sender->sendMessage(m);
           
            
        }
        
        if (parrots[0].state==STATE_IDLE && parrots[1].state==STATE_IDLE) {
            for (vector<pair<int,int> >::iterator siter=piter->segments.begin();siter!=piter->segments.end();siter++) {
                if (currentFrame>=siter->first && currentFrame<siter->second) {
                    
                    if (recordingSystem.getNumRecordings() && ofGetElapsedTimef()-recordingSystem.getLastRecordingTime()<15) {
                        if(ofRandomuf()>0.3) {
                            recordingSystem.triggerLast();
                            piter->state = STATE_PLAYING_LIVE;
                        } else {
                            piter->sounds[(int)ofRandom(100) % piter->sounds.size()].play();
                            piter->state = STATE_PLAYING;
                        }
                    } else {
                        if (ofRandomuf()<0.5) {
                            if (ofRandomuf() > 0.8 && recordingSystem.getNumRecordings()) {
                                recordingSystem.trigger(ofRandom(100));
                                piter->state = STATE_PLAYING_LIVE;
                                

                            } else {
                                piter->sounds[(int)ofRandom(100) % piter->sounds.size()].play();
                                piter->state = STATE_PLAYING;
                            }
                        }
                    }
                    
                                   
                    
                    if (piter->state!=STATE_IDLE) {
                        if (piter->bRepeat) {
                            ofxOscMessage m;
                            m.setAddress("/repeat");
                            m.addIntArg(siter->first);
                            m.addIntArg(siter->second);
                            m.addFloatArg(piter->speed);
                            piter->endPos = siter->second;
                            piter->sender->sendMessage(m);
                            cout << distance(parrots.begin(), piter) <<  ": /repeat " << siter->first << "\t" << siter->second << "\t" << piter->speed << endl;
                        } else {
                            ofxOscMessage m;
                            m.setAddress("/shuffle");
                            m.addIntArg(siter->first);
                            m.addIntArg(siter->second);
                             piter->endPos = siter->second;
                            piter->sender->sendMessage(m);
                            cout << distance(parrots.begin(), piter) <<  ": /shuffle " << siter->first << "\t" << siter->second << endl;
                        }
                        piter->bLooping = true;
                        core.setTranspose(ofRandom(piter->minShift, piter->maxShift));
                        break;
                    }
                }
            }
            
        }
    }
    
   
}

//--------------------------------------------------------------
void testApp::draw() {
    recordingSystem.draw();
	core.draw();
}

//--------------------------------------------------------------
void testApp::exit() {
    soundStream.stop();
    for (vector<parrot>::iterator piter=parrots.begin();piter!=parrots.end();piter++) {
        delete piter->sender;
        piter->sender = 0;
    }
	core.exit();
}


//--------------------------------------------------------------
void testApp::keyPressed(int key) {
    
    if (key == 'p') {
        recordingSystem.triggerLast();
    }
    
    if (key>='1' && key<='9') {
        recordingSystem.trigger(key-'1');
    }
    
    if (key == 'c') {
        bCursorIsHidden =! bCursorIsHidden;
        if (bCursorIsHidden) {
            ofHideCursor();
        } else {
            ofShowCursor();
        }
    }
    
	core.keyPressed(key);
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y) {}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button) {}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button) {}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button) {}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h) {}

//--------------------------------------------------------------
void testApp::audioReceived(float * input, int bufferSize, int nChannels) {
    recordingSystem.audioIn(input, bufferSize, nChannels);
	
}

//--------------------------------------------------------------
void testApp::audioRequested(float * output, int bufferSize, int nChannels) {
    
    float pan = 0.5;
    float volume = 1.0;
    
    memset(input.data(), 0, bufferSize*sizeof(float));
    
    for (vector<parrot>::iterator piter=parrots.begin();piter!=parrots.end();piter++) {
        bool bPlaying = false;

        switch (piter->state) {
            case STATE_PLAYING: 
                for (vector<ofxSndFile>::iterator siter=piter->sounds.begin();siter!=piter->sounds.end();siter++) {
                    if (siter->getIsPlaying()) {
                        siter->audioRequested(input.data(), bufferSize, 1);
                        bPlaying = true;
                        break;
                    }
                }
                break;
                
            case STATE_PLAYING_LIVE:
                recordingSystem.audioOut(input.data(), bufferSize, 1);
                bPlaying = recordingSystem.getIsPlaying();
                
            break;
        }
        if (!bPlaying) {
            piter->state = STATE_IDLE;
        } else {
            pan = piter->pan;
            volume = piter->volume;
        }
    }
    
   
    core.audioReceived(input.data(), bufferSize, 1);
	core.audioRequested(output, bufferSize, nChannels);
    
    for (int i=0;i<bufferSize;i++) {
        output[i*nChannels] *= volume*(1.0-pan);
        output[i*nChannels+1] *= volume*pan;
    }
}
