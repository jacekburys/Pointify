#ifndef POINTIFY_CAMERA_H
#define POINTIFY_CAMERA_H

using namespace std;

#include <string>
#include <opencv2/opencv.hpp>
#include "calibration.hpp"
#include <socketio/sio_client.h>
#include <libfreenect2/libfreenect2.hpp>
#include <libfreenect2/frame_listener_impl.h>
#include <libfreenect2/registration.h>
#include <libfreenect2/packet_pipeline.h>
#include <libfreenect2/logger.h>

class Camera
{
public:
    Camera() {};
    static void start();
    struct ColorCameraParams* getColorCameraParams();
    static sio::array_message::ptr takePicture();
    static bool calibrate();
private:
    static sio::array_message::ptr getPointCloud(libfreenect2::Registration* registration,
                                                 libfreenect2::Frame& undistorted,
                                                 libfreenect2::Frame& registered);
    static string serializeMatrix(cv::Mat image);
    static void cameraLoop();
    static void sendPicture(cv::Mat image);
};

#endif
