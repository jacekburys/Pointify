#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <cmdlog.h>
#include <cmdline.h>

#ifndef SOCKET_H
#define SOCKET_H

class Socket
{
private:
    const char* ip;
    unsigned short port;
public:
    Socket(const char* inputIp, unsigned short inputPort);
    void start();
};

#endif