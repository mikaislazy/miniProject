#pragma once

#include "ofMain.h"
#include "ofxCv.h"
#include "ofxOpenCv.h"
#include "ofxGui.h"
// # include <iostream>
// #include <opencv2/opencv.hpp>

using namespace ofxCv;
using namespace cv;
using namespace std;

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
    // Mat contrastAdjustment(Mat matIn, int contrast, int brightness);
    vector<pair<int, int>> getForeGround(Mat src); // get the sorted area
    void imageSegmentation();                      // do the segmentation on the processed image
    Mat drawCCA(int label, Mat img);               // draw the selected label area of the image
    void resetParam();

    // variable
    ofDirectory dir;
    ofImage initImg;
    ofImage processImg;
    ofImage greyImg;
    Mat mat;
    Mat processMat;
    Mat greyMat;
    string imgName;
    int imgLen;
    int curImg = 0;
    // CCA
    Mat labels;
    Mat stats;
    Mat centroids;
    int n;
    vector<pair<int, int>> areas;
    // gui
    ofxPanel gui;

    // image adjustment
    ofxFloatSlider contrastVal;
    ofxFloatSlider brightnessVal;
    // image segmentation
    ofxButton otsu;
    ofxButton CCA;
    ofxIntSlider ccaLabel;
    int maxLabel;
    int segmentSelected = 2;
    ofxButton original;

    // next image
    ofxIntSlider imgIdx;
    ofxButton confirmChgImg;
    // reset Param
    ofxButton reset;
    // draw
    ofVideoGrabber vidGrabber;
    ofImage im;
    Mat drawing;
    Mat mat_HSV;
    Mat mat_HSV_Threshold;
    Mat mat_HSV_Threshold_2;
    ofxPanel drawGui;
    bool drawOn = false;
    ofxIntSlider minH;
    ofxIntSlider maxH;
    ofxIntSlider minS;
    ofxIntSlider maxS;
    ofxIntSlider minV;
    ofxIntSlider maxV;
    ofxCvContourFinder contourFinder;
    vector<pair<int, int>> drawingPoint;
};
