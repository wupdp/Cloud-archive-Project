//
// Created by gabr on 11.04.24.
//
#ifndef CONNECTION_H
#define CONNECTION_H

#include <iostream>

class Connection {
private:
    int connfd;

public:
    Connection(int connfd);
    ~Connection();

    bool receiveData(char* buffer, int bufferSize);
    bool sendData(const char* data, int dataSize);
    void closeConnection();
};

#endif // CONNECTION_H
