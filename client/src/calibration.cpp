#include "calibration.hpp"
#include <opencv2/aruco.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <vector>
#include <cmdlog.h>
#include <iostream>

Calibration::Calibration(libfreenect2::Freenect2Device *device)
{
    // Device used for calibration
    this->device = device;

    // dict is the set of aruco markers that we are expecting to see
    dict = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_1000);
}

Calibration::Calibration()
{
  device = NULL;
  dict = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_50);
}

void Calibration::detectMarkers(cv::Mat* img)
{
    //Perform marker detection
    cv::aruco::detectMarkers(*img, dict, corners, ids);
    cv::aruco::drawDetectedMarkers(*img, corners, ids);
}


bool Calibration::calibrate(cv::Mat img)
{
    detectMarkers(&img);

    INFO("found %d corners", (int)corners[0].size());
    if(ids.size() == 0) {
        // if there are no markers in frame, fail
        INFO("Calibration failed, likely could not find any markers");
        return false;
    }

    // perform camera calibration
    cv::Ptr<cv::aruco::Board> board = cv::aruco::GridBoard::create(MARKER_X, MARKER_Y, MARKER_LENGTH, MARKER_SEPARATION, dict).staticCast<cv::aruco::Board>();

    //Get the depth parameters from device 
    libfreenect2::Freenect2Device::IrCameraParams depthParameters = device->getIrCameraParams();
    
    float fx,fy,cx,cy;
    fx = depthParameters.fx;
    fy = depthParameters.fy;
    cx = depthParameters.cx;
    cy = depthParameters.cy;

    float depthMatrix[3][3] = 
    {
     {fx, 0, cx},
     {0, fy, cy},
     {0,  0, 1}
    };    

    // (width,height)
    cv::Mat cameraMatrix = cv::Mat(3,3,CV_64F,depthMatrix);
    cv::Mat distCoeffs = cv::Mat::zeros(5, 1, CV_64F);    
   
    // rotation (rvecs) and translation (tvecs) vectors that bring the marker from it's coordinate space to world coordinate space
    //std::vector<Mat> rvecs, tvecs;
    std::vector<cv::Mat> rvecs, tvecs;

    // amount of markers per frame (assume only one)
    std::vector<int> counter;
    counter.push_back(corners[0].size());

    // calibrate
    cv::aruco::calibrateCameraAruco(corners, ids, counter, board, img.size(), cameraMatrix, distCoeffs, rvecs, tvecs);
	
    // transform output
    rotation = rvecs[0];
    translation = tvecs[0];

    INFO("Calibration success");
    return true;
}

