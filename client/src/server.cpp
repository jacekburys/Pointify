#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <ifaddrs.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cmdline.h>
#include <cmdlog.h>

int main(int argc, char *argv[])
{
    cmdline::parser cmdParser;
    cmdParser.add<unsigned short>("port", 'p', "Server port number", false, 4500);
    cmdParser.parse_check(argc, argv);
    unsigned short portToUse = cmdParser.get<unsigned short>("port");
    if (portToUse < 1024)
    {
        FATAL("Cannot use system reserved ports (< 1024)");
    }
    {
        ifaddrs *interface = NULL;
        getifaddrs(&interface);
        for (ifaddrs* curr = interface; curr; curr = curr->ifa_next)
        {
            sockaddr_in* ifAddr = (sockaddr_in*)curr->ifa_addr;
            if (ifAddr->sin_family == AF_INET && ((htonl(ifAddr->sin_addr.s_addr) & 0xffffff00) != 0x7f000000))
            {
                char ifIpStr[INET_ADDRSTRLEN];
                inet_ntop(AF_INET, &ifAddr->sin_addr, ifIpStr, sizeof(ifIpStr));
                INFO("Server running at IP = %s", ifIpStr);
            }
        }
        freeifaddrs(interface);
    }

    addrinfo hint = {0}, *serverAddrRes;
    hint.ai_flags = AI_PASSIVE;
    hint.ai_family = AF_INET;
    hint.ai_socktype = SOCK_STREAM;
    char portStr[6];
    sprintf(portStr, "%hu", portToUse);
    int res = getaddrinfo(NULL, portStr, &hint, &serverAddrRes);
    if (res != 0)
    {
        FATAL("Cannot get server address information");
    }
    if (!serverAddrRes)
    {
        FATAL("No available server address");
    }

    int sockfd = socket(serverAddrRes->ai_family, serverAddrRes->ai_socktype, 0);
    if (sockfd < 0)
    {
        FATAL("Cannot create socket");
    }
    int yes = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(int)) == -1)
    {
        FATAL("Cannot set socket option");
    }
    if (bind(sockfd, serverAddrRes->ai_addr, serverAddrRes->ai_addrlen) == -1)
    {
        FATAL("Cannot bind socket to port %s", portStr);
    }
    if (listen(sockfd, 5) == -1)
    {
        FATAL("Cannot listen to port %s", portStr);
    }
    INFO("Server listening with port = %s", portStr);
    freeaddrinfo(serverAddrRes);

    while (true)
    {
        sockaddr_in peerAddr;
        socklen_t peerAddrLen = sizeof(peerAddr);
        int nsockfd = accept(sockfd, (sockaddr*)&peerAddr, &peerAddrLen);
        if (nsockfd == -1)
        {
            FATAL("Cannot accept client");
        }
        char peerIpStr[INET_ADDRSTRLEN];
        inet_ntop(AF_INET, &peerAddr.sin_addr, peerIpStr, sizeof(peerIpStr));
        INFO("Connection from IP = %s", peerIpStr);

        int childPid = fork();
        if (!childPid)
        {
            close(sockfd);
            while (true)
            {
                const int MAX_LINE_LEN = 256;
                char buffer[MAX_LINE_LEN];
                int msgLen = recv(nsockfd, buffer, MAX_LINE_LEN - 1, 0);
                if (msgLen > 0)
                {
                    buffer[msgLen] = '\0';
                    INFO("Received message from IP = %s, Message is \n\t%s", peerIpStr, buffer);
                }
                else if (msgLen == 0)
                {
                    INFO("Connection from IP = %s is closed", peerIpStr);
                    close(nsockfd);
                    exit(0);
                }
                else
                {
                    ERROR("Cannot receive client data");
                }
            }
        }

        if (childPid == -1)
        {
            ERROR("Failed to create child process");
        }
        close(nsockfd);
    }
}

