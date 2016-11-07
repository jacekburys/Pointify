#include "calibration.hpp"
#include <opencv2/aruco.hpp>
#include <vector>

Calibration::Calibration()
{
	// dict is the set of aruco markers that we are expecting to see
    dict = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_50);
}

/*Calibration::~Calibration()
{
	
}*/

bool Calibration::calibrate(cv::Mat img)
{
    
	//Perform marker detection
    cv::aruco::detectMarkers(img, dict, corners, ids);

    // perform camera calibration
    cv::Ptr<cv::aruco::Board> board = cv::aruco::GridBoard::create(MARKER_X, MARKER_Y, MARKER_LENGTH, MARKER_SEPARATION, dict).staticCast<cv::aruco::Board>();
	
	// cv::Mat is (height,width) 3x3 camera matrix
    cv::Mat cameraMatrix = cv::Mat::eye(3, 3, CV_64F);
	
    cv::Mat distCoeffs = NULL;/* TODO: getIrCameraParams() -> to Mat */
    cv::Mat rvecs, tvecs; // rotation and translation vectors that bring the marker from it's coordinate space to world coordinate space  
 
    cv::aruco::calibrateCameraAruco(corners, ids, MARKER_X * MARKER_Y, board, img.size(), cameraMatrix, distCoeffs, rvecs, tvecs);
	
	return true;
}

