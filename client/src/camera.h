#ifndef POINTIFY_CAMERA_H
#define POINTIFY_CAMERA_H

using namespace std;

#include <string>
#include <opencv2/opencv.hpp>

class Camera
{
public:
    Camera() {};
    static void start();
    static string takePicture();
    struct ColorCameraParams* getColorCameraParams();
private:
    static string serializeMatrix(cv::Mat image);
    static void cameraLoop();
    static void sendPicture(cv::Mat image);
};

#endif
