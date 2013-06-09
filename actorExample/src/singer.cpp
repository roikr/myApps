//
//  singer.cpp
//  actorExample
//
//  Created by Roee Kremer on 4/25/13.
//
//

#include "singer.h"
#include "ofxXmlSettings.h"
#include "constants.h"

enum {
    SEQUENCE_IDLE = 0,
    SEQUENCE_IN = 1,
    SEQUENCE_OUT = 2
};
//--------------------------------------------------------------
void singer::setup(string setName){
    
    ofxXmlSettings xml;
    xml.loadFile("VIDEOS/"+setName+"/"+setName+".xml");
    
    bindings.assign(3, 0);
    
    
    xml.pushTag("video_set");
    
    pos = ofPoint(xml.getValue("x", 0),xml.getValue("y", 0));
    
    
    xml.pushTag("actor",0);
    
    
    
    for (int i=0;i<xml.getNumTags("sequence");i++) {
        singerSequence s;
        sequences.push_back(s);
        vector<ofTexture> &images = sequences.back().images;
        
        string prefix = xml.getAttribute("sequence", "prefix","", i);
        
        seqmap[prefix]=i;
        for (int j=0;j<xml.getAttribute("sequence", "frames",0, i);j++) {
            stringstream filename;
            filename << "VIDEOS/" << setName << "/" << prefix << "/" << prefix << '_' << setfill('0') << setw(2) << j+1 << ".png";
            cout << "loading " <<filename.str() << endl;
            
            ofImage image;
            image.setUseTexture(false);
            image.loadImage(filename.str());
            
            ofTexture texture;
            images.push_back(texture);
            images.back().allocate(image.getPixelsRef());
            images.back().loadData(image.getPixelsRef());
            
        }
        
        if (prefix==setName+"_IDLE") {
            bindings[SEQUENCE_IDLE]=i;
            cout << "IDLE: " << bindings[SEQUENCE_IDLE] << endl;
        }
        
        if (prefix==setName+"_IN") {
            bindings[SEQUENCE_IN]=i;
            cout << "IN: " << bindings[SEQUENCE_IN] << endl;
        }
        
        if (prefix==setName+"_OUT") {
            bindings[SEQUENCE_OUT]=i;
            cout << "OUT: " << bindings[SEQUENCE_OUT] << endl;
        }
        
    }
    
    xml.popTag();
    
    xml.pushTag("actor",1);
    
    for (int i=0;i<xml.getNumTags("sequence");i++) {
        string prefix = xml.getAttribute("sequence", "prefix","", i);
        string seqname = prefix.substr(0,prefix.size()-string("_LIPS").size());
        map<string,int>::iterator iter = seqmap.find(seqname);
        if (iter!=seqmap.end()) {
            
            vector<ofTexture> &images = sequences[iter->second].lips_images;
            
            for (int j=0;j<xml.getAttribute("sequence", "frames",0, i);j++) {
                stringstream filename;
                filename << "VIDEOS/" << setName << "/" << seqname << "/" << prefix << '_' << setfill('0') << setw(2) << j+1 << ".png";
                cout << "loading " <<filename.str() << endl;
                ofImage image;
                image.setUseTexture(false);
                image.loadImage(filename.str());
                
                ofTexture texture;
                images.push_back(texture);
                images.back().allocate(image.getPixelsRef());
                images.back().loadData(image.getPixelsRef());
            }
        }
    }
    
    xml.popTag();
    
    xml.popTag();
    
    xml.loadFile("VIDEOS/"+setName+"/"+setName+"_TRACKS.xml");
    xml.pushTag("tracks");
    for (int i=0;i<xml.getNumTags("track");i++) {
        vector<pair<int,int> > &track = sequences[seqmap[xml.getAttribute("track", "name", "",i)]].track;
        xml.pushTag("track",i);
        for (int j=0;j<xml.getNumTags("point");j++) {
            track.push_back(make_pair(xml.getAttribute("point", "x", 0,j), xml.getAttribute("point", "y", 0,j)));
        }
        xml.popTag();
    }
    xml.popTag();
    
    bPlaying = false;
    bLips = false;
    
    currentFrameNum = 0;
    currentSampleNum = 0;
    currentSequenceNum = bindings[SEQUENCE_IDLE];
    currentImageNum = 0;
}

void singer::setupSamples(vector<sample>&samples) {
    this->samples = samples;
    for (vector<sample>::iterator iter=samples.begin(); iter!=samples.end(); iter++) {
        cout << "sequence: " <<  iter->seqname << "\t" <<seqmap[iter->seqname] << endl;
        sampvec.push_back(seqmap[iter->seqname]);
    }
    
}

//--------------------------------------------------------------
void singer::update(){
    
    
    
    if (bPlaying) {
        currentFrameNum = floor((ofGetElapsedTimef()-triggerTime) * FRAMES_PER_SECOND );
        if (currentFrameNum>=samples[currentSampleNum].frames.size()) {
            bPlaying = false;
        } else {
            currentImageNum = samples[currentSampleNum].frames[currentFrameNum];
            if (currentImageNum>=sequences[currentSequenceNum].images.size()) {
                cout << "image overflow seq: " << currentSequenceNum << "\timage: " << currentImageNum <<  "\ttotal: " << sequences[currentSequenceNum].images.size() << endl;
                
                bPlaying = false;
                currentImageNum = 0;
                
            }
            
            if (currentFrameNum>=samples[currentSampleNum].lips.size()) {
                bLips = false;
            } else {
                bLips = true;
                currentLipsImageNum = currentImageNum * 6 + samples[currentSampleNum].lips[currentFrameNum];
                vector<pair<int,int> >::iterator iter = sequences[currentSequenceNum].track.begin()+currentImageNum;
                currentLipsPos = ofPoint(iter->first,iter->second);
            }
        }
        
        
    }
    
    if (!bPlaying) {
        trigger(0); // bindings[SEQUENCE_IDLE]
        
    }
    
}

//--------------------------------------------------------------
void singer::draw(){
    if (bPlaying) {
        sequences[currentSequenceNum].images[currentImageNum].draw(pos);
        
        if (bLips) {
            sequences[currentSequenceNum].lips_images[currentLipsImageNum].draw(pos+currentLipsPos);
        }
    }
    
}

void singer::trigger(int num) {
    if (num>=samples.size()) {
        cout << "sample overflow" << endl;
        return;
    }
    
    
   
    
    currentSampleNum = num;
    currentSequenceNum =  sampvec[currentSampleNum];
    
    cout << "trigger sample: " << currentSampleNum << "\tsequence: " << currentSequenceNum << endl;
    
    currentImageNum = 0;
    currentFrameNum = 0;
    triggerTime = ofGetElapsedTimef();
    //    cout << currentSeqNum << " " << currentImageNum << endl;
    bPlaying = true;
    
    bLips = false;
    currentLipsImageNum = 0;
}
