#include <unistd.h>
#include "socket.hpp"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>

int main(int argc, char *argv[])
{
    cmdline::parser cmdParser;
    cmdParser.add<std::string>("ip", 'i', "Server IP address", false, "127.0.0.1");
    cmdParser.add<unsigned short>("port", 'p', "Server port number", false, 4500);
    cmdParser.parse_check(argc, argv);

    const char* parsedIp = cmdParser.get<std::string>("ip").c_str();
    unsigned short parsedPort = cmdParser.get<unsigned short>("port");

    Socket* socket = new Socket(parsedIp, parsedPort);
    socket->start();
}

