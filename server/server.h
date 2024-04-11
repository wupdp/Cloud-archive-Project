//
// Created by gabr on 30.03.24.
//
#ifndef CLIENT_SERVER_SERVER_H
#define CLIENT_SERVER_SERVER_H

#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <dirent.h>
#include <stdbool.h>
#include <sys/stat.h>
#include <pthread.h>
#include <sys/time.h>
#include <netinet/in.h>

#define BUF_SIZE 1024
#define MAX_THREAD 5
#define PORT 8080

// Структура для аргументов сервера
struct server_args {
    int socket;
    int adr_len;
};

void handle_pasv_mode(int client_socket);
void handle_port_mode(int client_socket, char *ip, int port);
int create_data_socket();
void handle_data_transfer(int data_socket);
void send_response(int client_socket, char* response);
void handle_retr_command(int client_socket, char* filename);
void handle_stor_command(int client_socket, char* filename);
void *handle_client(void *arg);
void *handle_server(void *arg);
char* remove_substring(char* str, const char* sub);
char *merge_strings(char *first_str, char *second_str);
char* get_directory_listing(const char* current_dir);

#endif //CLIENT_SERVER_SERVER_H