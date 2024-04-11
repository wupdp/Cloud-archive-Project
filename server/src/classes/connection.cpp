//
// Created by gabr on 11.04.24.
//
#include "../../include/connection.h"
#include <unistd.h>

Connection::Connection(int connfd) : connfd(connfd) {}

Connection::~Connection() {
    closeConnection();
}

bool Connection::receiveData(char* buffer, int bufferSize) {
    int n = read(connfd, buffer, bufferSize);
    return n >= 0;
}

bool Connection::sendData(const char* data, int dataSize) {
    int n = write(connfd, data, dataSize);
    return n >= 0;
}

void Connection::closeConnection() {
    if (connfd != -1) {
        close(connfd);
        connfd = -1;
    }
}
