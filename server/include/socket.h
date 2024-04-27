#ifndef SOCKET_H
#define SOCKET_H

#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <cstring>

int create_socket(int port);
int accept_conn(int sock);

using namespace std;

#endif // SOCKET_H