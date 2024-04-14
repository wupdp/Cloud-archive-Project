#include "../include/socket_utils.h"

int setup_server_socket(const char* port) {
    int listenfd;
    struct sockaddr_in servaddr;

    // Создание сокета
    if ((listenfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        cerr << "Problem in creating the socket" << endl;
        exit(2);
    }

    // Настройка адреса сервера
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(atoi(port));

    // Привязка сокета к адресу
    if (bind(listenfd, (struct sockaddr*)&servaddr, sizeof(servaddr)) < 0) {
        cerr << "Problem in binding the socket" << endl;
        exit(2);
    }

    // Начало прослушивания сокета
    listen(listenfd, LISTENQ);

    return listenfd;
}
