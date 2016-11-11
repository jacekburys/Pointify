#ifndef POINTIFY_CAMERA_H
#define POINTIFY_CAMERA_H

using namespace std;

#include <string>
#include <opencv2/opencv.hpp>
#include <socketio/sio_client.h>

class Camera
{
public:
    Camera() {};
    static void start();
    static sio::array_message::ptr takePicture();
private:
    static sio::array_message::ptr getMessage(cv::Mat image);
    static string serializeMatrix(cv::Mat image);
    static void cameraLoop();
    static void sendPicture(cv::Mat image);
};

#endif
