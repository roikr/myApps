//
//  ofxPointGrab.cpp
//  videoGrabberExample
//
//  Created by Roee Kremer on 5/20/13.
//
//

#include "ofxPointGrab.h"
#include "PG_Engine.h"

class MyCallback : public IPG_EngineCallback
{
public:
    virtual void OnNewFrame(IplImage * pFrame, PG_EngineResult const & result)
    {
        this->result = result;
        this->pFrame = pFrame;
    }
    
    PG_EngineResult result;
    IplImage *pFrame;
};


struct Wrapper {
    PG_Engine * pPG;
    
    MyCallback pPGCallback;
    
    vector<char> gray_image_data;
    
    IplImage* image;
    IplImage* gray_image;
    
};

static Wrapper wrapper;


void ofxPointGrab::setup(int width,int height) {
    wrapper.pPG = PG_Engine::Instance();
    bool bRes = wrapper.pPG->Init("", "", NULL, 0, "", "", "", "", "", "", VIDEO_FORMAT_NV21);
    assert(bRes);
    
    
    wrapper.gray_image_data.assign(width*height, 0);
    
    
    wrapper.image = cvCreateImageHeader(cvSize(width,height), IPL_DEPTH_8U, 4);
    
    
    wrapper.image->width =width;
    wrapper.image->height = height;
    wrapper.image->nChannels = 4;
    wrapper.image->depth = IPL_DEPTH_8U;
    wrapper.image->widthStep = width*3;
    wrapper.image->imageSize = width*height*3;
    
    
    wrapper.gray_image = cvCreateImageHeader(cvSize(width,height), IPL_DEPTH_8U, 1);
    
    wrapper.gray_image->width =width;
    wrapper.gray_image->height = height;
    wrapper.gray_image->nChannels = 1;
    wrapper.gray_image->depth = IPL_DEPTH_8U;
    wrapper.gray_image->widthStep = width;
    wrapper.gray_image->imageData=wrapper.gray_image_data.data();
    wrapper.gray_image->imageSize = width*height;
    
    
    
}

void ofxPointGrab::update(ofPixelsRef &pixels) {
    wrapper.image->imageData = reinterpret_cast<char*>(pixels.getPixels());
    
    cvCvtColor(wrapper.image, wrapper.gray_image, CV_BGRA2GRAY);
    bool bRes = wrapper.pPG->ProcessFrame(wrapper.gray_image, &wrapper.pPGCallback);
    assert(bRes);
    
    cout << wrapper.pPGCallback.result.m_PrimaryPalm.m_bIsTrackable;
        

}
