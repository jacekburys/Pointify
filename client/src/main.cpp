#include <unistd.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <cxxopts.hpp>

int sockfd;

void fatal(const char *msg)
{
    fprintf(stderr, "FATAL - %s\n", msg);
    exit(1);
}

void error(const char *msg)
{
    fprintf(stderr, "ERROR - %s\n", msg);
}

void info(const char* msg)
{
    fprintf(stdout, "INFO - %s\n", msg);
}

void endProgram(int sig)
{
    close(sockfd);
    exit(0);
}

void printIp(int ip)
{
    printf("%u.%u.%u.%u", ip & 0xff, (ip >> 8) & 0xff, (ip >> 16) & 0xff, ip >> 24);        
}

void printPort(unsigned short port)
{
    printf("%hu", ntohs(port));
}

int main(int argc, char *argv[])
{
    cxxopts::Options options(argv[0], "Pointify client");
    
    options.add_options()
        ("ip", "IP address used to connect to the server", cxxopts::value<std::string>())
        ("port", "Port used to connect to the server", cxxopts::value<std::string>())
    ;
    options.parse(argc, argv);

    sockaddr_in serverAddress;
    bzero(&serverAddress, sizeof(serverAddress));
    serverAddress.sin_family = AF_INET;
    if (options.count("ip"))
    {
        if (!inet_aton(options["ip"].as<std::string>().c_str(), &serverAddress.sin_addr))
        {
            fatal("The IP address passed in is not a valid IPv4 address.");
        }
    }
    else
    {
        info("IP address not specified, using localhost instead.");
        inet_aton("127.0.0.1", &serverAddress.sin_addr);
    }
    if (options.count("port"))
    {
        int parsedPort = atoi(options["port"].as<std::string>().c_str());
        if (!parsedPort || parsedPort < 1024 || parsedPort > 5000)
        {
            fatal("The port passed in is not a valid port, or out of range 1024 - 5000.");
        }
        serverAddress.sin_port = htons(parsedPort);
    }
    else
    {
        info("Port not specified, using 4500 instead.");
        serverAddress.sin_port = htons(4500);
    }

    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        fatal("Cannot open socket.");
    }
    int opt_val = 1;
    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char *)&opt_val, sizeof(opt_val));
    if (connect(sockfd, (sockaddr*)&serverAddress, sizeof(serverAddress)) != 0)
    {
        fatal("Cannot bind to server.");
    }

    signal(SIGINT, endProgram);
    while (true)
    {
        char buffer[256];
        bzero(buffer, 256);
        fgets(buffer, 255, stdin);
        if (write(sockfd, buffer, strlen(buffer)) < 0)
        {
            fatal("Cannot write to socket.");
        }
    }
}

