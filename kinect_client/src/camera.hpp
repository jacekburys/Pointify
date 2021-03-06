#ifndef POINTIFY_CAMERA_H
#define POINTIFY_CAMERA_H

#include <string>
#include <mutex>
#include <thread>
#include <memory>
#include <iostream>
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
    Camera(sio::client* client);
    void start();
string takePicture();
    bool calibrate();
    void startStreaming();
    void stopStreaming();
private:
    bool streaming = false;
    string latestFrameTaken = "";
    static void streamFramesWrapper(Camera* camera);
    sio::client* client;
    string getPointCloudStream();
    void streamFrames();
    void streamFrame();
    string serializeMatrix(cv::Mat image);
    void cameraLoop();
    void sendPicture(cv::Mat image);

    int DEPTH_WIDTH = 512;
    int DEPTH_HEIGHT = 424;
    int DEPTH_CHANNELS = 4;
    int COLOR_WIDTH = 1920;
    int COLOR_HEIGHT = 1080;
    int COLOR_CHANNELS = 4;

    int CALIBRATION_TIMEOUT = 3; // seconds
    int TAKEPICTURE_TIMEOUT = 3; // seconds
    bool streamTriggered = false;
    bool pictureFinished = true;
    bool pictureTriggered = false;
    libfreenect2::Registration* registration;
    libfreenect2::Frame* undistorted;
    libfreenect2::Frame* registered;
    mutex pictureMutex;
    condition_variable pictureCv;
    string capturedPicture;
    bool calibrationFinished = true;
    bool calibrationTriggered = false;
    mutex calibrationMutex;
    condition_variable calibrationCv;
    bool calibrationSuccess;
    int framesEmitted = 0;

    Calibration calibration;
};

#endif
