#include "ofApp.h"
#include <filesystem>
//--------------------------------------------------------------
void ofApp::setup()
{
    // turn on the camera and set the grabber
    vidGrabber.setDeviceID(0);
    vidGrabber.initGrabber(100, 100);
    // get the image path in data file
    string path = "../data";
    dir = ofDirectory(path);
    // image index init set up
    imgIdx = 1;
    dir.listDir();
    // load image
    imgName = dir.getPath(imgIdx - 1);
    initImg.load(imgName);
    greyImg.load(imgName);
    initImg.setImageType(OF_IMAGE_COLOR);
    greyImg.setImageType(OF_IMAGE_GRAYSCALE);
    mat = toCv(initImg);
    mat.copyTo(processMat);
    greyMat = toCv(greyImg);
    areas = getForeGround(greyMat);

    // gui set up
    gui.setup();
    // slider to select other image in data folder
    gui.add(imgIdx.setup("Image Index", 1, 1, dir.size()));
    gui.add(confirmChgImg.setup("Change Image"));

    // image adjustement
    gui.setPosition(initImg.getWidth(), 0);
    gui.add(contrastVal.setup("Contrast Value", 1, 0.1, 10));
    gui.add(brightnessVal.setup("Brightness value", 0, -100, 100));

    // convert image
    gui.add(otsu.setup("Otsu"));
    gui.add(CCA.setup("CCA"));
    gui.add(ccaLabel.setup("CCA Label", 0, 0, areas.size()));
    gui.add(original.setup("Original Image"));

    // reset Button
    gui.add(reset.setup("Reset Parameter"));

    // for hsv thresholding
    drawGui.setup();
    drawGui.add(minH.setup("Min H", 0, 0, 255));
    drawGui.add(minS.setup("Min S", 0, 0, 255));
    drawGui.add(minV.setup("Min V", 0, 0, 255));
    drawGui.add(maxH.setup("Max H", 255, 0, 255));
    drawGui.add(maxS.setup("Max S", 255, 0, 255));
    drawGui.add(maxV.setup("Max V", 255, 0, 255));
}

//--------------------------------------------------------------
void ofApp::update()
{
    // reset parameter
    if (reset)
    {
        resetParam();
    }
    // change to other image
    if (confirmChgImg)
    {
        imgName = dir.getPath(imgIdx - 1);
        initImg.load(imgName);
        greyImg.load(imgName);
        initImg.setImageType(OF_IMAGE_COLOR);
        greyImg.setImageType(OF_IMAGE_GRAYSCALE);
        mat = toCv(initImg);
        mat.copyTo(processMat);
        greyMat = toCv(greyImg);
        areas = getForeGround(greyMat);
        ccaLabel = 0;
        resetParam();

        // change the maximum number of CCA Label to current image
        ofxIntSlider *slider = (ofxIntSlider *)gui.getControl("CCA Label");
        slider->setMax(areas.size());
    }

    // select the segmentation method
    if (otsu)
    {
        segmentSelected = 0;
    }
    else if (CCA)
    {
        segmentSelected = 1;
    }
    else if (original)
    {

        segmentSelected = 2;
    }

    // update image when slider value change
    imageAdjustment();

    // when the user want to draw  on it
    if (drawOn)
    {
        // do the hsv thresholding
        vidGrabber.update();
        if (vidGrabber.isFrameNew())
        {
            im.setFromPixels(vidGrabber.getPixels());
            im.resize(initImg.getWidth(), initImg.getHeight());
            drawing = toCv(im);
            cvtColor(drawing, mat_HSV, CV_BGR2HSV);
            inRange(mat_HSV, Scalar(minH, minS, minV), Scalar(maxH, maxS, maxV), mat_HSV_Threshold);
            erode(mat_HSV_Threshold, mat_HSV_Threshold, Mat());
            dilate(mat_HSV_Threshold, mat_HSV_Threshold, Mat());

            ofImage im_temp;
            ofxCvGrayscaleImage im_temp_gray;

            toOf(mat_HSV_Threshold, im_temp);

            im_temp_gray.setFromPixels(im_temp.getPixels());
            // find only 1 contour to track
            contourFinder.findContours(im_temp_gray, 5, (340 * 240) / 4, 1, false, true);
        }
    }

    // when the user press D , then the contour will be drawn on the image and stored to the drawPoint
    for (int i = 0; i < drawingPoint.size(); i++)
    {
        cv::circle(processMat, cv::Point(drawingPoint[i].first, drawingPoint[i].second), 5, Scalar(0, 0, 255), -1);
    }
}

//--------------------------------------------------------------
void ofApp::draw()
{

    // do the segmentation here
    imageSegmentation();

    // draw the image
    drawMat(processMat, 0, 0);
    
    // draw the gui
    gui.draw();
    // only show the gui for hsv when user release key "D"
    if (drawOn)
    {
        drawMat(mat_HSV_Threshold, initImg.getWidth(), 0);
        drawGui.draw();
    }
}

//--------------------------------------------------------------
void ofApp::exit()
{
}

//--------------------------------------------------------------
void ofApp::keyPressed(int key)
{
    if (key == 'd')
    {
        for (int i = 0; i < contourFinder.nBlobs; i++)
        {

            ofPoint center = contourFinder.blobs[i].centroid;
            int centerX = static_cast<int>(center.x);
            int centerY = static_cast<int>(center.y);
            drawingPoint.push_back({centerX, centerY});
        }
    }
}

//--------------------------------------------------------------
void ofApp::keyReleased(int key)
{
    if (key == 's')
    {
        saveImage("saveImg.png", processMat);
    }

    if (key == 'd')
    {
        drawOn = !drawOn;
        // when the user release d again after drawing, the drawing will be clear
        if (drawOn)
        {
            drawingPoint.clear();
        }
    }
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
    Mat tmpMat;
    saveImg.copyTo(tmpMat);
    toOf(tmpMat, tmp); // convert mat to ofImage
    ofSaveImage(tmp.getPixels(), imgName);
    cout << "Save Image" << endl;
}

Mat ofApp::scalarAdd(Mat src, float val)
{
    return src + val;
}
void ofApp::imageAdjustment()
{
    // contrast
    mat.convertTo(processMat, -1, contrastVal, 0);

    // brightness
    processMat = scalarAdd(processMat, brightnessVal);

    cout << "imageAdjustment" << endl;
}
void ofApp::imageSegmentation()
{
    // otsu
    if (segmentSelected == 0)
    {
        Mat tmp;
        vector<Mat> bgr;
        split(processMat, bgr);
        threshold(bgr[0], tmp, 0, 255, CV_THRESH_BINARY | CV_THRESH_OTSU);
        bitwise_and(bgr[0], tmp, bgr[0]);
        bitwise_and(bgr[1], tmp, bgr[1]);
        bitwise_and(bgr[2], tmp, bgr[2]);
        merge(bgr, processMat);
    }
    else if (segmentSelected == 1)
    {
        // cca
        processMat = drawCCA(ccaLabel, processMat);
    }
    cout << "image Segmentation" << endl;
}
vector<pair<int, int>> ofApp::getForeGround(Mat img)
{
    int kernel = 5;
    // thresholding the input image matrix
    threshold(img, img, 128, 255, cv::THRESH_BINARY);
    GaussianBlur(img, img, cv::Size(kernel, kernel), 0);
    // run the connected component algorithm
    n = connectedComponentsWithStats(img, labels, stats, centroids);

    // store the area and label to vector
    vector<pair<int, int>> areas;
    for (int i = 0; i < n; i++)
    {
        int area = stats.at<int>(i, cv::CC_STAT_AREA);

        areas.push_back({area, i});
    };
    // sort areas in  descending order
    sort(areas.rbegin(), areas.rend());

    return areas;
}
Mat ofApp::drawCCA(int label, Mat img)
{
    // draw the selected number of label area of the image ( descending order)
    Mat rgbaImg;
    vector<int>::iterator it;

    Mat mask = Mat::zeros(img.rows, img.cols, CV_8UC4);

    // convert the original image to rgba
    vector<Mat> rgba;
    split(img, rgba);                          // split original image matrix to rgb and save to rgba vector
    Mat tmp(img.size(), CV_8UC1, Scalar(255)); // create alpha matrix
    rgba.push_back(tmp);                       // add the alpha matrix to rgba vector
    merge(rgba, rgbaImg);                      // convert the rgba vector to rgba matrix

    vector<int> drawLabel;
    // draw the top n label area of the image
    cout << "ccaLabel " << ccaLabel << endl;
    for (int i = 1; i <= ccaLabel; i++)
    {
        int label = areas[i].second;
        cout << "label: " << label << endl;
        drawLabel.push_back(label);
    }
    //
    for (int i = 0; i < img.rows; i++)
    {
        for (int j = 0; j < img.cols; j++)
        {
            int label = labels.at<int>(i, j);

            it = find(drawLabel.begin(), drawLabel.end(), label);
            if (it != drawLabel.end())
            {
                mask.at<Vec4b>(i, j) = rgbaImg.at<Vec4b>(i, j);
            }
            else
            {
                // set the other region transparent
                mask.at<Vec4b>(i, j)[3] = 0;
            }
        }
    }
    return mask;
}
void ofApp::resetParam()
{
    segmentSelected = 2;
    contrastVal = 1;
    brightnessVal = 0;
    ccaLabel = 0;
    drawingPoint.clear();
}
