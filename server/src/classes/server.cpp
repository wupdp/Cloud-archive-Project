//
// Created by gabr on 11.04.24.
//
#include "../../include/server.h"
#include <iostream>
#include <cstring>

Server::Server(int port, int backlog) : backlog(backlog) {
    network = new Network(port);
}

Server::~Server() {
    delete network;
}

void Server::start() {
    if (!network->createSocket()) {
        std::cerr << "Error creating socket" << std::endl;
        return;
    }

    if (!network->bindSocket()) {
        std::cerr << "Error binding socket" << std::endl;
        return;
    }

    if (!network->listenForConnections(backlog)) {
        std::cerr << "Error listening for connections" << std::endl;
        return;
    }

    std::cout << "Server running...waiting for connections." << std::endl;

    struct sockaddr_in cliaddr;
    while (true) {
        int connfd = network->acceptConnection(cliaddr);
        if (connfd == -1) {
            std::cerr << "Error accepting connection" << std::endl;
            continue;
        }

        handleClient(connfd);
    }
}

void Server::handleClient(int connfd) {
    Connection clientConn(connfd);
    char buffer[4096]; // adjust buffer size as needed

    while (true) {
        memset(buffer, 0, sizeof(buffer)); // Clear buffer
        if (!clientConn.receiveData(buffer, sizeof(buffer))) {
            std::cerr << "Error receiving data from client" << std::endl;
            break;
        }

        // Handle client request (parse buffer, process command, etc.)
        // For simplicity, let's just print the received message
        std::cout << "Received message from client: " << buffer << std::endl;

        // Example: Send acknowledgment back to client
        const char* ackMsg = "Message received";
        if (!clientConn.sendData(ackMsg, strlen(ackMsg))) {
            std::cerr << "Error sending acknowledgment to client" << std::endl;
            break;
        }

        // Example: Close connection if client sends "quit"
        if (strcmp(buffer, "quit\n") == 0) {
            std::cout << "Client requested to quit" << std::endl;
            break;
        }
    }

    // Close client connection
    clientConn.closeConnection();
}
