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
    bEdge = false;
    
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
        
        ofRectangle rect(0,0,s.width,s.height);
        int left = xml.getAttribute("inset", "left", 0);
        int top = xml.getAttribute("inset", "top", 0);
        int right = xml.getAttribute("inset", "right", 0);
        int bottom = xml.getAttribute("inset", "bottom", 0);
        
        for (int j=0;j<xml.getNumTags("corner");j++) {
            corner c;
            c.pos = getCorner(rect, j);
            switch (j) {
                case 0:
                    c.pos+=ofPoint(left,top);
                    break;
                case 1:
                    c.pos+=ofPoint(-right,top);
                    break;
                case 2:
                    c.pos+=ofPoint(-right,-bottom);
                    break;
                case 3:
                    c.pos+=ofPoint(left,-bottom);
                    break;
                    
                default:
                    break;
            }
            
            
            c.warp = c.pos+ofPoint(xml.getAttribute("corner", "dx", 0,j),xml.getAttribute("corner", "dy", 0,j));
            s.corners.push_back(c);
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
        xml.addTag("inset");
        xml.addAttribute("inset", "left", (int)siter->corners[0].pos.x,0);
        xml.addAttribute("inset", "top", (int)siter->corners[0].pos.y,0);
        xml.addAttribute("inset", "right", (int)(siter->width-siter->corners[1].pos.x),0);
        xml.addAttribute("inset", "bottom", (int)(siter->height-siter->corners[2].pos.y),0);
        
        for (vector<corner>::iterator citer=siter->corners.begin();citer!=siter->corners.end();citer++) {
            xml.addTag("corner");
            xml.addAttribute("corner", "dx", (int)(citer->warp.x-citer->pos.x),distance(siter->corners.begin(),citer));
            xml.addAttribute("corner", "dy", (int)(citer->warp.y-citer->pos.y),distance(siter->corners.begin(), citer));
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
            sink &s=sinks.front();
            
            
            if (bCorner) {
                
                ofRectangle rect;
                rect.setFromCenter(s.mat.preMult(s.corners[selectedCorner].warp), 10, 10);
                ofRect(rect);
            }
            
            
            
            ofSetColor(0, 255, 0);
            
        }
        ofRect(iter->rect);
        
        ofSetColor(255, 0, 0);
        ofRectangle rect(0,0,iter->width,iter->height);
        ofBeginShape();
        for (vector<corner>::iterator citer=iter->corners.begin(); citer!=iter->corners.end(); citer++) {
            ofVec3f pos(iter->mat.preMult(citer->pos));
            ofVertex(pos);
        }
        ofEndShape(true);
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
        
        
        for (vector<corner >::iterator iter=siter->corners.begin();iter!=siter->corners.end();iter++) {
            src.push_back(iter->pos);
            dst.push_back(iter->warp);
            
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
        ofPoint disp = (pos-lastPos).rotated(-s.orientation*90, ofVec3f(0,0,1));
        if (bCorner) {
            s.corners[selectedCorner].warp+=disp;
        } else if (bEdge){
            int nextCorner = (selectedCorner+1) % 4;
            
            ofPoint diff(s.corners[nextCorner].pos-s.corners[selectedCorner].pos);
            if (diff.y) {
                s.corners[selectedCorner].pos+=ofPoint(disp.x,0);
                s.corners[nextCorner].pos+=ofPoint(disp.x,0);
            } else {
                s.corners[selectedCorner].pos+=ofPoint(0,disp.y);
                s.corners[nextCorner].pos+=ofPoint(0,disp.y);
            }
            
            s.corners[selectedCorner].pos = clampPoint(ofRectangle(0,0,s.width,s.height), s.corners[selectedCorner].pos);
            s.corners[nextCorner].pos = clampPoint(ofRectangle(0,0,s.width,s.height), s.corners[nextCorner].pos);
            
            
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
        bEdge = false;
        sink &s = sinks.front();
    
        //ofRectangle rect =transformRect(ofRectangle(ofPoint(0,0),s.width,s.height), s.mat);
        for (vector<corner >::iterator iter=s.corners.begin();iter!=s.corners.end();iter++) {

            if ((pos-s.mat.preMult(iter->pos)).length()<50) {
                 
                bCorner = true;
                selectedCorner = distance(s.corners.begin(), iter);
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
            ofPoint spos = s.imat.preMult(pos);
            for (vector<corner>::iterator iter=s.corners.begin();iter!=s.corners.end();iter++) {
                vector<corner>::iterator niter=iter+1;
                if (niter==s.corners.end()) {
                    niter=s.corners.begin();
                }
                
                ofRectangle rect;
                ofPoint center = (iter->pos+niter->pos)/2;
                rect.setFromCenter(center, MAX(100,abs(niter->pos.x-iter->pos.x)), MAX(100,abs(niter->pos.y-iter->pos.y)));
                
                
                
                if (rect.inside(spos)) {
                    
                    bEdge = true;
                    selectedCorner = distance(s.corners.begin(), iter);
                    bZoom = true;
                    float scale = mat.getScale().x*2;
                    
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
        
        if (!bEdge ) {
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
    bEdge = false;
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