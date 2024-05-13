#ifndef UTILITIES_H
#define UTILITIES_H

#include <string>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>

std::string get_current_dir();
int create_socket(int port);
int accept_conn(int sock);

#endif // UTILITIES_H