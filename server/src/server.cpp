#include "../include/server.h"

std::string root_directory;

bool handle_ftp_command(SSL *ssl, int connfd, int data_port) {
    char command[MAXLINE];
    SSL_read(ssl, command, sizeof(command));

    if (strncmp(command, "ls", 2) == 0) {
        handle_ls_command(ssl);
    } else if (strncmp(command, "pwd", 3) == 0) {
        handle_pwd_command(ssl);
    } else if (strncmp(command, "cd", 2) == 0) {
        char directory[MAXLINE];
        SSL_read(ssl, directory, sizeof(directory));
        handle_cd_command(ssl, directory);
    } else if (strncmp(command, "put", 3) == 0) {
        char filename[MAXLINE];
        SSL_read(ssl, filename, sizeof(filename));
        handle_put_command(ssl, data_port, filename);
    } else if (strncmp(command, "get", 3) == 0) {
        char filename[MAXLINE];
        SSL_read(ssl, filename, sizeof(filename));
        handle_get_command(ssl, data_port, filename);
    } else if (strncmp(command, "quit", 4) == 0) {
        return false;
    } else {
        std::cerr << "Unknown command: " << command << std::endl;
    }
    return true;
}

void run_server(int argc, char **argv) {
    int data_port = 1024;
    int listenfd, connfd;
    pid_t childpid;
    socklen_t clilen;
    struct sockaddr_in cliaddr, servaddr;

    if (argc != 3) {
        std::cerr << "Usage: ./server_ssl <port number> <root directory>" << std::endl;
        return;
    }

    SSL_CTX *ctx;
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    ctx = SSL_CTX_new(SSLv23_server_method());
    if (!ctx) {
        std::cerr << "SSL context creation failed." << std::endl;
        return;
    }

    // Загрузка сертификата и закрытого ключа
    if (SSL_CTX_use_certificate_file(ctx, "../server.crt", SSL_FILETYPE_PEM) <= 0) {
        std::cerr << "Error loading server certificate." << std::endl;
        return;
    }
    if (SSL_CTX_use_PrivateKey_file(ctx, "../server.key", SSL_FILETYPE_PEM) <= 0) {
        std::cerr << "Error loading server private key." << std::endl;
        return;
    }

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd < 0) {
        std::cerr << "Problem in creating the socket" << std::endl;
        return;
    }

    root_directory = argv[2];

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(atoi(argv[1]));

    if (bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        std::cerr << "Problem in binding the socket" << std::endl;
        return;
    }

    if (listen(listenfd, LISTENQ) < 0) {
        std::cerr << "Problem in listening on the socket" << std::endl;
        return;
    }

    for (;;) {
        clilen = sizeof(cliaddr);
        connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &clilen);
        if (connfd < 0) {
            std::cerr << "Problem in accepting the socket" << std::endl;
            return;
        }

        if ((childpid = fork()) == 0) {
            close(listenfd);

            SSL *ssl = SSL_new(ctx);
            SSL_set_fd(ssl, connfd);

            if (SSL_accept(ssl) <= 0) {
                std::cerr << "SSL handshake error." << std::endl;
                SSL_free(ssl);
                return;
            }

            SSL_write(ssl, "Handshake!", sizeof ("Handshake!"));

            //if (authenticateUserSSL(ssl)) {
               while(handle_ftp_command(ssl, connfd, data_port));
            /*} else {
                char new_username[256];
                char new_password[256];

                SSL_read(ssl, new_username, sizeof(new_username));
                SSL_read(ssl, new_password, sizeof(new_password));

                addUser(new_username, new_password);

                SSL_write(ssl, "New user added successfully", strlen("New user added successfully"));
            }*/

            close(connfd);
            SSL_free(ssl);
            return;
        }

        close(connfd);
    }

    SSL_CTX_free(ctx);
    close(listenfd);
}