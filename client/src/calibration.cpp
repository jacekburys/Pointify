#include "calibration.hpp"

Calibration::Calibration()
{

}

void Calibration::calibrate()
{
  //TODO: get input image somehow (parameter/sensor?)
  //TODO: opencv includes

  // dict is the set of aruco markers that we are expecting to see
  cv::Ptr<cv::aruco::Dictionary> dict = cv::aruco::getPredefinedDictionary(cv::aruco::DICT_4X4_50);

  // perform marker detection
  vector<vector<Point2f>> corners;
  vector<int> ids;
  cv::aruco::detectMarkers(INPUT_IMAGE, dict, corners, ids, cv::aruco::DetectorParameters::create());

  //TODO: perform camera calibration with cv::aruco::calibrateCamera
}

