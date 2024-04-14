#ifndef SERVER_H
#define SERVER_H

#include <iostream>
#include <unistd.h>
#include <netinet/in.h>

#include "../include/socket_utils.h"
#include "../include/command_handler.h"

#define MAXLINE 4096 /*max text line length*/
#define LISTENQ 8 /*maximum number of client connections*/

using namespace std;

int create_socket(int);
int accept_conn(int);
void accept_client_connections(int listenfd, int data_port, const char* root_dir);

#endif /* SERVER_H */