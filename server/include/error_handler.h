#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

#include <iostream>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>

using namespace std;

void send_error_message(int connfd, const char* message);

#endif /* ERROR_HANDLER_H */
