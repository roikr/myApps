#include "testApp.h"
#include "ofxXmlSettings.h"
#include "utils.h"






//--------------------------------------------------------------
void testApp::setup(){
    loadPrefs();
    
    image.loadImage("checkers.jpg");
    
    defMat.scale(0.25, 0.25, 1);
    defMat.translate(ofGetWidth()/2, ofGetHeight()/2, 0);
    mat = defMat;
    imat = mat.getInverse();
    bSelected = false;
    bCorner = false;
    
    ofEnableAlphaBlending();
}


void testApp::loadPrefs() {
    ofxXmlSettings xml;
    bool bLoaded = xml.loadFile("settings.xml");
    assert(bLoaded);
    xml.pushTag("settings");
    for (int i=0; i<xml.getNumTags("sink"); i++) {
        xml.pushTag("sink",i);
        sink s;
        s.width = xml.getAttribute("size", "width", 0);
        s.height = xml.getAttribute("size", "height", 0);
        s.orientation = xml.getValue("orientation", 0);
        s.position = ofPoint(xml.getAttribute("position", "x", 0),xml.getAttribute("position", "y", 0));
        
        s.name = xml.getValue("name", "default");
        
        for (int j=0;j<xml.getNumTags("corner");j++) {
            s.corners.push_back(make_pair(ofPoint(xml.getAttribute("corner", "rx", 0,j),xml.getAttribute("corner", "ry", 0,j)), ofPoint(xml.getAttribute("corner", "ox", 0,j),xml.getAttribute("corner", "oy", 0,j))));
        }
        
        s.host=xml.getAttribute("osc", "host", "localhost");
        s.port=xml.getAttribute("osc", "port", 3000);
        s.sender = new ofxOscSender();
        s.sender->setup(s.host, s.port);
        
        
        
        sinks.push_back(s);
        xml.popTag();
    }
    
   
    updateSinks();
    
}

void testApp::savePrefs() {
    ofxXmlSettings xml;
    xml.addTag("settings");
    xml.pushTag("settings");
    for (vector<sink>::iterator siter=sinks.begin(); siter!=sinks.end(); siter++) {
        xml.addTag("sink");
        xml.pushTag("sink",distance(sinks.begin(), siter));
        xml.addTag("position");
        xml.addAttribute("position", "x", (int)siter->position.x,0);
        xml.addAttribute("position", "y", (int)siter->position.y,0);
        xml.addTag("size");
        xml.addAttribute("size", "width", siter->width,0);
        xml.addAttribute("size", "height",siter->height,0);
        xml.addValue("orientation", siter->orientation);
        xml.addValue("name", siter->name);
        for (vector<pair<ofPoint,ofPoint> >::iterator citer=siter->corners.begin();citer!=siter->corners.end();citer++) {
            xml.addTag("corner");
            xml.addAttribute("corner", "rx", (int)citer->first.x,distance(siter->corners.begin(),citer));
            xml.addAttribute("corner", "ry", (int)citer->first.y,distance(siter->corners.begin(), citer));
            xml.addAttribute("corner", "ox", (int)citer->second.x,distance(siter->corners.begin(),citer));
            xml.addAttribute("corner", "oy", (int)citer->second.y,distance(siter->corners.begin(), citer));
        }
        
        xml.addTag("osc");
        xml.addAttribute("osc", "host", siter->host,0);
        xml.addAttribute("osc", "port", siter->port,0);

        xml.popTag();
    }
    xml.popTag();
    xml.saveFile("settings.xml");
    
}


//--------------------------------------------------------------
void testApp::update(){

}

//--------------------------------------------------------------
void testApp::draw(){
    ofPushMatrix();
    ofMultMatrix(mat);
    
    image.draw(0, 0);
    
    ofPushStyle();
    
    
    ofNoFill();
    for (vector<sink>::reverse_iterator iter=sinks.rbegin();iter!=sinks.rend();iter++) {
        
        ofPushStyle();
       
                
        ofSetColor(255, 0, 0);
        if (bSelected && iter==sinks.rend()-1) {
            ofSetColor(0, 0, 255);
            
            if (bCorner) {
                sink &s=sinks.front();
                ofRectangle rect;
                
                rect.setFromCenter(s.mat.preMult(getCorner(ofRectangle(0,0,s.width,s.height),corner)+s.corners[corner].first+s.corners[corner].second), 10, 10);
                ofRect(rect);
            }
            
            ofSetColor(0, 255, 0);
            
        }
        ofRect(iter->rect);
        
        ofPopStyle();
    }
    ofPopStyle();
    
    ofPushStyle();
    ofFill();
    ofSetColor(255, 255, 0,50);
    for (vector<sink>::iterator siter=sinks.begin();siter!=sinks.end();siter++) {
       
        for (vector<ofRectangle>::iterator iter=siter->intersections.begin();iter!=siter->intersections.end();iter++) {
            ofRect(*iter);
        }
        
       
    }
    ofPopStyle();
    
    
    
    ofPopMatrix();
}

void testApp::exit(){
    savePrefs();
}



void testApp::updateSinks() {
    
    
    for (vector<sink>::iterator siter=sinks.begin();siter!=sinks.end();siter++ ){
       
        siter->mat=ofMatrix4x4::newIdentityMatrix();
        siter->mat.rotate(siter->orientation*90, 0, 0, 1);
        siter->mat.translate(siter->position);
        siter->imat = siter->mat.getInverse();
        siter->rect =transformRect(ofRectangle(ofPoint(0,0),siter->width,siter->height), siter->mat);
        
        
        vector<ofPoint> dst;
        vector<ofPoint> src;
        ofRectangle rect(0,0,siter->width,siter->height);
        
        for (vector<pair<ofPoint, ofPoint> >::iterator iter=siter->corners.begin();iter!=siter->corners.end();iter++) {
            ofPoint p(getCorner(rect, distance(siter->corners.begin(), iter)));
            src.push_back(p+iter->first);
            dst.push_back(p+iter->first+iter->second);
            
        }
        
        ofMatrix4x4 warp;
        findHomography(src.data(), dst.data(), warp.getPtr());
        
        ofxOscMessage m;
        m.setAddress("/warp");
        for (int i=0;i<16;i++) {
            m.addFloatArg(warp.getPtr()[i]);
            siter->sender->sendMessage(m);
        }
        
        m.clear();
        m.setAddress("/xform");
        for (int i=0;i<16;i++) {
            m.addFloatArg(siter->imat.getPtr()[i]);
            siter->sender->sendMessage(m);
        }
       
        
        siter->intersections.clear();
        m.clear();
        m.setAddress("/clear");
        siter->sender->sendMessage(m);
    }
    
    for (vector<sink>::iterator siter=sinks.begin();siter!=sinks.end();siter++ ){
        for (vector<sink>::iterator iter=sinks.begin();iter!=sinks.end();iter++ ){
            if (siter!=iter && siter->rect.intersects(iter->rect)) {
                ofRectangle rect(siter->rect.getIntersection(iter->rect));
                
                siter->intersections.push_back(rect);
                rect = transformRect(rect,siter->imat);
                ofxOscMessage m;
                m.setAddress("/intersection");
                m.addIntArg(rect.x);
                m.addIntArg(rect.y);
                m.addIntArg(rect.width);
                m.addIntArg(rect.height);
                siter->sender->sendMessage(m);
            }
        }
    }
}

//--------------------------------------------------------------
void testApp::keyPressed(int key){
    switch (key) {
        case 'r':
            if (bSelected) {
                sink &s=sinks.front();
                s.orientation = (s.orientation+1)%4;
                ofPoint center = s.rect.getCenter();
                s.position = (s.position-s.rect.getCenter()).rotated(90, ofVec3f(0,0,1))+s.rect.getCenter(); // rotate around center
                updateSinks();
            }
            
            break;
            
        default:
            break;
    }
}

//--------------------------------------------------------------
void testApp::keyReleased(int key){
    
}

//--------------------------------------------------------------
void testApp::mouseMoved(int x, int y){
    
    ofPoint pos =imat.preMult(ofPoint(x,y));
//    cout << "mouseMoved\t" << pos.x << "\t" << pos.y << endl;
}

//--------------------------------------------------------------
void testApp::mouseDragged(int x, int y, int button){
    
    ofPoint pos =imat.preMult(ofPoint(x,y));
    if (bSelected) {
        sink &s = sinks.front();
        
        if (bCorner) {
            s.corners[corner].second+=(pos-lastPos).rotated(-s.orientation*90, ofVec3f(0,0,1));
        } else {
            
            s.position +=pos-lastPos;
        }
       
        lastPos=pos;
        
        
        
        updateSinks();
         
        
    }
    
    
//    cout << "mouseDragged\t" << pos.x << "\t" << pos.y << "\t" << button << endl;
}

//--------------------------------------------------------------
void testApp::mousePressed(int x, int y, int button){
    ofPoint pos =imat.preMult(ofPoint(x,y));
//    cout << "mousePressed\t" << pos.x << "\t" << pos.y << "\t" << button << endl;
    bSelected = false;
    for (vector<sink>::iterator iter=sinks.begin();iter!=sinks.end();iter++) {
        if (iter->rect.inside(pos)) {
            bSelected = true;
            sink s = sinks.front();
            sinks.front() = *iter;
            *iter = s;
            
            lastPos = pos;
            break;
        }
    }
    
    
    if (bSelected) {
        bCorner = false;
        sink &s = sinks.front();
        //ofRectangle rect =transformRect(ofRectangle(ofPoint(0,0),s.width,s.height), s.mat);
        for (vector<pair<ofPoint, ofPoint> >::iterator iter=s.corners.begin();iter!=s.corners.end();iter++) {
            
            if ((pos-s.mat.preMult(getCorner(ofRectangle(0,0,s.width,s.height),distance(s.corners.begin(), iter))+iter->first)).length()<50) {
                 
                bCorner = true;
                corner = distance(s.corners.begin(), iter);
                bZoom = true;
                float scale = mat.getScale().x*25;
                
                mat=ofMatrix4x4::newIdentityMatrix();
                
                //mat.translate(-rect.getCenter());
                mat.translate(-pos);
                mat.scale(0.75*scale, 0.75*scale, 1);
                //                    mat.translate(ofPoint(ofGetWidth(),ofGetHeight())/2);
                mat.translate(x, y,0);
                imat = mat.getInverse();
                lastPos = imat.preMult(ofPoint(x,y));
                break;

            }
        }
        
        if (!bCorner) {
            for (vector<sink>::iterator iter=sinks.begin()+1;iter!=sinks.end();iter++) {
                if (sinks.front().rect.intersects(iter->rect)) {
                    
                   
                    ofRectangle rect = sinks.front().rect.getIntersection(iter->rect);
                    if (rect.inside(pos)) {
                        bZoom = true;
                        //float scale = 0.75/max(rect.getWidth()/ofGetWidth(),rect.getHeight()/ofGetHeight());
                        float scale = mat.getScale().x*25;
                        
                        mat=ofMatrix4x4::newIdentityMatrix();
                        
                        //mat.translate(-rect.getCenter());
                        mat.translate(-pos);
                        mat.scale(0.75*scale, 0.75*scale, 1);
    //                    mat.translate(ofPoint(ofGetWidth(),ofGetHeight())/2);
                        mat.translate(x, y,0);
                        imat = mat.getInverse();
                        lastPos = imat.preMult(ofPoint(x,y));
                        break;
                    }
                }
            }
        }
    }
}

//--------------------------------------------------------------
void testApp::mouseReleased(int x, int y, int button){
    ofPoint pos =imat.preMult(ofPoint(x,y));
    bSelected = false;
    bCorner = false;
    if (bZoom) {
        mat = defMat;
        imat = mat.getInverse();
        bZoom = false;
    }
//    cout << "mouseReleased\t" << pos.x << "\t" << pos.y << "\t" << button << endl;
}

//--------------------------------------------------------------
void testApp::windowResized(int w, int h){

}

//--------------------------------------------------------------
void testApp::gotMessage(ofMessage msg){

}

//--------------------------------------------------------------
void testApp::dragEvent(ofDragInfo dragInfo){ 

}