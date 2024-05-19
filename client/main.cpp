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

    // New code to read server message after connection
    char server_message[BUFFER_SIZE];
    int bytes = SSL_read(ssl, server_message, sizeof(server_message) - 1);
    if (bytes > 0) {
        server_message[bytes] = 0;
        printf("%s\n", server_message);
    }
    
   // use_default_colors();

    setlocale(LC_ALL, "");
    initscr(); // Инициализация структуры экрана
    start_color();	
    cbreak(); // Включение режима посимвольного ввода
    //noecho(); // Отключение отображения введенных символов
    keypad(stdscr, TRUE); // Разрешение обработки функциональных клавиш

    int attempt = 0;
    if (authentification(ssl) != 0) {
        endwin();
        echo();
        printf("New user added!\n");

        // Clean up
        SSL_shutdown(ssl);
        close(sockfd);
        SSL_free(ssl);
        SSL_CTX_free(ctx);
        exit(6);
    }

    clear();
    print_menu();

    struct command cmd;
    while (1) {
        if (read_command_client(buffer, sizeof buffer, &cmd) < 0) {
            init_pair(6, COLOR_RED, COLOR_BLACK);
            attron(COLOR_PAIR(6));
            move(getcury(stdscr), 0); // Перемещение курсора в начало строки
            printw("Invalid command\n");
            attroff(COLOR_PAIR(6));
            refresh();
            continue;
        }

        SSL_write(ssl, cmd.serv_code, BUFFER_SIZE);

        if (strcmp(cmd.code, "QUIT") == 0) {
            clear();
            move(getcury(stdscr), 0); // Перемещение курсора в начало строки
            printw("Quit command...\n");
            refresh();
            break;
        }
        else if (strcmp(cmd.code, "LS") == 0) {
            clear();
            init_pair(5, COLOR_GREEN, COLOR_BLACK);
            attron(COLOR_PAIR(5));
            move(getcury(stdscr), 0); // Перемещение курсора в начало строки
            printw("Executing client_show_list...\n");
            attroff(COLOR_PAIR(5));
            refresh();
            move(getcury(stdscr)+1, 0);
            client_show_list(ssl, server_ip);
        }
        else if (strcmp(cmd.code, "GET") == 0) {
            clear();
            move(getcury(stdscr), 0);
            init_pair(5, COLOR_GREEN, COLOR_BLACK);
            attron(COLOR_PAIR(5));
            printw("Getting file...\n");
            attroff(COLOR_PAIR(5));
            refresh();
            move(getcury(stdscr)+1, 0);
            SSL_write(ssl, cmd.arg, BUFFER_SIZE);
            client_get_file(ssl, server_ip, cmd);
        }
        else if (strcmp(cmd.code, "PUT") == 0) {
            clear();
            init_pair(5, COLOR_GREEN, COLOR_BLACK);
            attron(COLOR_PAIR(5));
            move(getcury(stdscr), 0);
            printw("Putting file...\n");
            attroff(COLOR_PAIR(5));
            refresh();
            move(getcury(stdscr)+1, 0);
            SSL_write(ssl, cmd.arg, BUFFER_SIZE);
            client_put_file(ssl, server_ip, cmd);
        }
        else if (strcmp(cmd.code, "CD") == 0) {
            clear();
            SSL_write(ssl, cmd.arg, BUFFER_SIZE);
            client_cd_action(ssl, cmd);
        }
        else if (strcmp(cmd.code, "PWD") == 0) {
            clear();
            move(getcury(stdscr), 0);
            client_pwd_action(ssl);
        }
        else if (strcmp(cmd.code, "!LS") == 0) {
            move(getcury(stdscr), 0);
            system("ls");
            refresh();
            move(getcury(stdscr), 0);
            printw("done");
            refresh();
        }
        else if (strcmp(cmd.code, "!PWD") == 0) {
            move(getcury(stdscr), 0);
            system("pwd");
            refresh();
            move(getcury(stdscr), 0);
            printw("done");
            refresh();
        }
        else if (strcmp(cmd.code, "!CD") == 0) {
            //cmd.arg = strtok(NULL, " \n");
            move(getcury(stdscr), 0);
            printw("Path given is: %s",cmd.arg);
            refresh();
            if (chdir(cmd.arg) < 0){
                move(getcury(stdscr) + 1, 0);
                printw("Directory doesn't exist. Check path");
                refresh();
            }
            move(getcury(stdscr) + 1, 0);
        }
        else if (strcmp(cmd.code, "HELP") == 0) {
                clear();
                move(getcury(stdscr), 0);
                print_menu();
                move(getcury(stdscr) + 1, 0);
        }

    }

    endwin();
    echo();
    printf("Thank you, buy!\n");

    // Clean up
    SSL_shutdown(ssl);
    close(sockfd);
    SSL_free(ssl);
    SSL_CTX_free(ctx);

    return 0;
}