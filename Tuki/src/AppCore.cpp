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
#include "AppCore.h"

//--------------------------------------------------------------
void AppCore::setup(const int numOutChannels, const int numInChannels,
				    const int sampleRate, const int ticksPerBuffer) {

	ofSetFrameRate(60);
	ofSetVerticalSync(true);
	
	// setup pd
	if(!pd.init(numOutChannels, numInChannels, sampleRate, ticksPerBuffer)) {
		ofLog(OF_LOG_ERROR, "Could not init pd");
		OF_EXIT_APP(1);
	}
	pd.subscribe("mix");
	pd.subscribe("transpose");
	pd.subscribe("inputGain");
	pd.subscribe("outputGain");
	pd.addToSearchPath("pd");
	pd.start();

	// open patch
	Patch patch = pd.openPatch("pd/_main.pd");
	cout << patch << endl;

	// setup GUI
	int x = -12, width = 100, step = 75;
	x += step;
	transposeSlider.setup(x, 34, width, 700, -12, 12, 0, true, true); x += width + step*2;
	mixSlider.setup(x, 34, width, 700, 0, 1, 1.0, true, true); x += width + step*2;
	inGainSlider.setup(x, 34, width, 700, 0, 1, 0.5, true, true); x += width + step*2;
	outGainSlider.setup(x, 34, 100, 700, 0, 1, 0.25, true, true);
	transposeSlider.setLabelString("Transpose");
	mixSlider.setLabelString("Wet/Dry Mix");
	inGainSlider.setLabelString("Input Gain");
	outGainSlider.setLabelString("Output Gain");
}

//--------------------------------------------------------------
void AppCore::update() {
	ofBackground(0, 0, 0);
	
	// update scope array from pd
	pd.readArray("scope", scopeArray);
	
	// udpate pd from gui
	pd << StartMessage() << "transpose" << transposeSlider.getValue() << FinishList("TO_PD");
	pd << StartMessage() << "mix" << mixSlider.getValue() << FinishList("TO_PD");
	pd << StartMessage() << "inGain" << inGainSlider.getValue() << FinishList("TO_PD");
	pd << StartMessage() << "outGain" << outGainSlider.getValue() << FinishList("TO_PD");
}

//--------------------------------------------------------------
void AppCore::draw() {

	// draw scope
	ofSetColor(0, 255, 0, 127);
	ofSetRectMode(OF_RECTMODE_CENTER);
	ofSetLineWidth(2.0);
	float x = 1, y = ofGetHeight()/2;
	float w = ofGetWidth() / (float) scopeArray.size(), h = ofGetHeight()/2;
	for(int i = 0; i < scopeArray.size()-1; ++i) {
		ofLine(x, y+scopeArray[i]*h, x+w, y+scopeArray[i+1]*h);
		x += w;
	}
	ofSetLineWidth(1.0);
}

//--------------------------------------------------------------
void AppCore::exit() {}

void AppCore::setTranspose(float x) {
    transposeSlider.setPercent(x);
}

//--------------------------------------------------------------
void AppCore::keyPressed(int key) {}

//--------------------------------------------------------------
void AppCore::audioReceived(float * input, int bufferSize, int nChannels) {
	pd.audioIn(input, bufferSize, nChannels);
}

//--------------------------------------------------------------
void AppCore::audioRequested(float * output, int bufferSize, int nChannels) {
	pd.audioOut(output, bufferSize, nChannels);
}

//--------------------------------------------------------------
void AppCore::print(const std::string& message) {
	cout << message << endl;
}
