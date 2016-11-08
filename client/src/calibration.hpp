#ifndef CALIBRATION_H 
#define CALIBRATION_H 
#include <opencv2/core.hpp>
#include <opencv2/aruco.hpp>

class Calibration 
{
private:
    // amount of markers in the x and y directions on the grid
    const int MARKER_X = 1;
    const int MARKER_Y = 1;
    
    // size of marker in meters(?)
    const int MARKER_LENGTH = 1;
    
    // distance between markers
    const int MARKER_SEPARATION = 1;
	
	//Every Calibration() object has these 
	
	// dict is the set of aruco markers that we are expecting to see
	cv::Ptr<cv::aruco::Dictionary> dict;
	
	// marker corners
	std::vector<std::vector<cv::Point2f>> corners;
	
	// marker ids 
    std::vector<int> ids;
	
public:
    Calibration();
	//~Calibration();
    bool calibrate(cv::Mat);
};

#endif
