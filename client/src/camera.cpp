#include <iostream>
#include <functional>
#include <thread>
#include <chrono>
#include <future>
#include <mutex>
#include <condition_variable>
#include <unistd.h>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <socketio/sio_client.h>
#include <cmdlog.h>
#include <queue>
#include <chrono>

#include "camera.hpp"

typedef unsigned char byte;
using namespace std;
using namespace std::chrono;

Camera::Camera(sio::client* client) {
    this->client = client;
}

void Camera::streamFramesWrapper(Camera* camera) {
    camera->streamFrames();
}

void Camera::streamFrames() {
    while (true)
        streamFrame();
}

void Camera::streamFrame()
{
    string buffString = getPointCloudStream();
    client->socket()->emit("new_frame", make_shared<string>(buffString.c_str(), buffString.size()));
    usleep(500000);
    framesEmitted++;
    INFO("%i", framesEmitted);
}

string Camera::getPointCloudStream()
{
    int rows = undistorted->height;
    int cols = undistorted->width;

    // build up xyz and rgb matrices
    cv::Mat xyzmat(rows, cols, CV_32FC3);
    cv::Mat rgbmat(rows, cols, CV_8UC3);
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            float x, y, z, rgb;
            registration->getPointXYZRGB(undistorted, registered, i, j, x, y, z, rgb);
            const uint8_t *rgbp = reinterpret_cast<uint8_t*>(&rgb);

            rgbmat.at<cv::Vec3b>(i, j) = cv::Vec3b({rgbp[2], rgbp[1], rgbp[0]});
            xyzmat.at<cv::Vec3f>(i, j) = cv::Vec3f({x, y, z});
        }
    }

    // perform transformation to bring the marker to [0,0,0]
    calibration.transformPoints(xyzmat, xyzmat);

    // convert these points to a string message
    std::ostringstream buffer;
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            cv::Vec3f xyz = xyzmat.at<cv::Vec3f>(i, j);
            float x = xyz[0];
            float y = xyz[1];
            float z = xyz[2];

            cv::Vec3b rgb = rgbmat.at<cv::Vec3b>(i, j);
            uint8_t r = rgb[0];
            uint8_t g = rgb[1];
            uint8_t b = rgb[2];

            if (r > 0 || g > 0 || b > 0)
            {
                buffer << char(r);
                buffer << char(g);
                buffer << char(b);
                uint8_t* x2 = reinterpret_cast<uint8_t*>(&x);
                buffer << x2[0];
                buffer << x2[1];
                buffer << x2[2];
                buffer << x2[3];
                uint8_t* y2 = reinterpret_cast<uint8_t*>(&y);
                buffer << y2[0];
                buffer << y2[1];
                buffer << y2[2];
                buffer << y2[3];
                uint8_t* z2 = reinterpret_cast<uint8_t*>(&z);
                buffer << z2[0];
                buffer << z2[1];
                buffer << z2[2];
                buffer << z2[3];
            }
        }
    }

    string buffString = buffer.str();

    return buffString;
}

void Camera::start()
{
    timeval tim;
    int time = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
    // init kinect
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

    dev = freenect2.openDevice(freenect2.getDefaultDeviceSerialNumber());

    if(dev == 0)
    {
        cout << "failure opening device!" << endl;
        throw;
    }

    dev->setColorFrameListener(&listener);
    dev->setIrAndDepthFrameListener(&listener);

    dev->start();
    calibration.setDevice(dev);

    // set up viewport
    cv::namedWindow("Camera", CV_WINDOW_NORMAL);
    cv::resizeWindow("Camera", DEPTH_WIDTH, DEPTH_HEIGHT);

    bool shutdown = false;
    cv::Mat rgbmat, depthmat, rgbd, registeredmat;
    registration = new libfreenect2::Registration(dev->getIrCameraParams(), dev->getColorCameraParams());
    libfreenect2::Frame undistortedLocal(DEPTH_WIDTH, DEPTH_HEIGHT, DEPTH_CHANNELS), 
                        registeredLocal(DEPTH_WIDTH, DEPTH_HEIGHT, DEPTH_CHANNELS), 
                        depth2rgb(COLOR_WIDTH, COLOR_HEIGHT + 2, COLOR_CHANNELS);
    undistorted = &undistortedLocal;
    registered = &registeredLocal;
    int numberOfMarkers = 0;

    while (!shutdown)
    {
        // block until new frame arrives 
        listener.waitForNewFrame(frames);
        libfreenect2::Frame *rgb = frames[libfreenect2::Frame::Color];
        libfreenect2::Frame *depth = frames[libfreenect2::Frame::Depth];

        cv::Mat(rgb->height, rgb->width, CV_8UC4, rgb->data).copyTo(rgbmat);
        cv::Mat(depth->height, depth->width, CV_32FC1, depth->data).copyTo(depthmat);

        // generate color-depth frame, and convert to matrix
        registration->apply(rgb, depth, undistorted, registered, true, &depth2rgb);
        cv::Mat(registered->height, registered->width, CV_8UC4, registered->data).copyTo(rgbd);

        cv::cvtColor(rgbd, rgbd, CV_RGBA2RGB);

        // handle picture signal
        if (!pictureFinished)
        {
            static future<string> future;
            if (!pictureTriggered)
            {
                future = async(launch::async, &Camera::getPointCloudStream, this);
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

        latestFrameTaken = getPointCloudStream();
        int newtime = duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
        int framerate = 1000 / (newtime - time);
        time = newtime;

        // handle calibration signal
        if (!calibrationFinished)
        {
            static future<bool> future;
            if (!calibrationTriggered)
            {
                future = async(launch::async, &Calibration::calibrate, &calibration, rgbd);
                calibrationTriggered = true;
            }
            if (future.wait_for(chrono::seconds(CALIBRATION_TIMEOUT)) == future_status::ready)
            {
                calibrationSuccess = future.get();
                calibrationFinished = true;
                calibrationTriggered = false;
                calibrationCv.notify_one();
            }
        }

        if (!streaming) {
            // draw markers/axis over image, then display it
            int currentNumberOfMarkers = calibration.detectMarkers(&rgbd);

            if ((numberOfMarkers == 0 && currentNumberOfMarkers > 0) ||
                (numberOfMarkers == 1 && currentNumberOfMarkers != 1) ||
                (numberOfMarkers > 1 && currentNumberOfMarkers < 2)){
                client->socket()->emit("number_of_markers", sio::int_message::create(currentNumberOfMarkers));
            }
            
            numberOfMarkers = currentNumberOfMarkers;
        }

        cv::imshow("Camera", rgbd);

        int key = cv::waitKey(1);

        // Shutdown on escape
        shutdown = shutdown || (key > 0 && ((key & 0xFF) == 27));

        // Shutdown on window close
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

string Camera::takePicture()
{
    return latestFrameTaken;
}

bool Camera::calibrate()
{
    unique_lock<mutex> lock(calibrationMutex);
    calibrationFinished = false;
    calibrationCv.wait(lock, [this] { return calibrationFinished; });
    return calibrationSuccess;
}

void Camera::startStreaming() {
    streaming = true;
}

void Camera::stopStreaming() {
    streaming = false;
}