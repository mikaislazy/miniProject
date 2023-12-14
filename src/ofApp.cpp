#include "ofApp.h"

//--------------------------------------------------------------
void ofApp::setup()
{
    // image init set up
    imgName = "apple.jpeg";
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
    gui.add(ccaLabel.setup("CCA Label", 0, 0, n));
    gui.add(original.setup("Original Image"));
    
    
}

//--------------------------------------------------------------
void ofApp::update()
{
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
}

//--------------------------------------------------------------
void ofApp::draw()
{
    // do the segmentation here
    imageSegmentation();

    // draw the image
    drawMat(processMat, 0, 0);
    drawMat(mat, 0, 400);

    // draw the gui
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

Mat ofApp::scalarAdd(Mat src, float val)
{
    return src + val;
}

// Mat ofApp::getForeGround(Mat src)
//{
//
//     Mat greyMat = toCv(greyImg);
//
//     Mat mask = Mat::zeros(greyMat.rows, greyMat.cols, CV_8UC3);
//
//     // thresholding the input image matrix
//     threshold(greyMat, greyMat, 128, 255, cv::THRESH_BINARY);
//
//     // run the connected component algorithm
//     n = connectedComponentsWithStats(greyMat, labels, stats, centroids);
//     cout << "Total Connected Components Detected: " << n << endl;
//     cout << stats << endl;
//     // create n different colors
//     vector<Vec3b> colors(n);
//     for (int i = 0; i < n; i++)
//     {
//         colors[i] = Vec3b(rand() % 256, rand() % 256, rand() % 256);
//     }
//
//     // fill the result matrix with colors according to which component a pixel belongs to
//     for (int i = 0; i < greyMat.rows; i++)
//     {
//         for (int j = 0; j < greyMat.cols; j++)
//         {
//             int label = labels.at<int>(i, j);
//             mask.at<Vec3b>(i, j) = mat.at<Vec3b>(i, j);
//         }
//     }
//     return mask;
// }

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
    vector<int>::iterator it ;
    
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
        cout << "label: " << label<< endl;
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
