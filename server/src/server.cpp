#include "../include/server.h"

void handle_ftp_command(SSL *ssl, int connfd, int data_port) {
    char command[MAXLINE];
    SSL_read(ssl, command, sizeof(command));

    if (strncmp(command, "ls", 2) == 0) {
        handle_ls_command(connfd, data_port);
    } else if (strncmp(command, "pwd", 3) == 0) {
        handle_pwd_command(connfd);
    } else if (strncmp(command, "cd", 2) == 0) {
        char directory[MAXLINE];
        SSL_read(ssl, directory, sizeof(directory));
        handle_cd_command(connfd, directory);
    } else if (strncmp(command, "put", 3) == 0) {
        char filename[MAXLINE];
        SSL_read(ssl, filename, sizeof(filename));
        handle_put_command(connfd, data_port, filename);
    } else if (strncmp(command, "get", 3) == 0) {
        char filename[MAXLINE];
        SSL_read(ssl, filename, sizeof(filename));
        handle_get_command(connfd, data_port, filename);
    } else {
        std::cerr << "Unknown command: " << command << std::endl;
    }
}

void run_server_ssl(int argc, char **argv) {
    int data_port = 1024;
    int listenfd, connfd;
    pid_t childpid;
    socklen_t clilen;
    struct sockaddr_in cliaddr, servaddr;

    if (argc != 3) {
        std::cerr << "Usage: ./server_ssl <port number> <root directory>" << std::endl;
        exit(1);
    }

    SSL_CTX *ctx;
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    ctx = SSL_CTX_new(SSLv23_server_method());
    if (!ctx) {
        std::cerr << "SSL context creation failed." << std::endl;
        exit(2);
    }

    // Загрузка сертификата и закрытого ключа
    if (SSL_CTX_use_certificate_file(ctx, "../server.crt", SSL_FILETYPE_PEM) <= 0) {
        std::cerr << "Error loading server certificate." << std::endl;
        exit(3);
    }
    if (SSL_CTX_use_PrivateKey_file(ctx, "../server.key", SSL_FILETYPE_PEM) <= 0) {
        std::cerr << "Error loading server private key." << std::endl;
        exit(4);
    }

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd < 0) {
        std::cerr << "Problem in creating the socket" << std::endl;
        exit(5);
    }

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(atoi(argv[1]));

    bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

    listen(listenfd, LISTENQ);

    for (;;) {
        clilen = sizeof(cliaddr);
        connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &clilen);

        if ((childpid = fork()) == 0) {
            close(listenfd);

            SSL *ssl = SSL_new(ctx);
            SSL_set_fd(ssl, connfd);

            if (SSL_accept(ssl) <= 0) {
                std::cerr << "SSL handshake error." << std::endl;
                exit(6);
            }

            handle_ftp_command(ssl, connfd, data_port);

            close(connfd);
            exit(0);
        }

        close(connfd);
    }

    SSL_CTX_free(ctx);
    close(listenfd);
}

