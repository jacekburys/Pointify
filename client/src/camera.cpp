#include "camera.h"
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <socketio/sio_client.h>
#include <cmdlog.h>

#include <libfreenect2/libfreenect2.hpp>
#include <libfreenect2/frame_listener_impl.h>
#include <libfreenect2/registration.h>
#include <libfreenect2/packet_pipeline.h>
#include <libfreenect2/logger.h>
#include <iostream>
#include <thread>

typedef unsigned char byte;
bool pictureTriggered = false;
sio::array_message::ptr triggeredPicture;

string Camera::serializeMatrix(cv::Mat image)
{
    ostringstream stream;
    stream << image;
    return stream.str();
}

sio::array_message::ptr Camera::getMessage(cv::Mat image) {
    sio::message::list result;
    int rows = image.rows;
    int cols = image.cols;
    for (int i = 0; i < rows; i++) {
        cv::Vec3b* vector = image.ptr<cv::Vec3b>(i);
        for (int j = 0; j < cols; j++) {
            char r = vector[j][0];
            char g = vector[j][1];
            char b = vector[j][2];
            int point = 0;
            point |= (r << 16);
            point |= (g << 8);
            point |= (b << 0);
            sio::message::ptr ptr_point = sio::int_message::create(point);
            result.push(ptr_point);
        }
    }
    return result.to_array_message();
}

void Camera::start()
{
    libfreenect2::Freenect2 freenect2;
    libfreenect2::SyncMultiFrameListener listener(libfreenect2::Frame::Color |
                                                  libfreenect2::Frame::Depth |
                                                  libfreenect2::Frame::Ir);
    libfreenect2::FrameMap frames;
    libfreenect2::Freenect2Device *dev = 0;

    if(freenect2.enumerateDevices() == 0)
    {
        cout << "no device connected!" << endl;
        throw;
    }

    string serial = freenect2.getDefaultDeviceSerialNumber();

    cout << "SERIAL: " << serial << endl;
    dev = freenect2.openDevice(serial);

    if(dev == 0)
    {
        cout << "failure opening device!" << endl;
        throw;
    }

    libfreenect2::setGlobalLogger(libfreenect2::createConsoleLogger(libfreenect2::Logger::None));

    dev->setColorFrameListener(&listener);
    dev->setIrAndDepthFrameListener(&listener);

    dev->start();

    cout << "device serial: " << dev->getSerialNumber() << endl;
    cout << "device firmware: " << dev->getFirmwareVersion() << endl;

    cv::namedWindow("Camera", CV_WINDOW_NORMAL);
    cv::resizeWindow("Camera", 1500, 844);

    bool shutdown = false;
    cv::Mat rgbmat, depthmat, depthmatUndistorted, irmat, rgbd, rgbd2;
    libfreenect2::Registration* registration = new libfreenect2::Registration(dev->getIrCameraParams(), dev->getColorCameraParams());
    libfreenect2::Frame undistorted(512, 424, 4), registered(512, 424, 4), depth2rgb(1920, 1080 + 2, 4);

    while (!shutdown) {
        listener.waitForNewFrame(frames);
        libfreenect2::Frame *rgb = frames[libfreenect2::Frame::Color];
        libfreenect2::Frame *ir = frames[libfreenect2::Frame::Ir];
        libfreenect2::Frame *depth = frames[libfreenect2::Frame::Depth];

        cv::Mat(rgb->height, rgb->width, CV_8UC4, rgb->data).copyTo(rgbmat);
        cv::Mat(ir->height, ir->width, CV_32FC1, ir->data).copyTo(irmat);
        cv::Mat(depth->height, depth->width, CV_32FC1, depth->data).copyTo(depthmat);

        if (pictureTriggered) {
            triggeredPicture = getMessage(rgbmat);
            pictureTriggered = false;
        }

        cv::imshow("Camera", depthmat);

        registration->apply(rgb, depth, &undistorted, &registered, true, &depth2rgb);

        cv::Mat(undistorted.height, undistorted.width, CV_32FC1, undistorted.data).copyTo(depthmatUndistorted);
        cv::Mat(registered.height, registered.width, CV_8UC4, registered.data).copyTo(rgbd);
        cv::Mat(depth2rgb.height, depth2rgb.width, CV_32FC1, depth2rgb.data).copyTo(rgbd2);

        int key = cv::waitKey(1);

        // Shutdown on escape
        shutdown = shutdown || (key > 0 && ((key & 0xFF) == 27));

        // Shutdown on window close
        try {
            int windowProperty = cv::getWindowProperty("Camera", 0);
        } catch (cv::Exception e) {
            shutdown = true;
        }

        listener.release(frames);
    }
    dev->stop();
    dev->close();
}

sio::array_message::ptr Camera::takePicture() {
    pictureTriggered = true;
    while (pictureTriggered) {}
    return triggeredPicture;
}
