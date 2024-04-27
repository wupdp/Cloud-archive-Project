#include "client.h"

using namespace std;

SSL_CTX* init_ssl_context() {
    SSL_library_init();
    SSL_CTX* ctx = SSL_CTX_new(SSLv23_client_method());
    if (!ctx) {
        cerr << "SSL context creation failed." << endl;
        exit(EXIT_FAILURE);
    }
    return ctx;
}

void handle_error(const string& msg) {
    cerr << msg << endl;
    ERR_print_errors_fp(stderr);
    exit(EXIT_FAILURE);
}

int main(int argc, char **argv)
{
    int sockfd;
    struct sockaddr_in servaddr;
    char sendline[BUFFER_SIZE], recvline[BUFFER_SIZE];
    char buffer[BUFFER_SIZE];

    // Check of the arguments
    if (argc < 3) {
        cerr << "Usage: ./ftclient <IP address of the server> <port number>" << endl;
        exit(1);
    }

    int server_port = atoi(argv[2]);

    // Check port number
    if (server_port < 1) {
        printf("Incorrect FTP port!\n");
        exit(2);
    }

    // Check hostname (IP address)
    char* server_ip = argv[1];
    struct hostent *host_name;
    host_name = gethostbyname(server_ip);
    if (host_name == NULL) {
        printf("Incorrect FTP hostname (IP address)!\n");
        exit(3);
    }

    // Create SSL context
    SSL_CTX* ctx = init_ssl_context();

    // Create a socket for the client
    if ((sockfd = socket (AF_INET, SOCK_STREAM, 0)) < 0) {
        cerr << "Problem in creating the socket" << endl;
        exit(4);
    }

    // Creation of the socket
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(argv[1]);
    servaddr.sin_port =  htons(server_port); // Convert to big-endian order

    // Connection of the client to the socket
    if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        cerr << "Problem in connecting to the server" << endl;
        exit(5);
    }

    // Create SSL object
    SSL* ssl = SSL_new(ctx);
    if (!ssl) {
        handle_error("SSL object creation failed");
    }

    // Associate SSL object with file descriptor of the socket
    SSL_set_fd(ssl, sockfd);

    // Perform SSL handshake
    if (SSL_connect(ssl) <= 0) {
        handle_error("SSL handshake error");
    }

    menu_client();
    struct command cmd;
    while (1) {
        if (read_command_client(buffer, sizeof buffer, &cmd) < 0) {
            printf("Invalid command\n");
            continue;
        }

        SSL_write(ssl, cmd.serv_code, BUFFER_SIZE);

        if (strcmp(cmd.code, "QUIT") == 0) {
            printf("Quit command...\n");
            break;
        }
        else if (strcmp(cmd.code, "LS") == 0) {
            cout << "Executing client_show_list..." << endl;
            client_show_list(ssl, server_ip);
        }
        else if (strcmp(cmd.code, "GET") == 0) {
            cout << "Getting file..." << endl;
            client_get_file(ssl, server_ip, cmd);
        }
        else if (strcmp(cmd.code, "PUT") == 0) {
            cout << "Putting file..." << endl;
            client_put_file(ssl, server_ip, cmd);
        }
        else if (strcmp(cmd.code, "CD") == 0) {
            client_cd_action(ssl, cmd);
        }
        else if (strcmp(cmd.code, "PWD") == 0) {
            client_pwd_action(ssl);
        }

    }

    printf("Thank you, buy!\n");

    // Clean up
    SSL_shutdown(ssl);
    close(sockfd);
    SSL_free(ssl);
    SSL_CTX_free(ctx);

    return 0;
}
