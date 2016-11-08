using namespace std;

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
#include <json.hpp>
#include <socketio/sio_client.h>

#include "camera.h"

int main(int argc, char *argv[])
{

    cmdline::parser cmdParser;
    cmdParser.add<string>("ip", 'i', "Server IP address", false, "127.0.0.1");
    cmdParser.add<unsigned short>("port", 'p', "Server port number", false, 9000);
    cmdParser.parse_check(argc, argv);
    const char* inputIp = cmdParser.get<string>("ip").c_str();
    unsigned short inputPort = cmdParser.get<unsigned short>("port");
    char serverUrl[100];
    sprintf(serverUrl, "http://%s:%hu", inputIp, inputPort);
    INFO("Trying to connect to %s", serverUrl);

    sio::client client;
    client.set_open_listener([] { INFO("Connected"); });
    client.set_fail_listener([] { INFO("Failed"); });
    client.set_close_listener([] (sio::client::close_reason const& reason) { INFO("Closed"); });

    client.set_socket_open_listener( [&] (string const& nsp)
                                     {
                                     INFO("Socket connected");
                                     nlohmann::json msg;
                                     msg["values"] = { 1.0, 2.0, 3.0 };
                                     client.socket()->emit("new_frame", msg.dump());
                                     });
    client.socket()->on("hello", [] (sio::event& event) { INFO("Hello from server"); });

    client.connect(serverUrl);

    Camera camera;
    client.socket()->on("takepicture",  [&camera] (sio::event& event) { cout << camera.takePicture() << endl << endl; });
    camera.start();


    return 0;
}