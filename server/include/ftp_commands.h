#ifndef FTP_COMMANDS_H
#define FTP_COMMANDS_H

#define MAXLINE 4096 /*max text line length*/
#define LISTENQ 8 /*maximum number of client connections*/

#include "../include/socket.h"
#include "../include/utilities.h"

#include <unistd.h>
#include <sys/socket.h>
#include <iostream>

extern std::string root_directory;

void handle_ls_command(int connfd);
void handle_pwd_command(int connfd);
void handle_cd_command(int connfd, const char* directory);
void handle_put_command(int connfd, int data_port, const char* filename);
void handle_get_command(int connfd, int data_port, const char* filename);

#endif // FTP_COMMANDS_H