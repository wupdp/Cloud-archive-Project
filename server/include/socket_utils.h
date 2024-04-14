#ifndef SOCKET_UTILS_H
#define SOCKET_UTILS_H

#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <arpa/inet.h>

#include "../include/server.h"

using namespace std;

int setup_server_socket(const char* port);

#endif /* SOCKET_UTILS_H */
