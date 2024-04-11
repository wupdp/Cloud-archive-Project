//
// Created by gabr on 11.04.24.
//
#include "../../include/network.h"
#include <unistd.h>

Network::Network(int port) : port(port), sockfd(-1) {}

Network::~Network() {
    if (sockfd != -1) {
        close(sockfd);
    }
}

bool Network::createSocket() {
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    return sockfd != -1;
}

bool Network::bindSocket() {
    struct sockaddr_in servaddr;
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(port);

    return bind(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) != -1;
}

bool Network::listenForConnections(int backlog) {
    return listen(sockfd, backlog) != -1;
}

int Network::acceptConnection(struct sockaddr_in &cliaddr) {
    socklen_t clilen = sizeof(cliaddr);
    return accept(sockfd, (struct sockaddr *)&cliaddr, &clilen);
}
