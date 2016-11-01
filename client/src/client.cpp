#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>

#include <cmdline.h>
#include <cmdlog.h>
#include <socketio/sio_client.h>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

int main(int argc, char *argv[])
{
    cmdline::parser cmdParser;
    cmdParser.add<std::string>("ip", 'i', "Server IP address", false, "127.0.0.1");
    cmdParser.add<unsigned short>("port", 'p', "Server port number", false, 4500);
    cmdParser.parse_check(argc, argv);
    const char* inputIp = cmdParser.get<std::string>("ip").c_str();
    unsigned short inputPort = cmdParser.get<unsigned short>("port");
    char serverUrl[100];
    sprintf(serverUrl, "http://%s:%hu", inputIp, inputPort);

    cv::Mat image = cv::imread(std::string("./image.jpg"));
    cv::imshow("image", image);
    cv::waitKey(0);

    sio::client client;
    client.connect(serverUrl);
    client.socket()->emit("test", std::string("test"));
}

