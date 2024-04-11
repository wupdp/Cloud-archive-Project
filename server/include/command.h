//
// Created by gabr on 11.04.24.
//
#ifndef COMMAND_H
#define COMMAND_H

#include "connection.h"

class Command {
private:
    Connection* connection;

public:
    Command(Connection* conn);
    ~Command();

    void processCommand();
};

#endif // COMMAND_H
