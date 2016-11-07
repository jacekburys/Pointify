#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <chrono>
#include <thread>
#include <iostream>
#include <iomanip>
#include <time.h>
#include <signal.h>

#include <cmdline.h>
#include <cmdlog.h>
#include <json.hpp>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <socketio/sio_client.h>

#include <libfreenect2/libfreenect2.hpp>
#include <libfreenect2/frame_listener_impl.h>
#include <libfreenect2/registration.h>
#include <libfreenect2/packet_pipeline.h>
#include <libfreenect2/logger.h>

typedef unsigned char byte;

byte* matToBytes(cv::Mat image)
{
    int size = image.rows*image.cols;
    byte* bytes = (byte*) malloc(size);
    std::memcpy(bytes,image.data,size * sizeof(byte));
    return bytes;
}

byte* startCamera()
{
    libfreenect2::Freenect2 freenect2;
    libfreenect2::Freenect2Device *dev = 0;

    if(freenect2.enumerateDevices() == 0)
    {
        std::cout << "no device connected!" << std::endl;
        throw;
    }

    std::string serial = freenect2.getDefaultDeviceSerialNumber();

    std::cout << "SERIAL: " << serial << std::endl;
    dev = freenect2.openDevice(serial);

    if(dev == 0)
        {
            std::cout << "failure opening device!" << std::endl;
            throw;
        }

        //! [listeners]
        libfreenect2::SyncMultiFrameListener listener(libfreenect2::Frame::Color |
                                                      libfreenect2::Frame::Depth |
                                                      libfreenect2::Frame::Ir);
        libfreenect2::FrameMap frames;

        dev->setColorFrameListener(&listener);
        dev->setIrAndDepthFrameListener(&listener);
        //! [listeners]

        //! [start]
        dev->start();

        std::cout << "device serial: " << dev->getSerialNumber() << std::endl;
        std::cout << "device firmware: " << dev->getFirmwareVersion() << std::endl;
        //! [start]

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
        } catch (std::exception& t) {
            shutdown = true;
        }

        listener.release(frames);
    }

    dev->stop();
    dev->close();

    return 0;
}

int main(int argc, char *argv[])
{
    using namespace std::chrono_literals;

    cmdline::parser cmdParser;
    cmdParser.add<std::string>("ip", 'i', "Server IP address", false, "127.0.0.1");
    cmdParser.add<unsigned short>("port", 'p', "Server port number", false, 9000);
    cmdParser.parse_check(argc, argv);
    const char* inputIp = cmdParser.get<std::string>("ip").c_str();
    unsigned short inputPort = cmdParser.get<unsigned short>("port");
    char serverUrl[100];
    sprintf(serverUrl, "http://%s:%hu", inputIp, inputPort);
    INFO("Trying to connect to %s", serverUrl);

    sio::client client;
    client.set_open_listener([] { INFO("Connected"); });
    client.set_fail_listener([] { INFO("Failed"); });
    client.set_close_listener([] (sio::client::close_reason const& reason) { INFO("Closed"); });

    client.set_socket_open_listener( [&] (std::string const& nsp)
                                     {
                                     INFO("Socket connected");
                                     nlohmann::json msg;
                                     msg["values"] = { 1.0, 2.0, 3.0 };
                                     client.socket()->emit("new_frame", msg.dump());
                                     });
    client.socket()->on("hello", [] (sio::event& event) { INFO("Hello from server"); });

    client.connect(serverUrl);

    startCamera();
/*
    Camera camera;
    camera.getFrame();

    client.socket()->on("send_frame", [] (sio::event& event) {
                                            
                                          ); });
  */
    return 0;
}