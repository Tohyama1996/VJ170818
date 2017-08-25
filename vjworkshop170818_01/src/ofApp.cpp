#include "ofApp.h"

void ofApp::setup(){
    ofBackground(0);
    ofSetFrameRate(60);
    ofSetFullscreen(true);
    receiver.setup(8888);
    scene = 0;
    
    fft.setup();
    fft.setNumFFTBins(16);
    fft.setNormalize(true);
    
    now = ofVec2f(0, 0);
    state = 1;
    
    float height = ofGetHeight();
    sphere.set(height*0.3, 8, OF_PRIMITIVE_TRIANGLES);
    sphere.setPosition(0, 0, 0);
    box.set(height*0.5, height*0.5, height*0.5, 6, 6, 6);
    box.setPosition(0, 0, 0);
    plane.set(height*0.8, height*0.8, 12, 12, OF_PRIMITIVE_TRIANGLES);
    plane.setPosition(0, 0, 0);
    
    defaultMesh = sphere.getMeshPtr();
    
    // Setup post-processing chain
    effect.init(ofGetWidth(), ofGetHeight());
    effect.createPass<BloomPass>()->setEnabled(false);
    effect.createPass<KaleidoscopePass>()->setEnabled(false);
}

void ofApp::update(){
    time = ofGetElapsedTimef() - timeStamp;
    fft.update();
    
    while (receiver.hasWaitingMessages()) {
        ofxOscMessage message;
        receiver.getNextMessage(message);
        value = message.getArgAsFloat(0);
        items = ofSplitString(message.getAddress(), "/");
        address = ofToChar(items.back());
        switch (address) {
            case 'a': fader01 = value; break;
            case 'b': fader02 = value; break;
            case 'c': fader03 = value; break;
            case '1':
                aswitch = !aswitch;
                break;
            case '2':
                pswitch = !pswitch;
                break;
            case '3':
                cswitch = !cswitch;
                break;
            case '4':
                dswitch = !dswitch;
                break;
            case '5':
                effect[0]->setEnabled(!effect[0]->getEnabled());
                effect[1]->setEnabled(!effect[1]->getEnabled());
                break;
            default:
                break;
        }
    }
    
    switch (scene) {
        case 0:
            defaultMesh = sphere.getMeshPtr();
            break;
        case 1:
            defaultMesh = box.getMeshPtr();
            break;
        case 2:
            defaultMesh = plane.getMeshPtr();
            break;
        default:
            break;
    }
    
    switch (state) {
        case 1:
            if (fft.getLowVal() > 0.2) {
                timeStamp = ofGetElapsedTimef();
                from = now;
                to = ofVec2f(ofRandom(360),ofRandom(360));
                state = 2;
            }
            break;
            
        case 2:
            if (time <= easeEnd) {
                now.x = easeOutCubic(time, from.x, to.x - from.x, easeEnd);
                now.y = easeOutCubic(time, from.y, to.y - from.y, easeEnd);
            } else {
                state = 1;
            }
            if (time > 0.2 && fft.getMidVal() > 0.5) {
                state = 1;
                timeStamp = ofGetElapsedTimef();
            }
            break;
            
        default:
            break;
    }
    
    modifiedMesh.clear();
    
    if (aswitch && fft.getMidVal() >= 0.1) acceleration = fft.getMidVal()*200;
    
    if (pswitch && fft.getHighVal() >= 0.2) scene = ofRandom(3);
    
    for (int i=0; i<defaultMesh->getNumIndices(); i++) {
        int index = defaultMesh->getIndex(i);
        ofVec3f position = defaultMesh->getVertex(index);
        ofVec3f force = position - ofVec3f(0);
        force.normalize();
        force *= ofRandom(-1,1) * acceleration;
        position += force;
        modifiedMesh.addVertex(position);
        seed += 0.001;
    }
    acceleration *= 0.95;
    
    if (cswitch) color.setHsb(ofRandom(255), 100, 255);
    else color = ofColor(255);
    
    
    if (dswitch && fft.getMidVal() > 0.2) {
        dmode = ofRandom(3);
    }

}

void ofApp::draw(){
    // copy enable part of gl state
    glPushAttrib(GL_ENABLE_BIT);
    
    // setup gl state
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    
    effect.begin();
    
    for (int i=0; i<fader01+1; i++) {
        ofPushMatrix();
        ofTranslate(ofGetWidth()/2, ofGetHeight()/2, i*-50);
        ofRotateX(10*time + now.x);
        ofRotateZ(15*time + now.y);
        
        ofSetColor(color);
        switch (dmode) {
            case 0:
                glLineWidth(2);
                modifiedMesh.setMode(OF_PRIMITIVE_TRIANGLES);
                modifiedMesh.drawWireframe();
                break;
                
            case 1:
                glPointSize(10);
                modifiedMesh.setMode(OF_PRIMITIVE_POINTS);
                modifiedMesh.drawWireframe();
                break;
                
            case 2:
                modifiedMesh.draw();
                break;
                
            default:
                break;
        }
        
        ofPopMatrix();
    }
    
    effect.end();
    
    // set gl state back to original
    glPopAttrib();
}

float ofApp::easeOutCubic(float t, float b, float c, float d) {
    t /= d ;
    t-- ;
    return (c*(t*t*t + 1) + b) ;
}
