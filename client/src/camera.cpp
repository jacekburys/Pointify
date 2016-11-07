#include "camera.h"

typedef unsigned char byte;

string matrixToJSON(cv::Mat image)
{
    return "";
}

void Camera::start(sio::client client)
{
    libfreenect2::Freenect2 freenect2;
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

    libfreenect2::SyncMultiFrameListener listener(libfreenect2::Frame::Color |
                                                  libfreenect2::Frame::Depth |
                                                  libfreenect2::Frame::Ir);
    libfreenect2::FrameMap frames;

    dev->setColorFrameListener(&listener);
    dev->setIrAndDepthFrameListener(&listener);

    dev->start();

    cout << "device serial: " << dev->getSerialNumber() << endl;
    cout << "device firmware: " << dev->getFirmwareVersion() << endl;

    cv::Mat rgbmat, depthmat, depthmatUndistorted, irmat, rgbd, rgbd2;
    bool shutdown = false;
    cv::namedWindow("Camera", CV_WINDOW_NORMAL);
    cv::resizeWindow("Camera", 1500, 844);

    while (!shutdown) {
        listener.waitForNewFrame(frames);
        libfreenect2::Frame *rgb = frames[libfreenect2::Frame::Color];
        libfreenect2::Frame *ir = frames[libfreenect2::Frame::Ir];
        libfreenect2::Frame *depth = frames[libfreenect2::Frame::Depth];

        cv::Mat(rgb->height, rgb->width, CV_8UC4, rgb->data).copyTo(rgbmat);
        cv::Mat(ir->height, ir->width, CV_32FC1, ir->data).copyTo(irmat);
        cv::Mat(depth->height, depth->width, CV_32FC1, depth->data).copyTo(depthmat);

        cv::imshow("Camera", depthmat);

        int key = cv::waitKey(1);

        // Shutdown on escape
        shutdown = shutdown || (key > 0 && ((key & 0xFF) == 27));

        // Shutdown on window close
        try {
            int windowProperty = cv::getWindowProperty("Camera", 0);
        } catch (Exception e) {
            shutdown = true;
        }

        listener.release(frames);
    }

    dev->stop();
    dev->close();
}