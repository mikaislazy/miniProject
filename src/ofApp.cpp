#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
    // image init set up
    imgName = "faces.png";
    initImg.load(imgName);
    initImg.setImageType(OF_IMAGE_COLOR);
    matImg = toCv(initImg);
    // gui set up
    gui.setup();
    // image adjustement
    gui.setPosition(initImg.getWidth(), 0);
    gui.add(contrastVal.setup("Contrast Value", 0, -100, 100));
    gui.add(saturationVal.setup("Saturation value", 0, -100, 100));
    gui.add(brightnessVal.setup("Brightness value", 0, -100, 100));

    // filtering
    gui.add(brushSize.setup("Brush Size", 1, 1, 30));
    gui.add(box.setup("Box Filter"));
    gui.add(gaussian.setup("Gaussian Filter"));
    gui.add(median.setup("Median Filter"));

    // convert image
    gui.add(binaryOtsu.setup("Binary Otsu"));
    gui.add(knn.setup("KNN Segment"));
    gui.add(knnSize.setup("KNN Size", 0, 1, 4));
}

//--------------------------------------------------------------
void ofApp::update()
{
    // select checkbox for filtering
    if (box)
    {
        checkBoxSelected = 0;
    }
    else if (gaussian)
    {
        checkBoxSelected = 1;
    }
    else if (median)
    {
        checkBoxSelected = 2;
    }
}

//--------------------------------------------------------------
void ofApp::draw()
{
    imageAdjustment();
    drawMat(matImg, 0, 0);
    gui.draw();
}

//--------------------------------------------------------------
void ofApp::exit()
{
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key)
{
}

//--------------------------------------------------------------
void ofApp::mouseMoved(int x, int y)
{
}

//--------------------------------------------------------------
void ofApp::mouseDragged(int x, int y, int button)
{
}

//--------------------------------------------------------------
void ofApp::mousePressed(int x, int y, int button)
{
}

//--------------------------------------------------------------
void ofApp::mouseReleased(int x, int y, int button)
{
}

//--------------------------------------------------------------
void ofApp::mouseScrolled(int x, int y, float scrollX, float scrollY)
{
}

//--------------------------------------------------------------
void ofApp::mouseEntered(int x, int y)
{
}

//--------------------------------------------------------------
void ofApp::mouseExited(int x, int y)
{
}

//--------------------------------------------------------------
void ofApp::windowResized(int w, int h)
{
}

//--------------------------------------------------------------
void ofApp::gotMessage(ofMessage msg)
{
}

//--------------------------------------------------------------
void ofApp::dragEvent(ofDragInfo dragInfo)
{
}

void ofApp::saveImage(String imgName, Mat saveImg)
{
    ofImage tmp;
    toOf(saveImg, tmp); // convert mat to ofImage
    ofSaveImage(tmp.getPixels(), imgName);
    cout << "Save Image" << endl;
}

Mat ofApp::scalarAdd(Mat m1, int val)
{
    return m1 + val;
}

void