#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <chrono>
#include <thread>
#include <iostream>
#include <iomanip>
#include <time.h>
#include <signal.h>

#include <cmdline.h>
#include <cmdlog.h>
#include <socketio/sio_client.h>

#include "camera.hpp"

using namespace std;

int main(int argc, char *argv[])
{
    // handle parameters
    cmdline::parser cmdParser;
    cmdParser.add<string>("ip", 'i', "Server IP address", false, "127.0.0.1");
    cmdParser.add<unsigned short>("port", 'p', "Server port number", false, 9000);
    cmdParser.parse_check(argc, argv);
    const char* inputIp = cmdParser.get<string>("ip").c_str();
    unsigned short inputPort = cmdParser.get<unsigned short>("port");

    // connect to server
    char serverUrl[100];
    sprintf(serverUrl, "http://%s:%hu", inputIp, inputPort);
    INFO("Trying to connect to %s", serverUrl);

    sio::client client;
    client.set_open_listener([] ()
                             {
                                 INFO("Connected");
                             });
    client.set_fail_listener([] ()
                             {
                                 INFO("Failed");
                             });
    client.set_close_listener([] (sio::client::close_reason const& reason)
                              {
                                  INFO("Closed");
                              });
    client.set_socket_open_listener([&] (string const& nsp)
                                    {
                                        INFO("Socket connected");
                                    });
    client.connect(serverUrl);

    // init camera
    Camera camera(&client);

    // add camera event listeners
    client.socket()->on("take_picture",
                        [&camera, &client] (sio::event& event)
                        {
                            string buffString = camera.takePicture();
                            client.socket()->emit("new_frame", make_shared<string>(buffString.c_str(), buffString.size()));
                            INFO("picture sent");
                        });
    client.socket()->on("calibrate",
                        [&camera, &client] (sio::event& event)
                        {
                            INFO("got calibrate message");
                            client.socket()->emit("calibration_status", sio::bool_message::create(camera.calibrate()));
                        });
    client.socket()->on("start_streaming",
                        [&camera, &client] (sio::event& event)
                        {
                            camera.startStreaming();
                        });
    client.socket()->on("stop_streaming",
                        [&camera, &client] (sio::event& event)
                        {
                            camera.stopStreaming();
                        });

    // start camera display
    camera.start();

    return 0;
}

