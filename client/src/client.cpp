#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <string>

#include <cmdlog.h>
#include <cmdline.h>

int sendall(int sockfd, char *buffer, int *bufferLen)
{
    int bytesTotal = 0;
    int bytesLeft = *bufferLen;
    int bytesSent;

    while (bytesTotal < *bufferLen)
    {
        bytesSent = send(sockfd, buffer + bytesTotal, bytesLeft, 0);
        if (bytesSent == -1)
        {
            break;
        }
        bytesTotal += bytesSent;
        bytesLeft -= bytesSent;
    }
    *bufferLen = bytesTotal;

    return bytesSent == -1 ? -1 : 0;
}

int main(int argc, char *argv[])
{
    cmdline::parser cmdParser;
    cmdParser.add<std::string>("ip", 'i', "Server IP address", false, "127.0.0.1");
    cmdParser.add<unsigned short>("port", 'p', "Server port number", false, 4500);
    cmdParser.parse_check(argc, argv);

    sockaddr_in serverAddress = {0};
    serverAddress.sin_family = AF_INET;
    const char* parsedIp = cmdParser.get<std::string>("ip").c_str();
    if (!inet_pton(AF_INET, parsedIp, &serverAddress.sin_addr))
    {
        FATAL("The IP address passed in is not a valid IPv4 address");
    }
    unsigned short parsedPort = cmdParser.get<unsigned short>("port");
    if (parsedPort < 1024)
    {
        FATAL("Cannot use system reserved ports (< 1024)");
    }
    else
    {
        serverAddress.sin_port = htons(parsedPort);
    }
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        FATAL("Cannot create socket");
    }
    INFO("Try connecting to server with IP = %s  port = %hu", parsedIp, parsedPort);

    if (connect(sockfd, (sockaddr*)&serverAddress, sizeof(serverAddress)) != 0)
    {
        FATAL("Cannot connect to server");
    }

    while (true)
    {
        const int MAX_LINE_LEN = 256;
        char buffer[MAX_LINE_LEN];
        fgets(buffer, MAX_LINE_LEN, stdin);
        int bufferLen = strlen(buffer);
        if (send(sockfd, buffer, strlen(buffer), 0) == -1)
        {
            ERROR("Cannot send message");
        }
    }
}

