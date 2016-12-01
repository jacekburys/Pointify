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

    cmdline::parser cmdParser;
    cmdParser.add<string>("ip", 'i', "Server IP address", false, "127.0.0.1");
    cmdParser.add<unsigned short>("port", 'p', "Server port number", false, 9000);
    cmdParser.parse_check(argc, argv);
    const char* inputIp = cmdParser.get<string>("ip").c_str();
    unsigned short inputPort = cmdParser.get<unsigned short>("port");
    char serverUrl[100];
    map<string, string> query;
    query["clientID"] = to_string(rand() % 1000000000);
    sprintf(serverUrl, "http://%s:%hu", inputIp, inputPort);
    INFO("Trying to connect to %s", serverUrl);

    vector<sio::client*> clients;
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

    client.connect(serverUrl, query);
    clients.push_back(&client);

//    for (int i = 0; i < 1; i++)
//    {
//        sio::client newClient;
//        newClient.connect(serverUrl, query);
//        clients.push_back(&newClient);
//    }

    sio::client newClient1;
    newClient1.connect(serverUrl, query);
    clients.push_back(&newClient1);

    sio::client newClient2;
    newClient2.connect(serverUrl, query);
    clients.push_back(&newClient2);

    sio::client newClient3;
    newClient3.connect(serverUrl, query);
    clients.push_back(&newClient3);

    Camera camera(&client);
    client.socket()->on("take_picture",
                        [&camera, &client] (sio::event& event)
                        {
                            INFO("taking picture");
                            string buffString = camera.takePicture();
                            INFO("picture taken");
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
                        [&camera, &client, &clients] (sio::event& event)
                        {
                            INFO("started streaming");
                            camera.startStreaming(clients);
                        });

    INFO("reached");
    camera.start();

    return 0;
}

