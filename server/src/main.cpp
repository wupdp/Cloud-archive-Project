//
// Created by gabr on 11.04.24.
//
#include "../include/server.h"
#include <iostream>

int main() {
    const int port = 8080; // Порт, на котором запускаем сервер
    const int backlog = 5; // Максимальная очередь подключений

    Network network(port);
    Server server(port, backlog);

    if (!network.createSocket() || !network.bindSocket() || !network.listenForConnections(backlog)) {
        std::cerr << "Error setting up server" << std::endl;
        return 1;
    }

    std::cout << "Server running on port " << port << "..." << std::endl;

    server.start();

    return 0;
}
