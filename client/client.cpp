#include "client.h"

using namespace std;

void menu_client() {
    printf("\nWelcome to FTP-client!\n\n");
    printf("Command that you can use:\n");
    printf("ls                  --      check file that you can get\n");
    printf("get <file-name>     --      get file from server\n");
    printf("put <file-name>     --      send file to server\n");
    printf("pwd                 --      show current directory name\n");
    printf("cd <directory-name> --      go to different directory\n");
    printf("cd ..               --      go to the directory above\n");
    printf("quit                --      shutdown\n");
}

void read_input(char *buffer, int size) {
    char *nl = NULL;
    memset(buffer, 0, size);

    if (fgets(buffer, size, stdin) != NULL) {
        nl = strchr(buffer, '\n');
        if (nl) *nl = '\0'; //ovewriting newline
    }
}

int read_command_client(char *buf, int size, struct command *cstruct) {
    memset(cstruct->code, 0, sizeof(cstruct->code));
    memset(cstruct->arg, 0, sizeof(cstruct->arg));

    printf("client-input> ");    // prompt for input
    fflush(stdout);

    // wait for user to enter a command
    read_input(buf, size);
    size_t len = strlen(buf);
    char *com = (char *) malloc((len + 1) * sizeof(char));
    strcpy(com, buf);

    char *arg = NULL;
    arg = strtok(buf, " ");
    arg = strtok(NULL, " ");

    if (arg != NULL) {
        // store the argument if there is one
        strncpy(cstruct->arg, arg, strlen(arg));
    }

    // buf = command
    if (strcmp(buf, "ls") == 0) {
        strcpy(cstruct->code, "LS");
        strcpy(cstruct->serv_code, "ls\n");
    } else if (strcmp(buf, "get") == 0) {
        strcpy(cstruct->code, "GET");
        strcpy(cstruct->serv_code, com);
    } else if (strcmp(buf, "put") == 0) {
        strcpy(cstruct->code, "PUT");
        strcpy(cstruct->serv_code, com);
    } else if (strcmp(buf, "quit") == 0) {
        strcpy(cstruct->code, "QUIT");
        strcpy(cstruct->serv_code, "quit\n");
    } else if (strcmp(buf, "cd") == 0) {
        strcpy(cstruct->code, "CD");
        strcpy(cstruct->serv_code, com);
    } else if (strcmp(buf, "pwd") == 0) {
        strcpy(cstruct->code, "PWD");
        strcpy(cstruct->serv_code, "pwd\n");
    } else {
        return -1;
    }

    // store code in beginning of buffer
    memset(buf, 0, 400);
    strcpy(buf, cstruct->code);

    // if there's an arg, append it to the buffer
    if (arg != NULL) {
        strcat(buf, " ");
        strncat(buf, cstruct->arg, strlen(cstruct->arg));
    }
    return 0;
}

void client_show_list(SSL *ssl) {
    char buff[BUFFER_SIZE], check[BUFFER_SIZE] = "1", port[BUFFER_SIZE];
    SSL_read(ssl, port, BUFFER_SIZE);
    while (strcmp("1", check) == 0) {
        SSL_read(ssl, check, BUFFER_SIZE);
        if (strcmp("0", check) == 0)
            break;
        while (1) {
            int bytes = SSL_read(ssl, buff, BUFFER_SIZE - 1);
            if (bytes <= 0)
                break;
            buff[bytes] = 0;
            if (buff[0] == '\0') // Если получен специальный символ, прекращаем чтение
                break;
            cout << buff;
        }
    }
}

void client_get_file(SSL *ssl, char *server_ip, struct command cmd) {
    SSL_write(ssl, cmd.arg, strlen(cmd.arg) + 1);

    char message[BUFFER_SIZE];
    SSL_read(ssl, message, sizeof(message));

    if (strcmp("1", message) == 0) {
        char port[BUFFER_SIZE], buffer[MAXLINE];
        char char_num_blks[BUFFER_SIZE], char_num_last_blk[BUFFER_SIZE];
        int data_port, datasock, num_blks, num_last_blk, i;
        FILE *fp;

        // Читаем порт и сообщение о статусе файла
        SSL_read(ssl, port, sizeof(port));

        data_port = atoi(port);
        datasock = create_socket(data_port, server_ip);

        SSL_read(ssl, message, sizeof(message));

        if (strcmp("1", message) == 0) {
            // Создаем файл для записи
            if ((fp = fopen(cmd.arg, "wb")) != NULL) {
                // Читаем количество блоков данных
                SSL_read(ssl, char_num_blks, sizeof(char_num_blks));
                num_blks = atoi(char_num_blks);

                // Читаем и записываем блоки данных в файл
                for (i = 0; i < num_blks; i++) {
                    recv(datasock, buffer, MAXLINE, 0);
                    fwrite(buffer, sizeof(char), MAXLINE, fp);
                }

                // Читаем размер последнего блока данных
                SSL_read(ssl, char_num_last_blk, sizeof(char_num_last_blk));
                num_last_blk = atoi(char_num_last_blk);
                if (num_last_blk > 0) {
                    // Читаем и записываем последний блок данных в файл
                    recv(datasock, buffer, num_last_blk, 0);
                    fwrite(buffer, sizeof(char), num_last_blk, fp);
                }
                fclose(fp);
                std::cout << "File <" << cmd.arg << "> downloaded successfully." << std::endl;
            } else {
                std::cerr << "Error in creating file" << std::endl;
            }
        } else {
            std::cerr << "Error in opening file. Check filename" << std::endl;
        }
        close(datasock);
    } else {
        std::cerr << "File <" << cmd.arg << "> does not exist" << std::endl;
    }
}

void client_put_file(SSL *ssl, char *server_ip, struct command cmd) {
    SSL_write(ssl, cmd.arg, strlen(cmd.arg) + 1);

    char port[BUFFER_SIZE], buffer[BUFFER_SIZE], char_num_blks[BUFFER_SIZE];
    char char_num_last_blk[BUFFER_SIZE];
    int lSize, num_blks, num_last_blk, i;
    FILE *fp;

    // Чтение ответа от сервера
    char server_response[BUFFER_SIZE];
    SSL_read(ssl, server_response, sizeof(server_response));
    if (strcmp(server_response, "ERROR") == 0) {
        std::cerr << "Server reported an error: " << server_response << std::endl;
        return;
    }

    SSL_read(ssl, port, sizeof(port));
    int data_port = atoi(port);
    int datasock = create_socket(data_port, server_ip);
    if (datasock < 0) {
        std::cerr << "Failed to create data socket" << std::endl;
        SSL_write(ssl, "ERROR", strlen("ERROR") + 1);
        return;
    } else {
        SSL_write(ssl, "SUCCESS", strlen("SUCCESS") + 1);
    }

    SSL_read(ssl, server_response, sizeof(server_response));
    if (strcmp(server_response, "ERROR") == 0) {
        std::cerr << "Server failed to accept data connection" << std::endl;
        return;
    }

    fp = fopen(cmd.arg, "rb");
    if (fp != NULL) {
        SSL_write(ssl, "1", strlen("1") + 1);
        SSL_read(ssl, server_response, sizeof(server_response));
        if (strcmp(server_response, "ERROR") == 0) {
            std::cerr << "Error creating file on server side" << std::endl;
            return;
        }

        fseek(fp, 0, SEEK_END);
        lSize = ftell(fp);
        rewind(fp);
        num_blks = lSize / BUFFER_SIZE;
        num_last_blk = lSize % BUFFER_SIZE;

        sprintf(char_num_blks, "%d", num_blks);
        SSL_write(ssl, char_num_blks, strlen(char_num_blks) + 1);

        // Чтение и отправка содержимого файла по блокам
        for (i = 0; i < num_blks; i++) {
            fread(buffer, sizeof(char), BUFFER_SIZE, fp);
            send(datasock, buffer, BUFFER_SIZE, 0);
        }

        // Отправка последнего блока данных
        sprintf(char_num_last_blk, "%d", num_last_blk);
        SSL_write(ssl, char_num_last_blk, strlen(char_num_last_blk) + 1);

        if (num_last_blk > 0) {
            fread(buffer, sizeof(char), num_last_blk, fp);
            send(datasock, buffer, num_last_blk, 0);
        }
        fclose(fp);
    } else {
        SSL_write(ssl, "0", strlen("0") + 1);
        std::cerr << "Error in opening file. Check filename" << std::endl;
    }
    close(datasock);
}


void client_cd_action(SSL *ssl, struct command cmd) {
    char response[MAXLINE];
    // Формируем команду cd для отправки на сервер
    SSL_write(ssl, cmd.arg, strlen(cmd.arg));
    // Получаем ответ от сервера
    SSL_read(ssl, response, MAXLINE);
    // Проверяем ответ от сервера и выводим соответствующее сообщение
    if (strcmp(response, "0") == 0) {
        std::cerr << "Error: Invalid path" << std::endl;
    } else {
        std::cout << "Directory changed successfully" << std::endl;
    }
}

void client_pwd_action(SSL *ssl) {
    char current_directory[BUFFER_SIZE];
    SSL_read(ssl, current_directory, BUFFER_SIZE);
    cout << current_directory << endl;
}

int create_socket(int port, char *addr) {
    int sockfd;
    struct sockaddr_in servaddr;

    //Create a socket for the client
    //If sockfd<0 there was an error in the creation of the socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        cerr << "Problem in creating the socket" << endl;
        return -1;
    }

    //Creation of the socket
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = inet_addr(addr);
    servaddr.sin_port = htons(port); //convert to big-endian order

    //Connection of the client to the socket
    if (connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
        cerr << "Problem in creating data channel" << endl;
        return -2;
    }

    return sockfd;
}