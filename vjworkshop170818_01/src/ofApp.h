#pragma once

#include "ofMain.h"
#include "ofxOsc.h"
#include "ofxProcessFFT.h"
#include "ofxPostProcessing.h"

class ofApp : public ofBaseApp{

public:
    void setup();
    void update();
    void draw();
    float easeOutCubic(float t, float b, float c, float d);
    
    ofxOscReceiver receiver;
    float value, exValue, fader01, fader02, fader03;
    vector<string> items;
    char address;
    int scene;
    
    ProcessFFT fft;
    float time, timeStamp;
    ofVec2f from, to, now;
    float easeEnd = 0.4;
    int toNum;
    int state;
    
    ofMesh *defaultMesh;
    ofMesh modifiedMesh;
    ofSpherePrimitive sphere;
    ofBoxPrimitive box;
    ofPlanePrimitive plane;
    
    float seed;
    float acceleration;
    ofColor color;
    
    Boolean aswitch, pswitch, cswitch, dswitch;
    int dmode;
    ofxPostProcessing effect;
    
};
