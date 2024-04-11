//
// Created by gabr on 11.04.24.
//
#ifndef SERVER_H
#define SERVER_H

#include "network.h"
#include "connection.h"

class Server {
private:
    Network* network;
    int backlog;

public:
    Server(int port, int backlog);
    ~Server();

    void start();
    void handleClient(int connfd);
};

#endif // SERVER_H
