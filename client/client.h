#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fstream>
#include <string.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#ifdef WINDOWS
#include <direct.h>
    #define GetCurrentDir _getcwd
#else
#include <unistd.h>
#define GetCurrentDir getcwd
#endif

#define BUFFER_SIZE 4096

struct command {
    char code[250];
    char serv_code[250];
    char arg[50];
};

void menu_client();
void read_input(char* buffer, int size);
int read_command_client(char* buf, int size, struct command *cstruct);
void client_show_list(SSL* ssl, char* server_ip);
void client_get_file(SSL* ssl, char* server_ip, struct command cmd) ;
void client_put_file(SSL* ssl, char* server_ip, struct command cmd);
void client_cd_action(SSL* ssl, struct command cmd);
void client_pwd_action(SSL* ssl);