#ifndef CALIBRATION_H 
#define CALIBRATION_H 
#include <opencv2/core.hpp>
#include <opencv2/aruco.hpp>
#include <socketio/sio_client.h>
#include "libfreenect2/libfreenect2.hpp"

using namespace std;

class Calibration 
{
private:
    const int MARKER_ID = 871;
    // amount of markers in the x and y directions on the grid
    const int MARKER_X = 1;
    const int MARKER_Y = 1;
    
    // size of marker in meters(?)
    const double MARKER_LENGTH = 0.179;
    
    // distance between markers
    const int MARKER_SEPARATION = 1;

    bool calibrated = false;
      
    //Every Calibration() object has these 
	
    // dict is the set of aruco markers that we are expecting to 
    cv::Ptr<cv::aruco::Dictionary> dict;
	
    // marker corners
    vector<vector<cv::Point2f>> corners;
	
    // marker ids 
    vector<int> ids;

    // camera device
    libfreenect2::Freenect2Device *device;

    // output rotation and translation vectors
    cv::Mat rotation;
    cv::Mat translation;
    cv::Mat transformation;

public:
    Calibration(libfreenect2::Freenect2Device*);
    Calibration(); //Don't use default constructor for instantiation!
    void transformPoints(cv::Mat, cv::Mat);
    void setDevice(libfreenect2::Freenect2Device*);
    bool calibrate(cv::Mat);
    vector<double> transformPoint(double, double, double);
    void detectMarkers(cv::Mat*);

};

#endif
