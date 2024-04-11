//
// Created by gabr on 11.04.24.
//
#ifndef NETWORK_H
#define NETWORK_H

#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

class Network {
private:
    int sockfd;
    int port;

public:
    Network(int port);
    ~Network();

    bool createSocket();
    bool bindSocket();
    bool listenForConnections(int backlog);
    int acceptConnection(struct sockaddr_in &cliaddr);
};

#endif // NETWORK_H
