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
    // size of marker in meters
    float MARKER_LENGTH = 0.287;

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
    
    // camera properties
    cv::Mat cameraMatrix;
    cv::Mat distCoeffs;

    // output rotation and translation vectors
    cv::Vec3d rvec;
    cv::Vec3d tvec;
    cv::Mat transformation;

public:
    Calibration(libfreenect2::Freenect2Device*);
    Calibration(); //Don't use default constructor for instantiation!
    void transformPoints(cv::Mat, cv::Mat);
    void setDevice(libfreenect2::Freenect2Device*);
    bool calibrate(cv::Mat);
    vector<float> transformPoint(float, float, float);
    int detectMarkers(cv::Mat*);

};

#endif
