#include "../include/server.h"

void handle_client_request(int connfd, int data_port, const char* root_dir) {
    char buf[MAXLINE];
    int n;

    while ((n = recv(connfd, buf, MAXLINE, 0)) > 0) {
        // Обработка запросов от клиента
        handle_command(connfd, buf, data_port, root_dir);

        if (n < 0)
            cerr << "Read error" << endl;
    }

    if (n == 0)
        cout << "Client closed connection" << endl;
    else
        cerr << "Error in receiving data from client" << endl;

    close(connfd);
}

void accept_client_connections(int listenfd, int data_port, const char* root_dir) {
    struct sockaddr_in cliaddr;
    socklen_t clilen;
    int connfd;

    while (true) {
        clilen = sizeof(cliaddr);
        connfd = accept(listenfd, (struct sockaddr*)&cliaddr, &clilen);
        if (connfd < 0) {
            cerr << "Error in accepting connection" << endl;
            continue;
        }

        cout << "Received request..." << endl;

        if (fork() == 0) {
            close(listenfd);
            handle_client_request(connfd, data_port, root_dir);
            exit(0);
        }
        close(connfd);
    }
}

int main(int argc, char** argv) {
    if (argc != 3) {
        cerr << "Usage: " << argv[0] << " <port number> <root directory>" << endl;
        exit(1);
    }

    // Основные настройки сервера
    int listenfd = setup_server_socket(argv[1]);
    int data_port = atoi(argv[1]) + 1;
    const char* root_dir = argv[2];

    cout << "Server running...waiting for connections." << endl;

    // Принятие и обработка соединений от клиентов
    accept_client_connections(listenfd, data_port, root_dir);

    return 0;
}
