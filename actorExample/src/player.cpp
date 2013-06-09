//
//  player.cpp
//  videoPlayerExample
//
//  Created by Roee Kremer on 4/27/13.
//
//

#include "player.h"
#include "ofxXmlSettings.h"

#include "constants.h"

enum {
    SEQUENCE_IDLE = 0,
    SEQUENCE_IN = 1,
    SEQUENCE_OUT = 2
};

//--------------------------------------------------------------
void player::setup(string setName){
    
    ofxXmlSettings xml;
    xml.loadFile("VIDEOS/"+setName+"/"+setName+".xml");
    
    xml.pushTag("video_set");
    
    pos = ofPoint(xml.getValue("x", 0),xml.getValue("y", 0));
    
    xml.pushTag("actor");
    
    bindings.assign(3, 0);
    
    for (int i=0;i<xml.getNumTags("sequence");i++) {
        playerSequence s;
        sequences.push_back(s);
        vector<ofTexture> &images = sequences.back().images;
        
        string prefix = xml.getAttribute("sequence", "prefix","", i);
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
    
    currentSeqNum = 0;
    currentImageNum = 0;
    bPlaying = false;

}

//--------------------------------------------------------------
void player::update(){
    
    
    
    if (bPlaying) {
        currentImageNum = floor((ofGetElapsedTimef()-triggerTime) * FRAMES_PER_SECOND );
        if (currentImageNum==sequences[currentSeqNum].images.size()) {
            bPlaying = false;
            currentSeqNum = 0;
            currentImageNum = 0;
        }
    }
    
    if (!bPlaying) {
        trigger(bindings[SEQUENCE_IDLE]);
        
    }
    
}

//--------------------------------------------------------------
void player::draw(){
    if (bPlaying) {
        sequences[currentSeqNum].images[currentImageNum].draw(pos);
    }
}

void player::trigger(int num) {
    currentSeqNum = num;
    currentImageNum = 0;
    triggerTime = ofGetElapsedTimef();
    cout << currentSeqNum << " " << currentImageNum << endl;
    bPlaying = true;
}

