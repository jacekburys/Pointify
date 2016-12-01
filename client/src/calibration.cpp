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
    INFO("found %d markers", (int)ids.size());

    //Get the depth parameters from device 
    libfreenect2::Freenect2Device::IrCameraParams depthParameters = device->getIrCameraParams();
    
    float fx,fy,cx,cy;
    fx = depthParameters.fx;
    fy = depthParameters.fy;
    cx = depthParameters.cx;
    cy = depthParameters.cy;

    cameraMatrix = (cv::Mat_<float>(3,3) << fx,0,cx,0,fy,cy,0,0,1);
    distCoeffs = cv::Mat::zeros(4, 1, CV_64F);    
   
    // rotation (rvecs) and translation (tvecs) vectors that bring the marker from it's coordinate space to world coordinate space
    vector<cv::Vec3d> rvecs, tvecs;

    // find marker positions 
    cv::aruco::estimatePoseSingleMarkers(corners, MARKER_LENGTH, cameraMatrix, distCoeffs, rvecs, tvecs); 

    // build transformation to apply to point cloud
    rvec = rvecs[0];
    tvec = tvecs[0];
    cv::Mat_<float> r = (cv::Mat_<float>(3, 1) << rvecs[0][0],rvecs[0][1],rvecs[0][2]);
    cv::Rodrigues(r, r);

    cv::Mat_<float> t = (cv::Mat_<float>(3, 1) << tvecs[0][0],tvecs[0][1],tvecs[0][2]);
    cv::hconcat(r, t, transformation);

    cv::Mat_<float> m = (cv::Mat_<float>(1, 4) << 0,0,0,1);
    cv::vconcat(transformation, m, transformation);
    cv::invert(transformation, transformation);

    INFO("Calibration success");
    calibrated = true;
    return true;
}

// maps each point in src into a point in dst, transformed such that that the marker is now on the origin
void Calibration::transformPoints(cv::Mat src, cv::Mat dst)
{
  if(!calibrated)
  {
    dst = src;
  } else {
    cv::perspectiveTransform(src, dst, transformation);
  }
}
