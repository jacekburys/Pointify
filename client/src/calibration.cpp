#include "calibration.hpp"
//TODO: opencv includes

Calibration::Calibration()
{

}

void Calibration::calibrate()
{
    cv::Mat img = /*TODO: get input image somehow (parameter/sensor?)*/

    // dict is the set of aruco markers that we are expecting to see
    cv::Ptr<cv::aruco::Dictionary> dict = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_50);

    // perform marker detection
    vector<vector<Point2f>> corners;
    vector<int> ids;

    cv::aruco::detectMarkers(img, dict, corners, ids);

    // perform camera calibration
    cv::Ptr<cv::aruco::Board> board = aruco::GridBoard::create(MARKER_X, MARKER_Y, MARKER_LENGTH, MARKER_SEPARATION, dict).staticCast<aruco::Board>();
    cv::Mat cameraMatrix = cv::Mat::eye(3, 3, cv::CV_64F)
    cv::Mat distCoeffs = /* TODO: getIrCameraParams() -> to Mat */
    cv::Mat rvects, tvecs;
 
    cv::aruco::calibrateCameraAruco(corners, ids, MARKER_X * MARKER_Y, board, img.size(), cameraMatrix, distCoeffs, rvecs, tvecs)
}

