#include <iostream>
#include <functional>
#include <thread>
#include <chrono>
#include <future>
#include <mutex>
#include <condition_variable>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <socketio/sio_client.h>
#include <cmdlog.h>

#include "camera.hpp"

typedef unsigned char byte;

using namespace std;

string Camera::serializeMatrix(cv::Mat image)
{
    ostringstream stream;
    stream << image;
    return stream.str();
}

sio::array_message::ptr Camera::getPointCloud(libfreenect2::Registration* registration,
                                              libfreenect2::Frame& undistorted,
                                              libfreenect2::Frame& registered)
{
    sio::message::list result;
    int rows = undistorted.height;
    int cols = undistorted.width;
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
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

            if (r > 0 || g > 0 || b > 0)
            {
                vector<double> pt = calibration.transformPoint(dx, dy, dz); // transform point for calibration
                sio::message::ptr ptr_x = sio::double_message::create(pt[0]);
                sio::message::ptr ptr_y = sio::double_message::create(pt[1]);
                sio::message::ptr ptr_z = sio::double_message::create(pt[2]);
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

char* Camera::getPointCloudStream(libfreenect2::Registration* registration,
                                              libfreenect2::Frame& undistorted,
                                              libfreenect2::Frame& registered)
{
    int rows = undistorted.height;
    int cols = undistorted.width;
    char* buffer = new char[rows * cols * 27];
    // make this frame invariant
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
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

            if (r > 0 || g > 0 || b > 0)
            {
                vectoir<double> pt = calibration.transformPoint(dx, dy, dz); // transform point for calibration
                int index = 27 * (i * cols + j);
                buffer[index] = r;
                buffer[index + 1] = g;
                buffer[index + 2] = b;
                *reinterpret_cast<double*>(buffer + (index + 3)) = pt[0];
                *reinterpret_cast<double*>(buffer + (index + 11)) = pt[1];
                *reinterpret_cast<double*>(buffer + (index + 19)) = pt[2];
            }
        }
    }

    return buffer;
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
    calibration.setDevice(dev);

    char serialNumber[100];
    char firmwareVersion[100];
    sprintf(serialNumber, "%s", dev->getSerialNumber().c_str());
    sprintf(firmwareVersion, "%s", dev->getFirmwareVersion().c_str());
    INFO("Device serial: %s", serialNumber);
    INFO("Device firmware: %s", firmwareVersion);

    cv::namedWindow("Camera", CV_WINDOW_NORMAL);
    cv::resizeWindow("Camera", 1500, 844);

    bool shutdown = false;
    cv::Mat rgbmat, depthmat, depthmatUndistorted, irmat, rgbd, rgbd2, registeredmat;
    libfreenect2::Registration* registration = new libfreenect2::Registration(dev->getIrCameraParams(), dev->getColorCameraParams());
    libfreenect2::Frame undistorted(512, 424, 4), registered(512, 424, 4), depth2rgb(1920, 1080 + 2, 4);

    while (!shutdown)
    {
        listener.waitForNewFrame(frames);
        libfreenect2::Frame *rgb = frames[libfreenect2::Frame::Color];
        libfreenect2::Frame *ir = frames[libfreenect2::Frame::Ir];
        libfreenect2::Frame *depth = frames[libfreenect2::Frame::Depth];

        cv::Mat(rgb->height, rgb->width, CV_8UC4, rgb->data).copyTo(rgbmat);
        cv::Mat(ir->height, ir->width, CV_32FC1, ir->data).copyTo(irmat);
        cv::Mat(depth->height, depth->width, CV_32FC1, depth->data).copyTo(depthmat);
        cv::cvtColor(rgbmat, rgbmat, CV_RGBA2RGB);

        registration->apply(rgb, depth, &undistorted, &registered, true, &depth2rgb);

        cv::Mat(undistorted.height, undistorted.width, CV_32FC1, undistorted.data).copyTo(depthmatUndistorted);
        cv::Mat(registered.height, registered.width, CV_8UC4, registered.data).copyTo(rgbd);
        cv::Mat(depth2rgb.height, depth2rgb.width, CV_32FC1, depth2rgb.data).copyTo(rgbd2);
        cv::cvtColor(rgbd, rgbd, CV_RGBA2RGB);

        if (!pictureFinished)
        {
            static future<sio::array_message::ptr> future;
            if (!pictureTriggered)
            {
                future = async(launch::async,
                               &Camera::getPointCloud, this, registration, ref(undistorted), ref(registered));
                pictureTriggered = true;
            }
            if (future.wait_for(chrono::seconds(TAKEPICTURE_TIMEOUT)) == future_status::ready)
            {
                capturedPicture = future.get();
                pictureFinished = true;
                pictureTriggered = false;
                pictureCv.notify_one();
            }
        }

        if (!calibrationFinished)
        {
            static future<bool> future;
            if (!calibrationTriggered)
            {
                future = async(launch::async,
                               &Calibration::calibrate, &calibration, rgbd);
                calibrationTriggered = true;
            }
            if (future.wait_for(chrono::seconds(CALIBRATION_TIMEOUT)) == future_status::ready)
            {
                calibrationSuccess = future.get();
                calibrationFinished = true;
                calibrationFinished = false;
                calibrationCv.notify_one();
            }
        }

        calibration.detectMarkers(&rgbmat);
        cv::imshow("Camera", rgbmat);

        int key = cv::waitKey(1);

        /// Shutdown on escape
        shutdown = shutdown || (key > 0 && ((key & 0xFF) == 27));

        /// Shutdown on window close
        try
        {
            int windowProperty = cv::getWindowProperty("Camera", 0);
        }
        catch (cv::Exception e)
        {
            shutdown = true;
        }

        listener.release(frames);
    }
    dev->stop();
    dev->close();
}

sio::array_message::ptr Camera::takePicture()
{
    unique_lock<mutex> lock(pictureMutex);
    pictureFinished = false;
    pictureCv.wait(lock, [this] { return pictureFinished; });
    return capturedPicture;
}

bool Camera::calibrate()
{
    unique_lock<mutex> lock(calibrationMutex);
    calibrationFinished = false;
    calibrationCv.wait(lock, [this] { return calibrationFinished; });
    return calibrationSuccess;
}

