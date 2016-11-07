#ifndef POINTIFY_CAMERA_H
#define POINTIFY_CAMERA_H

#include <string>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <socketio/sio_client.h>
#include <cmdlog.h>

#include <libfreenect2/libfreenect2.hpp>
#include <libfreenect2/frame_listener_impl.h>
#include <libfreenect2/registration.h>
#include <libfreenect2/packet_pipeline.h>
#include <libfreenect2/logger.h>

using namespace std;
using namespace cv;

class Camera
{
public:
    Camera() {};
    void start();
    void takePicture();
private:
    string matrixToJSON();
    void cameraLoop();
    void sendPicture(cv::Mat image);
};

#endif
