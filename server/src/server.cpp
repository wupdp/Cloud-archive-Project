#include <sys/wait.h>
#include "../include/server.h"

std::string root_directory;

bool handle_ftp_command(SSL *ssl, int data_port) {
    char command[MAXLINE];
    SSL_read(ssl, command, sizeof(command));

    if (strncmp(command, "ls", 2) == 0) {
        cout << "ls act" << endl;
        handle_ls_command(ssl);
    } else if (strncmp(command, "pwd", 3) == 0) {
        cout << "pwd act" << endl;
        handle_pwd_command(ssl);
    } else if (strncmp(command, "cd", 2) == 0) {
        char directory[MAXLINE];
        SSL_read(ssl, directory, sizeof(directory));
        handle_cd_command(ssl, directory);
    } else if (strncmp(command, "put", 3) == 0) {
        char filename[MAXLINE];
        cout << "put" << endl;
        SSL_read(ssl, filename, sizeof(filename));
        handle_get_command(ssl, data_port, filename);
    } else if (strncmp(command, "get", 3) == 0) {
        char filename[MAXLINE];
        SSL_read(ssl, filename, sizeof(filename));
        handle_put_command(ssl, data_port, filename);
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
    struct sockaddr_in cliaddr{}, servaddr;

    if (argc != 3) {
        std::cerr << "Usage: ./server_ssl <port number> <root directory>" << std::endl;
        return;
    }

    std::cout << "Starting FTPS server ..." << std::endl;

    SSL_CTX *ctx;
    SSL_library_init();
    OpenSSL_add_all_algorithms();
    SSL_load_error_strings();
    ctx = SSL_CTX_new(SSLv23_server_method());
    if (!ctx) {
        std::cerr << "SSL context creation failed." << std::endl;
        return;
    } else std::cout << "SSL content creation passed" << std::endl;

    // Загрузка сертификата и закрытого ключа
    if (SSL_CTX_use_certificate_file(ctx, "server.crt", SSL_FILETYPE_PEM) <= 0) {
        std::cerr << "Error loading server certificate." << std::endl;
        return;
    } else std::cout << "Loading server certificate passed" << std::endl;

    if (SSL_CTX_use_PrivateKey_file(ctx, "server.key", SSL_FILETYPE_PEM) <= 0) {
        std::cerr << "Error loading server private key." << std::endl;
        return;
    } else std::cout << "Loading server private key passed" << std::endl;

    listenfd = socket(AF_INET, SOCK_STREAM, 0);
    if (listenfd < 0) {
        std::cerr << "Problem in creating the socket" << std::endl;
        return;
    } else std::cout << "Socket creating passed" << std::endl;

    root_directory = argv[2];

    if(chdir(root_directory.c_str())<0){
        std::cerr << "Problem in changing the root directory" << std::endl;
        return;
    } else std::cout << "Root directory changed to" << root_directory << std::endl;

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(stoi(argv[1]));

    if (bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        std::cerr << "Problem in binding the socket" << std::endl;
        return;
    } else std::cout << "Bind socket passed" << std::endl;

    if (listen(listenfd, LISTENQ) < 0) {
        std::cerr << "Problem in listening on the socket" << std::endl;
        return;
    } else std::cout << "Listening socket start. LOOP:" << std::endl;

    for (;;) {
        clilen = sizeof(cliaddr);
        connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &clilen);
        if (connfd < 0) {
            std::cerr << "Problem in accepting the socket" << std::endl;
            return;
        } else std::cout << "Socket accepted" << std::endl << "Fork new process..." << std::endl;

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

            loadUsersFromFile();
             for (size_t i = 0; i < users.size(); ++i) {
                std::cout << users[i].username << " " << users[i].password << endl;
            }
            std::cout << std::endl;

            if (authenticateUserSSL(ssl)) {
               while(handle_ftp_command(ssl, data_port));
            } else {
                char new_username[256];
                char new_password[256];

                int bytes_read = SSL_read(ssl, new_username, sizeof(new_username));
                new_username[bytes_read] = '\0';
                bytes_read = SSL_read(ssl, new_password, sizeof(new_password));
                new_password[bytes_read] = '\0';

                std::cout << new_username << std::endl;
                std::cout << new_password << std::endl;

                addUser(new_username, new_password);
                saveUsersToFile();

                SSL_write(ssl, "New user added successfully", strlen("New user added successfully"));
            }

            close(connfd);
            SSL_free(ssl);
            return;
        } else std::cout << "PID:" << childpid << std::endl;

        while (waitpid(-1, NULL, WNOHANG) > 0) {
            std::cout << "Collected zombie process" << std::endl;
        }
        close(connfd);
    }

    SSL_CTX_free(ctx);
    close(listenfd);
}