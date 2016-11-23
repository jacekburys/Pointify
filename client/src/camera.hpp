#ifndef POINTIFY_CAMERA_H
#define POINTIFY_CAMERA_H

#include <string>
#include <mutex>
#include <thread>
#include <condition_variable>

#include <opencv2/opencv.hpp>
#include <socketio/sio_client.h>
#include <libfreenect2/libfreenect2.hpp>
#include <libfreenect2/frame_listener_impl.h>
#include <libfreenect2/registration.h>
#include <libfreenect2/packet_pipeline.h>
#include <libfreenect2/logger.h>

#include "calibration.hpp"

using namespace std;

class Camera
{
    public:
        Camera() {};
        void start();
        sio::array_message::ptr takePicture();
        bool calibrate();
    private:
        sio::array_message::ptr getPointCloud(libfreenect2::Registration* registration,
                                              libfreenect2::Frame& undistorted,
                                              libfreenect2::Frame& registered);
        char* Camera::getPointCloudStream(libfreenect2::Registration* registration,
                                              libfreenect2::Frame& undistorted,
                                              libfreenect2::Frame& registered);
            string serializeMatrix(cv::Mat image);
        void cameraLoop();
        void sendPicture(cv::Mat image);

        int CALIBRATION_TIMEOUT = 3; // seconds
        int TAKEPICTURE_TIMEOUT = 3; // seconds

        bool pictureFinished = true;
        bool pictureTriggered = false;
        mutex pictureMutex;
        condition_variable pictureCv;
        sio::array_message::ptr capturedPicture;

        bool calibrationFinished = true;
        bool calibrationTriggered = false;
        mutex calibrationMutex;
        condition_variable calibrationCv;
        bool calibrationSuccess;

        Calibration calibration;
};

#endif
