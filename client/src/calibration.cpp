#include "calibration.hpp"
#include <opencv2/aruco.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/calib3d.hpp>
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
    dict = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_1000);
}

void Calibration::setDevice(libfreenect2::Freenect2Device *device)
{
    this->device = device;
}


void Calibration::detectMarkers(cv::Mat* img)
{
    if(calibrated) return;

    //Perform marker detection
    cv::aruco::detectMarkers(*img, dict, corners, ids);
    cv::aruco::drawDetectedMarkers(*img, corners, ids);
}


bool Calibration::calibrate(cv::Mat img)
{
    detectMarkers(&img);

    if(ids.size() == 0) {
        // if there are no markers in frame, fail
        INFO("Calibration failed, likely could not find any markers");
        return false;
    }
    INFO("found %d corners", (int)corners[0].size());
    INFO("found %d ids", (int)ids.size());

    // perform camera calibration
    cv::Ptr<cv::aruco::Board> board = cv::aruco::GridBoard::create(MARKER_X, MARKER_Y, MARKER_LENGTH, MARKER_SEPARATION, dict).staticCast<cv::aruco::Board>();
    board->ids[0] = MARKER_ID;

    //Get the depth parameters from device 
    libfreenect2::Freenect2Device::ColorCameraParams depthParameters = device->getColorCameraParams();
    
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

    cv::Mat cameraMatrix = cv::Mat(3,3,CV_64F,depthMatrix);
    cv::Mat distCoeffs = cv::Mat::zeros(5, 1, CV_64F);    
   
    // rotation (rvecs) and translation (tvecs) vectors that bring the marker from it's coordinate space to world coordinate space
    vector<cv::Mat> rvecs, tvecs;

    // amount of markers per frame (assume only one)
    vector<int> counter;
    counter.push_back(corners.size());

    // calibrate
    cv::aruco::calibrateCameraAruco(corners, ids, counter, board, img.size(), cameraMatrix, distCoeffs, rvecs, tvecs);
	
    // build transformation to apply to point cloud
    cv::Rodrigues(rvecs[0], rotation);
    cv::invert(rotation, rotation);
    translation = tvecs[0];
    cv::hconcat(rotation, translation, transformation);
    cv::Mat_<double> m = (cv::Mat_<double>(1, 4) << 0,0,0,1);
    cv::vconcat(transformation, m, transformation);

    INFO("Calibration success");
    calibrated = true;
    return true;
}

void Calibration::transformPoints(cv::Mat src, cv::Mat dst)
{
  cv::perspectiveTransform(src, dst, transformation);
}
