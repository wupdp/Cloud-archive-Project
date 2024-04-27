#ifndef SERVER_H
#define SERVER_H



#include "socket.h"
#include "ftp_commands.h"
#include "authentification.h"
#include <iostream>
#include <thread>
#include <atomic>
#include <iostream>
#include <cstdlib>
#include <cstring>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

void run_server(int argc, char **argv);

using namespace std;

#endif // SERVER_H