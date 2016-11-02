#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>
#include <chrono>
#include <thread>

#include <cmdline.h>
#include <cmdlog.h>
#include <socketio/sio_client.h>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

int main(int argc, char *argv[])
{
    using namespace std::chrono_literals;

    cmdline::parser cmdParser;
    cmdParser.add<std::string>("ip", 'i', "Server IP address", false, "129.31.228.253");
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
    client.set_socket_open_listener( [] (std::string const& nsp) { INFO("Socket connected"); });
    client.socket()->on("hello", [] (sio::event& event) { INFO("Msg received"); });

    client.connect(serverUrl);
}

