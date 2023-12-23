#include "ofApp.h"
#include <filesystem>
//--------------------------------------------------------------
void ofApp::setup()
{
    vidGrabber.setDeviceID(0);
    vidGrabber.initGrabber(100, 100);
    // get the image path in data file
    string path = "../data";
    dir = ofDirectory(path);
    // image init set up
    imgIdx = 1;
    dir.listDir();
    for (int i = 0; i < dir.size(); i++)
    {
        ofLogNotice(dir.getPath(i));
    }
    // imgLen = dir.size();
    imgName = dir.getPath(imgIdx - 1);
    //    imgName = "faces.png";
    initImg.load(imgName);
    greyImg.load(imgName);
    initImg.setImageType(OF_IMAGE_COLOR);
    greyImg.setImageType(OF_IMAGE_GRAYSCALE);
    mat = toCv(initImg);
    mat.copyTo(processMat);
    greyMat = toCv(greyImg);
    areas = getForeGround(greyMat);
    // n = areas.size();

    // gui set up
    gui.setup();
    // next or prev img
    gui.add(imgIdx.setup("Image Index", 1, 1, dir.size()));
    gui.add(confirmChgImg.setup("Change Image"));

    // image adjustement
    gui.setPosition(initImg.getWidth(), 0);
    gui.add(contrastVal.setup("Contrast Value", 1, 0.1, 10));
    gui.add(brightnessVal.setup("Brightness value", 0, -100, 100));

    // filtering
    gui.add(brushSize.setup("Brush Size", 1, 1, 30));
    gui.add(box.setup("Box Filter"));
    gui.add(gaussian.setup("Gaussian Filter"));
    gui.add(median.setup("Median Filter"));

    // convert image
    gui.add(otsu.setup("Otsu"));
    gui.add(CCA.setup("CCA"));
    gui.add(ccaLabel.setup("CCA Label", 0, 0, areas.size()));
    gui.add(original.setup("Original Image"));

    // reset Button
    gui.add(reset.setup("Reset Parameter"));

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
    if (reset)
    {
        resetParam();
    }
    // next or prev image
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
        // n = areas.size();

        //        imgIdx.setMax(areas.size());
        ofxIntSlider *slider = (ofxIntSlider *)gui.getControl("CCA Label");
        slider->setMax(areas.size());
        vidGrabber.close();
        ofVideoGrabber tmp;
        tmp.setDeviceID(0);
        tmp.initGrabber(initImg.getWidth(), initImg.getHeight());
        vidGrabber = tmp;
    }
    // select checkbox for filtering
    if (box)
    {
        filterSelected = 0;
    }
    else if (gaussian)
    {
        filterSelected = 1;
    }
    else if (median)
    {
        filterSelected = 2;
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
    if (drawOn)
    {

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

            contourFinder.findContours(im_temp_gray, 5, (340 * 240) / 4, 1, false, true);
        }
    }
}

//--------------------------------------------------------------
void ofApp::draw()
{

    // do the segmentation here
    imageSegmentation();

    // draw the image
    drawMat(processMat, 0, 0);
    drawMat(mat_HSV_Threshold, 0, 400);
    // draw the gui
    gui.draw();
    if (drawOn)
    {
        drawGui.draw();
        for (int i = 0; i < contourFinder.nBlobs; i++)
        {
            ofRectangle r = contourFinder.blobs.at(i).boundingRect;
            // r.x += 320; r.y += 240;
            // c.setHsb(i * 64, 255, 255);
            ofSetColor(255, 0, 0);
            ofNoFill();
            ofDrawRectangle(r);
            ofPoint center = contourFinder.blobs[i].centroid;
            int centerX = static_cast<int>(center.x);
            int centerY = static_cast<int>(center.y);
            // cv::circle(processMat, cv::Point(centerX, centerY), 5, Scalar(0, 0, 255), -1);
            drawingPoint.push_back({centerX, centerY});
        }
        for (int i = 0; i < drawingPoint.size(); i++)
        {
            cv::circle(processMat, cv::Point(drawingPoint[i].first, drawingPoint[i].second), 5, Scalar(0, 0, 255), -1);
        }
        ofSetColor(255, 255, 255);
        drawMat(processMat, mat.cols, mat.cols);
    }
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
    if (key == 's')
    {
        saveImage("saveImg.png", processMat);
    }

    if (key == 'd')
    {
        drawOn = !drawOn;
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
    Mat rgbaImg;
    vector<int>::iterator it;

    Mat mask = Mat::zeros(img.rows, img.cols, CV_8UC4);

    // convert the original image to rgba
    vector<Mat> rgba;
    split(img, rgba);                          // split orginal image matrix to rgb and save to rgba vector
    Mat tmp(img.size(), CV_8UC1, Scalar(255)); // create alpha matrix
    rgba.push_back(tmp);                       // add the alpha matrix to rgba vector
    merge(rgba, rgbaImg);                      // conver the rgba vector to rgba matrix

    vector<int> drawLabel;
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
            //  the label of the pixel in the flowers region equal to the original pixel

            it = find(drawLabel.begin(), drawLabel.end(), label);
            if (it != drawLabel.end())
            {
                mask.at<Vec4b>(i, j) = rgbaImg.at<Vec4b>(i, j);
            }
            else
            {
                // set the region outside the flower to transparent
                mask.at<Vec4b>(i, j)[3] = 0;
            }
        }
    }
    return mask;
}
void ofApp::resetParam()
{
    filterSelected = -1;
    segmentSelected = 2;
    brushSize = 0;
    contrastVal = 1;
    brightnessVal = 0;
    ccaLabel = 0;
}