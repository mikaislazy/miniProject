#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxOpenCv.h"
#include "ofxGui.h"
// # include <iostream>
// #include <opencv2/opencv.hpp>

using namespace ofxCv;
using namespace cv;

class ofApp : public ofBaseApp
{

public:
    void setup() override;
    void update() override;
    void draw() override;
    void exit() override;

    void keyPressed(int key) override;
    void keyReleased(int key) override;
    void mouseMoved(int x, int y) override;
    void mouseDragged(int x, int y, int button) override;
    void mousePressed(int x, int y, int button) override;
    void mouseReleased(int x, int y, int button) override;
    void mouseScrolled(int x, int y, float scrollX, float scrollY) override;
    void mouseEntered(int x, int y) override;
    void mouseExited(int x, int y) override;
    void windowResized(int w, int h) override;
    void dragEvent(ofDragInfo dragInfo) override;
    void gotMessage(ofMessage msg) override;

    void imageAdjustment();
    void saveImage(String imgName, Mat saveImg);
    Mat scalarAdd(Mat src, float val); // add brightness

    ofImage initImg;
    ofImage processImg;
    Mat mat;
    Mat processMat;
    string imgName;
    // gui
    ofxPanel gui;
    // filtering
    ofxButton box;
    ofxButton gaussian;
    ofxButton median;
    ofxFloatSlider brushSize;
    int checkBoxSelected;
    // image adjustment
    ofxFloatSlider contrastVal;
    ofxFloatSlider saturationVal;
    ofxFloatSlider brightnessVal;
    // binary image
    ofxButton binaryOtsu;
    // foreground extract
    ofxButton knn;
    ofxFloatSlider knnSize;
};