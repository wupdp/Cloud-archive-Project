//
// Created by gabr on 11.04.24.
//
#include "../../include/command.h"
#include <iostream>
#include <cstring>
#include <unistd.h>

Command::Command(Connection* conn) : connection(conn) {}

Command::~Command() {}

void Command::processCommand() {
    char buffer[4096]; // adjust buffer size as needed
    memset(buffer, 0, sizeof(buffer)); // Clear buffer

    if (!connection->receiveData(buffer, sizeof(buffer))) {
        std::cerr << "Error receiving data from client" << std::endl;
        return;
    }

    // Process received command
    // For simplicity, let's just print the received command
    std::cout << "Received command from client: " << buffer << std::endl;

    // Example: Send acknowledgment back to client
    const char* ackMsg = "Command received";
    if (!connection->sendData(ackMsg, strlen(ackMsg))) {
        std::cerr << "Error sending acknowledgment to client" << std::endl;
        return;
    }

    // Example: Close connection if client sends "quit"
    if (strcmp(buffer, "quit\n") == 0) {
        std::cout << "Client requested to quit" << std::endl;
        connection->closeConnection();
    }
}
