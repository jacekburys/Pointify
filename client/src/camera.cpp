#include "camera.h"
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <socketio/sio_client.h>
#include <cmdlog.h>

#include <iostream>
#include <thread>

typedef unsigned char byte;
bool pictureTriggered = false;
bool calibrationTriggered = false;
bool calibrationSuccess;
sio::array_message::ptr triggeredPicture;

string Camera::serializeMatrix(cv::Mat image)
{
    ostringstream stream;
    stream << image;
    return stream.str();
}

sio::array_message::ptr Camera::getPointCloud(libfreenect2::Registration* registration, libfreenect2::Frame& undistorted, libfreenect2::Frame& registered) {
    sio::message::list result;
    int rows = undistorted.height;
    int cols = undistorted.width;
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {
            float x, y, z, rgb;
            double dx, dy, dz;

            registration->getPointXYZRGB(&undistorted, &registered, i, j, x, y, z, rgb);

            dx = static_cast<double>(x);
            dy = static_cast<double>(y);
            dz = static_cast<double>(z);

            const uint8_t *p = reinterpret_cast<uint8_t*>(&rgb);
            uint8_t b = p[0];
            uint8_t g = p[1];
            uint8_t r = p[2];

            if(r > 0 || g > 0 || b > 0) {
                sio::message::ptr ptr_x = sio::double_message::create(dx);
                sio::message::ptr ptr_y = sio::double_message::create(dy);
                sio::message::ptr ptr_z = sio::double_message::create(dz);
                sio::message::ptr ptr_r = sio::int_message::create(r);
                sio::message::ptr ptr_g = sio::int_message::create(g);
                sio::message::ptr ptr_b = sio::int_message::create(b);

                result.push(ptr_x);
                result.push(ptr_y);
                result.push(ptr_z);
                result.push(ptr_r);
                result.push(ptr_g);
                result.push(ptr_b);
            }
        }
    }

    return result.to_array_message();
}

void Camera::start()
{
    libfreenect2::Freenect2 freenect2;
    libfreenect2::setGlobalLogger(libfreenect2::createConsoleLogger(libfreenect2::Logger::None));
    libfreenect2::SyncMultiFrameListener listener(libfreenect2::Frame::Color |
                                                  libfreenect2::Frame::Depth |
                                                  libfreenect2::Frame::Ir);
    libfreenect2::FrameMap frames;
    libfreenect2::Freenect2Device *dev = 0;

    if(freenect2.enumerateDevices() == 0)
    {
        INFO("No device connected");
        throw;
    }

    string serial = freenect2.getDefaultDeviceSerialNumber();

    INFO("SERIAL: %s", serial.c_str());
    dev = freenect2.openDevice(serial);

    if(dev == 0)
    {
        cout << "failure opening device!" << endl;
        throw;
    }


    dev->setColorFrameListener(&listener);
    dev->setIrAndDepthFrameListener(&listener);

    dev->start();
    Calibration calibration(dev);

    char serialNumber[100];
    char firmwareVersion[100];
    sprintf(serialNumber, "%s", dev->getSerialNumber().c_str());
    sprintf(firmwareVersion, "%s", dev->getFirmwareVersion().c_str());
    INFO("Device serial: %s", serialNumber);
    INFO("Device firmware: %s", firmwareVersion);

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
        cv::cvtColor(rgbmat, rgbmat, CV_RGBA2RGB);

        registration->apply(rgb, depth, &undistorted, &registered, true, &depth2rgb);

        if (pictureTriggered) {
            triggeredPicture = getPointCloud(registration, undistorted, registered);
            pictureTriggered = false;
        }

        if (calibrationTriggered) {
            calibrationSuccess = calibration.calibrate(rgbmat);
            calibrationTriggered = false;
        }

        registration->apply(rgb, depth, &undistorted, &registered, true, &depth2rgb);

        cv::Mat(undistorted.height, undistorted.width, CV_32FC1, undistorted.data).copyTo(depthmatUndistorted);
        cv::Mat(registered.height, registered.width, CV_8UC4, registered.data).copyTo(rgbd);
        cv::Mat(depth2rgb.height, depth2rgb.width, CV_32FC1, depth2rgb.data).copyTo(rgbd2);
        calibration.detectMarkers(&rgbmat);
        cv::imshow("Camera", rgbmat);

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

bool Camera::calibrate() {
    calibrationTriggered = true;
    while (calibrationTriggered) {}
    return calibrationSuccess;
}
