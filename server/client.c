//
// Created by gabr on 30.03.24.
//
#define _GNU_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>

#define BUFFER_SIZE 1024
#define PORT 8080

char *remove_substring(char *str, const char *sub) {
    size_t len = strlen(sub);
    if (len == 0) return str;

    char *p = str;
    while ((p = strstr(p, sub))) {
        memmove(p, p + len, strlen(p + len) + 1);
    }
    str[strlen(str) - 1] = '\0';
    return str;
}


void send_file_to_server(int client_socket, const char *filename) {
    char buffer[BUFFER_SIZE];
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        perror("Ошибка при открытии файла");
        return;
    }

    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        send(client_socket, buffer, bytes_read, 0);
    }

    fclose(file);
}

void handle_retr_response(int client_socket, const char *filename) {
    char buffer[BUFFER_SIZE];

    // Получаем подтверждение от сервера
    recv(client_socket, buffer, BUFFER_SIZE, 0);
    if (strcmp(buffer, "READY") != 0) {
        printf("Server is not ready to send the file.\n");
        return;
    }

    FILE *file = fopen(filename, "wb");
    if (file == NULL) {
        perror("Ошибка при открытии файла");
        return;
    }

    ssize_t bytes_received;
    while ((bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0)) > 0) {
        fwrite(buffer, 1, bytes_received, file);
    }

    fclose(file);
}

int main() {
    int client_socket;
    struct sockaddr_in server_address;
    char *buffer = (char *) malloc(sizeof(char) * BUFFER_SIZE);
    FILE *file;
    char *file_name = (char *) malloc(sizeof(char) * BUFFER_SIZE);
    // Создание сокета
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Socket creation failed");
        return 1;
    }

    // Настройка адреса сервера
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = inet_addr("127.0.0.1"); // Укажите IP-адрес сервера
    server_address.sin_port = htons(PORT);  // Укажите порт сервера

    // Подключение к серверу
    if (connect(client_socket, (struct sockaddr *) &server_address, sizeof(server_address)) == -1) {
        perror("Connection failed");
        return 1;
    }

    char option[BUFFER_SIZE];
    printf("ECHO -отправить запрос\n");
    printf("INFO -получить информацию о сервере\n");
    printf("CD   -сменить каталог сервера\n");
    printf("LIST -список объектов из текущего каталога\n");
    printf("RETR -скачать файл\n");
    printf("STOR -загрузить файл\n");
    printf("PASV -перейти в пассивный режим\n");
    printf("PORT -перейти в активный режим\n");
    printf("QUIT -выход из программы\n");
    while (1) {
        printf("\n>");
        fgets(option, sizeof(option), stdin);
        if (strstr(option, "@")) {
            file_name = remove_substring(option, "@");
            send_file_to_server(client_socket, file_name);
        } else if (strstr(option, "QUIT")) {
            send(client_socket, option, BUFFER_SIZE, 0);
            recv(client_socket, buffer, BUFFER_SIZE, 0);
            printf("%s\n", buffer);
            break;
        } else if (strstr(option, "RETR")) {
            char *filename = remove_substring(option, "RETR ");
            send(client_socket, option, BUFFER_SIZE, 0);
            handle_retr_response(client_socket, filename);

        } else {
            send(client_socket, option, BUFFER_SIZE, 0);
            recv(client_socket, buffer, BUFFER_SIZE, 0);
            printf("%s\n", buffer);
        }
        memset(buffer, 0, BUFFER_SIZE);
    }

    if (buffer)
        free(buffer);
    close(client_socket);
}