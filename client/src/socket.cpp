#include "socket.hpp"

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

Socket::Socket(const char* inputIp, unsigned short inputPort)
{
    ip = inputIp;
    port = inputPort;
}

void Socket::start()
{
    sockaddr_in serverAddress = {0};
    serverAddress.sin_family = AF_INET;
    if (!inet_pton(AF_INET, ip, &serverAddress.sin_addr))
    {
        FATAL("The IP address passed in is not a valid IPv4 address");
    }
    if (port < 1024)
    {
        FATAL("Cannot use system reserved ports (< 1024)");
    }
    else
    {
        serverAddress.sin_port = htons(port);
    }
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        FATAL("Cannot create socket");
    }
    INFO("Try connecting to server with IP = %s  port = %hu", ip, port);

    if (connect(sockfd, (sockaddr*)&serverAddress, sizeof(serverAddress)) != 0)
    {
        FATAL("Cannot connect to server");
    }

    while (true)
    {
        const int MAX_LINE_LEN = 256;
        char buffer[MAX_LINE_LEN];
        fgets(buffer, MAX_LINE_LEN, stdin);
        if (send(sockfd, buffer, strlen(buffer), 0) == -1)
        {
            ERROR("Cannot send message");
        }
    }
}