#ifndef SERVER_H
#define SERVER_H

#include "socket.h"
#include "ftp_commands.h"
#include <iostream>
#include <thread>
#include <atomic>

void run_server(int argc, char **argv);

using namespace std;

#endif // SERVER_H