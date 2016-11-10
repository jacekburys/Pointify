#include "calibration.hpp"
#include "libfreenect2/libfreenect2.hpp"
#include <opencv2/aruco.hpp>
#include <vector>

Calibration::Calibration(libfreenect2::Freenect2Device *device)
{
    // Device used for calibration
    this->device = device;

    // dict is the set of aruco markers that we are expecting to see
    dict = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_50);
}

Calibration::Calibration()
{
  device = NULL;
  dict = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_50);
}

bool Calibration::calibrate(cv::Mat img)
{
    
    //Perform marker detection
    cv::aruco::detectMarkers(img, dict, corners, ids);

    // perform camera calibration
    cv::Ptr<cv::aruco::Board> board = cv::aruco::GridBoard::create(MARKER_X, MARKER_Y, MARKER_LENGTH, MARKER_SEPARATION, dict).staticCast<cv::aruco::Board>();
   
   // Unsure about this bit 

    //Get the depth parameters from device 
    Freenect2Device::IrCameraParams depthParameters = device->getIrCameraParams();
    
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

    //

    // (width,height)
    cv::Mat cameraMatrix = Mat(3,3,CV_64F,depthMatrix);
    cv::Mat distCoeffs = cv::Mat::zeros(5, 1, CV_64F);    
   
    // rotation (rvecs) and translation (tvecs) vectors that bring the marker from it's coordinate space to world coordinate space
    //std::vector<Mat> rvecs, tvecs;
    cv::Mat rvecs, tvecs;

    // calibrate
    cv::aruco::calibrateCameraAruco(corners, ids, MARKER_X * MARKER_Y, board, img.size(), cameraMatrix, distCoeffs, rvecs, tvecs);
	
    return true;
}

