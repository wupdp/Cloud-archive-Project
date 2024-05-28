#ifndef FTP_COMMANDS_H
#define FTP_COMMANDS_H

#define MAXLINE 4096 /*max text line length*/
#define LISTENQ 8 /*maximum number of client connections*/

#include "../include/utilities.h"

#include <openssl/ssl.h>
#include <openssl/err.h>
#include <unistd.h>
#include <sys/socket.h>
#include <iostream>
#include <limits.h>

extern std::string root_directory;
using namespace std;

void handle_ls_command(SSL* ssl);
void handle_pwd_command(SSL* ssl);
void handle_cd_command(SSL* ssl, const char* directory);
void handle_put_command(SSL* ssl, int data_port, const char* filename);
void handle_get_command(SSL* ssl, int data_port, const char* filename);

#endif // FTP_COMMANDS_H