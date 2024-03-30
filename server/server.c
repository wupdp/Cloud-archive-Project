//
// Created by gabr on 30.03.24.
//
#include "server.h"

int count_thread = 1;
char path_to_server[BUF_SIZE];
char rood[BUF_SIZE];
char name_server[] = {"\n\t\t===!FTP SERVER!===\n"};
bool server_free = false;

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

char *merge_strings(char *first_str, char *second_str) {
    char *result = malloc(BUF_SIZE);
    strcpy(result, first_str);
    strcat(result, second_str);
    return result;
}

char *get_directory_listing(const char *current_dir) {
    DIR *directory;
    struct dirent *de;
    char *file_names = (char *) malloc(sizeof(char) * BUF_SIZE);
    strcpy(file_names, "Current working directory: ");
    strcat(file_names, path_to_server);
    strcat(file_names, "\n");
    directory = opendir(current_dir);
    if (!directory) {
        return file_names;
    }

    while ((de = readdir(directory)) != NULL) {
        if (!strcmp(".", de->d_name) || !strcmp("..", de->d_name)) {
            continue;
        }

        char name[BUF_SIZE];
        strcpy(name, de->d_name);

        if (de->d_type == 4) {
            strcat(name, "/");
        } else if (de->d_type == 10) {
            char link_path[BUF_SIZE];
            struct stat sb;

            ssize_t len = readlink(merge_strings((char *) current_dir, name), link_path, sizeof(link_path) - 1);
            if (len == -1) {
                continue;
            }
            link_path[len] = '\0';

            lstat(link_path, &sb);
            if (S_ISLNK(sb.st_mode)) {
                strcat(name, "-->>");
            } else {
                strcat(name, "-->");
            }

            strcat(name, link_path);
        }

        strcat(name, "\n");
        strcat(file_names, name);
    }

    return file_names;
}


void send_response(int client_socket, char *response) {
    send(client_socket, response, strlen(response), 0);
}


void handle_pasv_mode(int client_socket) {
    // Создаем новый сокет для передачи данных
    int data_socket = create_data_socket();

    // Получаем адрес и порт нового сокета
    struct sockaddr_in data_address;
    socklen_t data_address_len = sizeof(data_address);
    getsockname(data_socket, (struct sockaddr *) &data_address, &data_address_len);

    // Отправляем информацию о новом порте клиенту
    char response[256];
    sprintf(response, "Entering Passive Mode (%s,%d,%d)",
            inet_ntoa(data_address.sin_addr),
            ntohs(data_address.sin_port) / 256,
            ntohs(data_address.sin_port) % 256);
    send_response(client_socket, response);

    // Ждем нового соединения от клиента
    int new_client_socket = accept(data_socket, NULL, NULL);

    // Обрабатываем передачу данных
    handle_data_transfer(new_client_socket);

    // Закрываем сокеты
    close(new_client_socket);
    close(data_socket);
}

void handle_port_mode(int client_socket, char *ip, int port) {
    // Configure the client address
    struct sockaddr_in client_address;
    client_address.sin_family = AF_INET;
    client_address.sin_addr.s_addr = inet_addr(ip);
    client_address.sin_port = htons(port);

    // Connect to the client
    if (connect(client_socket, (struct sockaddr *) &client_address, sizeof(client_address)) == -1) {
        perror("Connection failed");
        return;
    }

    // Handle data transfer
    handle_data_transfer(client_socket);

    // No need to close the socket here as it will be closed by the caller
}

void handle_retr_command(int client_socket, char *filename) {
    char buffer[BUF_SIZE];
    FILE *file = fopen(filename, "rb");
    if (file == NULL) {
        // Отправляем сообщение об ошибке клиенту
        send(client_socket, "ERROR", strlen("ERROR"), 0);
        fprintf(stderr, "Failed to open file: %s\n", filename);
        perror("File open failed");
        return;
    }

    // Отправляем подтверждение клиенту
    send(client_socket, "READY", strlen("READY"), 0);

    size_t bytes_read;
    while ((bytes_read = fread(buffer, 1, BUF_SIZE, file)) > 0) {
        send(client_socket, buffer, bytes_read, 0);
    }

    fclose(file);
}

void handle_stor_command(int client_socket, char *filename) {
    char buffer[BUF_SIZE];
    FILE *file = fopen(filename, "wb");
    if (file == NULL) {
        perror("File open failed");
        return;
    }

    ssize_t bytes_received;
    while ((bytes_received = recv(client_socket, buffer, BUF_SIZE, 0)) > 0) {
        fwrite(buffer, 1, bytes_received, file);
    }

    fclose(file);
}

int create_data_socket() {
    int data_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (data_socket == -1) {
        perror("Data socket creation failed");
        exit(1);
    }

    struct sockaddr_in data_address;
    data_address.sin_family = AF_INET;
    data_address.sin_addr.s_addr = INADDR_ANY;
    data_address.sin_port = htons(2000); // Specify a port number

    if (bind(data_socket, (struct sockaddr *) &data_address, sizeof(data_address)) == -1) {
        perror("Data socket bind failed");
        exit(1);
    }

    if (listen(data_socket, 1) == -1) {
        perror("Data socket listen failed");
        exit(1);
    }

    return data_socket;
}

void handle_data_transfer(int data_socket) {
    char buffer[BUF_SIZE];
    ssize_t bytes_received;


    FILE *file = fopen("received_file", "wb");
    if (file == NULL) {
        perror("File open failed");
        return;
    }

    while ((bytes_received = recv(data_socket, buffer, BUF_SIZE, 0)) > 0) {
        fwrite(buffer, 1, bytes_received, file);
    }

    if (bytes_received == -1) {
        perror("Data transfer failed");
    }

    fclose(file);
}


void *handle_client(void *arg) {
    int client_socket = *(int *) arg;
    char *buffer = (char *) malloc(sizeof(char) * BUF_SIZE);
    while (1) {
        read(client_socket, buffer, BUF_SIZE);
        printf("\nGet from client: %s\n", buffer);
        if (strstr(buffer, "ECHO")) {
            buffer = remove_substring(buffer, "ECHO");
            printf("Received: %s\n", buffer);
            send(client_socket, buffer, BUF_SIZE, 0);
        } else if (strstr(buffer, "LIST")) {
            buffer = get_directory_listing(path_to_server);
            send(client_socket, buffer, BUF_SIZE, 0);
        } else if (strstr(buffer, "RETR")) {
            char *filename = remove_substring(buffer, "RETR ");
            FILE *file = fopen(filename, "rb");
            if (file == NULL) {
                send(client_socket, "File not found", strlen("File not found"), 0);
                continue;
            }
            handle_retr_command(client_socket, filename);
        } else if (strstr(buffer, "STOR")) {
            char *filename = remove_substring(buffer, "STOR ");
            handle_stor_command(client_socket, filename);
        } else if (strstr(buffer, "INFO")) {
            strcpy(buffer, "Path: ");
            buffer = merge_strings(buffer, path_to_server);
            buffer = merge_strings(buffer, "\n");
            buffer = merge_strings(buffer, name_server);
            send(client_socket, buffer, BUF_SIZE, 0);
        } else if (strstr(buffer, "CD")) {
            buffer = remove_substring(buffer, "CD ");
            if (chdir(buffer) != 0) {
                perror("chdir() error");
                send(client_socket, "Ошибка при изменении каталога", sizeof("Ошибка при изменении каталога"), 0);
                continue;
            }
            getcwd(path_to_server, BUF_SIZE);
            memset(buffer, 0, BUF_SIZE);
            if (strcmp(rood, path_to_server) > 0) {
                chdir(rood);
                getcwd(path_to_server, BUF_SIZE);
                strcat(buffer, "\nВыход за коневой католог.\n");
                printf("\nВыход за коневой католог.\n");
            }
            strcat(buffer, "Path: ");
            strcat(buffer, path_to_server);
            send(client_socket, buffer, BUF_SIZE, 0);
        } else if (strstr(buffer, "PASV")) {
            handle_pasv_mode(client_socket);
        } else if (strstr(buffer, "PORT")) {
            char *ip = strtok(buffer, ",");
            int port = atoi(strtok(NULL, ","));
            handle_port_mode(client_socket, ip, port);
        } else if (strstr(buffer, "QUIT")) {
            send(client_socket, "BYE, SEE YOU LATE", strlen("BYE, SEE YOU LATE"), 0);
            break;
        } else {
            send(client_socket, "invalid option", strlen("invalid option"), 0);
        }
        memset(buffer, 0, BUF_SIZE);
    }

    close(client_socket);
    free(buffer);
    printf("\nClient %d ends\n", count_thread - 1);
    count_thread--;
    if (!count_thread)
        server_free = true;
    pthread_exit(NULL);
}

void *handle_server(void *arg) {
    struct server_args server_info = *(struct server_args *) arg;
    struct sockaddr_in client_address;
    pthread_t clients[MAX_THREAD];
    int client_socket;
    int adr_len = server_info.adr_len;
    int server_socket = server_info.socket;
    while (!server_free) {
        fd_set read_fds;
        FD_ZERO(&read_fds);
        FD_SET(server_socket, &read_fds);
        struct timeval pause;

        pause.tv_sec = 1;

        fd_set tmp_fds = read_fds;

        int res = select(server_socket + 1, &read_fds, NULL, NULL, &pause);
        if (res == -1) {
            perror("Error select\n");
            exit(1);
        } else if (res == 0) {
            sleep(1);
            continue;
        }

        if (!FD_ISSET(server_socket, &tmp_fds))
            continue;

        client_socket = accept(server_socket, (struct sockaddr *) &client_address, (socklen_t * ) & adr_len);
        if (client_socket < 0) {
            printf("Ошибка при подключении клиента\n");
            continue;
        }

        if (count_thread >= MAX_THREAD) {
            printf("Превышено максимальное количество клиентов\n");
            close(client_socket);
            continue;
        }

        pthread_create(&clients[count_thread], NULL, handle_client, (void *) &client_socket);
        count_thread++;
        printf("Count clients: %d\n", count_thread - 1);
    }
    pthread_exit(NULL);
}


int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Use: %s <path_to_server>\n", argv[0]);
        exit(-1);
    }

    if (argv[1]) {
        strcpy(path_to_server, argv[1]);
        chdir(path_to_server);
        getcwd(path_to_server, BUF_SIZE);
    }
    strcpy(rood, path_to_server);

    struct server_args args;
    struct sockaddr_in server_address, client_address;
    args.adr_len = sizeof(client_address);
    pthread_t server;

    args.socket = socket(AF_INET, SOCK_STREAM, 0);
    if (args.socket == -1) {
        perror("Socket creation failed");
        return 1;
    }
    int reuse = 1;
    setsockopt(args.socket, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = INADDR_ANY;
    server_address.sin_port = htons(PORT);

    if (bind(args.socket, (struct sockaddr *) &server_address, sizeof(server_address)) == -1) {
        perror(strerror(errno));
        return 1;
    }
    if (listen(args.socket, MAX_THREAD) == -1) {
        perror("Listening failed");
        return 1;
    }

    printf("Server started. Waiting for incoming connections...\n");
    printf("path_to_server - %s\n\n", path_to_server);
    printf("%s\n", name_server);
    printf("Максимальное количество подключений: 4\n");
    printf("Для завершения работы сервера нужно ввести \"q\"\n");

    pthread_create(&server, NULL, handle_server, (void *) &args);

    char end;
    while (1) {
        end = getchar();
        if (end == 'q')
            break;
    }
    server_free = true;
    printf("Server shutdown\n");
    pthread_join(server, NULL);
    close(args.socket);

    return 0;
}