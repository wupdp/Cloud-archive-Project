#include "client.h"

using namespace std;

void print_menu() {
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    attron(COLOR_PAIR(1));
    printw("\nWelcome to FTP-client!\n\n");
    printw("Command that you can use:\n");
    printw("ls                  --      check file that you can get\n");
    printw("get <file-name>     --      get file from server\n");
    printw("put <file-name>     --      send file to server\n");
    printw("pwd                 --      show current directory name\n");
    printw("cd <directory-name> --      go to different directory\n");
    printw("cd ..               --      go to the directory above\n");
    printw("quit                --      shutdown\n");
    attroff(COLOR_PAIR(1));
    refresh();
}

void read_input(char* buffer, int size)
{
    char* nl = NULL;
    memset(buffer, 0, size);

    if (getnstr(buffer, size - 1) != ERR) {
        nl = strchr(buffer, '\n');
        if (nl) *nl = '\0'; //ovewriting newline
    }
}

int read_command_client(char* buf, int size, struct command *cstruct)
{
    memset(cstruct->code, 0, sizeof(cstruct->code));
    memset(cstruct->arg, 0, sizeof(cstruct->arg));

    move(getcury(stdscr) + 1, 0); // Перемещение курсора в начало строки
    init_pair(2, COLOR_BLUE, COLOR_BLACK);
    attron(COLOR_PAIR(2));
    printw("client-input> ");
    attroff(COLOR_PAIR(2));
    refresh(); // Обновляем экран

    //fflush(stdout);

    // wait for user to enter a command
    read_input(buf, size);
    size_t len = strlen(buf);
    char *com = (char *)malloc((len + 1) * sizeof(char));
    strcpy(com, buf);

    char *arg = NULL;
    arg = strtok (buf," ");
    arg = strtok (NULL, " ");

    if (arg != NULL){
        // store the argument if there is one
        strncpy(cstruct->arg, arg, strlen(arg));

    }

    // buf = command
    if (strcmp(buf, "ls") == 0) {
        strcpy(cstruct->code, "LS");
        strcpy(cstruct->serv_code, "ls\n");
    }
    else if (strcmp(buf, "get") == 0) {
        strcpy(cstruct->code, "GET");
        strcpy(cstruct->serv_code, com);
    }
    else if (strcmp(buf, "put") == 0) {
        strcpy(cstruct->code, "PUT");
        strcpy(cstruct->serv_code, com);
    }
    else if (strcmp(buf, "quit") == 0) {
        strcpy(cstruct->code, "QUIT");
        strcpy(cstruct->serv_code, "quit\n");
    }
    else if (strcmp(buf, "cd") == 0) {
        strcpy(cstruct->code, "CD");
        strcpy(cstruct->serv_code, com);
    }
    else if (strcmp(buf, "pwd") == 0) {
        strcpy(cstruct->code, "PWD");
        strcpy(cstruct->serv_code, "pwd\n");
    }
    else if (strcmp(buf, "!ls") == 0) {
        strcpy(cstruct->code, "!LS");
        strcpy(cstruct->serv_code, "!ls\n");
    }
    else if (strcmp(buf, "!cd") == 0) {
        strcpy(cstruct->code, "!CD");
        strcpy(cstruct->serv_code, com);
    }
    else if (strcmp(buf, "!pwd") == 0) {
        strcpy(cstruct->code, "!PWD");
        strcpy(cstruct->serv_code, "!pwd\n");
    }
    else if (strcmp(buf, "help") == 0) {
        strcpy(cstruct->code, "HELP");	
        strcpy(cstruct->serv_code, "!help\n");
    }
    else {
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

void client_show_list(SSL* ssl, char* server_ip) {
    char buff[BUFFER_SIZE], check[BUFFER_SIZE] = "1";

    int row = 0;
    int col = 0;

    while (strcmp("1", check) == 0) {
        SSL_read(ssl, check, BUFFER_SIZE);
        if (strcmp("0", check) == 0)
            break;

        while (1) {
            int bytes = SSL_read(ssl, buff, BUFFER_SIZE - 1);
            if (bytes <= 0)
                break;

            buff[bytes] = '\0';
            if (buff[0] == '\0') // Если получен специальный символ, прекращаем чтение
                break;

            if (col > 100) { // Максимальная ширина, после которой начинаем новую строку
                col = 0;
                row++;
            }

            mvprintw(row, col, "%s", buff);
            col += 40; // Ширина одной "колонки"
            refresh();
        }
        printw("\n");
        refresh();
    }
}


void client_get_file(SSL* ssl, char* server_ip, struct command cmd) {
    char port[BUFFER_SIZE], buffer[BUFFER_SIZE];
    char char_num_blks[BUFFER_SIZE], char_num_last_blk[BUFFER_SIZE], message[BUFFER_SIZE];
    int num_blks, num_last_blk, i, data_port, datasock;
    FILE* fp;

    SSL_read(ssl, port, BUFFER_SIZE);
    data_port = atoi(port);

    SSL_read(ssl, message, BUFFER_SIZE);
    
    if (strcmp("1", message) == 0) {
        if ((fp = fopen(cmd.arg, "w")) == NULL){
            move(getcury(stdscr)+1, 0);
            printw("Error in creating file\n");
            refresh();
        }

        else {
            SSL_read(ssl, char_num_blks, BUFFER_SIZE);

            num_blks = atoi(char_num_blks);
            for(i = 0; i < num_blks; i++) {
                SSL_read(ssl, buffer, BUFFER_SIZE);
                fwrite(buffer, sizeof(char), BUFFER_SIZE, fp);
            }
            
            SSL_read(ssl, char_num_last_blk, BUFFER_SIZE);
            num_last_blk = atoi(char_num_last_blk);
            if (num_last_blk > 0) {
                SSL_read(ssl, buffer, BUFFER_SIZE);
                fwrite(buffer, sizeof(char), num_last_blk, fp);
            }
            fclose(fp);
            move(getcury(stdscr)+1, 0);
            printw("File %s downloaded successfully.\n", cmd.arg);
            refresh();
        }
    }
    else {
        move(getcury(stdscr)+1, 0);
        printw("Error in opening file. Check filename.");
        refresh();
        move(getcury(stdscr)+1, 0);
        printw("Usage: get filename");
        refresh();
    }
}

void client_put_file(SSL* ssl, char* server_ip, struct command cmd) {
    char port[BUFFER_SIZE], buffer[BUFFER_SIZE], char_num_blks[BUFFER_SIZE];
    char char_num_last_blk[BUFFER_SIZE];
    int  num_blks, num_last_blk, bytes_read;
    FILE* fp;
    SSL_read(ssl, port, BUFFER_SIZE);

    move(getcury(stdscr)+1, 0);
    printw("smth");
    refresh();

    if ((fp = fopen(cmd.arg, "r")) != NULL) {
        SSL_write(ssl, "1", 1024);

        fseek(fp, 0, SEEK_END);
        long lSize = ftell(fp);
        rewind(fp);
        num_blks = lSize / BUFFER_SIZE;
        num_last_blk = lSize % BUFFER_SIZE;
        sprintf(char_num_blks, "%d", num_blks);
        SSL_write(ssl, char_num_blks, BUFFER_SIZE);
        for(int i = 0; i < num_blks; i++) {
            fread(buffer, sizeof(char), BUFFER_SIZE, fp);
            SSL_write(ssl, buffer, BUFFER_SIZE);
        }
        sprintf(char_num_last_blk, "%d", num_last_blk);
        SSL_write(ssl, char_num_last_blk, BUFFER_SIZE);
        if (num_last_blk > 0) {
            fread(buffer, sizeof(char), num_last_blk, fp);
            SSL_write(ssl, buffer, BUFFER_SIZE);
        }
        fclose(fp);
        move(getcury(stdscr) + 1, 0); 
        printw("File %s uploaded successfully.\n", cmd.arg);
        refresh();
    }
    else {
        SSL_write(ssl, "0", BUFFER_SIZE);
        move(getcury(stdscr) + 1, 0); 
        printw("Error in opening file. Check filename\nUsage: put filename");
        refresh();
    }
}

void client_cd_action(SSL* ssl, struct command cmd) {
    char check[BUFFER_SIZE];

    move(getcury(stdscr) + 1, 0); // Перемещение курсора в начало строки
    printw("Given path: %s\n", cmd.arg); // Выводим информацию о заданном пути
    refresh(); // Обновляем экран

    SSL_read(ssl, check, BUFFER_SIZE);
    move(getcury(stdscr) + 1, 0);
    printw("%s", check);
    refresh();
    if (strcmp("2", check) == 0) {
        move(getcury(stdscr) + 1, 0); 
        printw("Directory doesn't exist. Check Path\n"); 
        refresh(); 
    } else {
        move(getcury(stdscr) + 1, 0); // Перемещаем курсор на следующую строку
        printw("Directory changed successfully\n"); // Выводим сообщение об успешном изменении директории
        refresh(); // Обновляем экран
    }
}

void client_pwd_action(SSL* ssl) {
    char current_directory[BUFFER_SIZE];
    int bytes_read = SSL_read(ssl, current_directory, sizeof(current_directory));
    current_directory[bytes_read] = '\0';
    move(getcury(stdscr) + 1, 0);
    printw("%s\n", current_directory);
    refresh();
}

int authentification(SSL* ssl) {
    init_pair(3, COLOR_CYAN, COLOR_BLACK);
    attron(COLOR_PAIR(3));
    move(getcury(stdscr), 0); // Перемещение курсора в начало строки
    printw("-----Authentification-----");
    attroff(COLOR_PAIR(3));
    refresh();

    char username[256];
    char password[256];

    char buf[BUFFER_SIZE];

    int size_user;
    init_pair(4, COLOR_GREEN, COLOR_BLACK);
    attron(COLOR_PAIR(4));
    move(getcury(stdscr) + 2, 0); // Перемещение курсора в начало строки
    printw("username: ");
    attroff(COLOR_PAIR(4));
    refresh();

    read_input(username, sizeof username);

    int size_pass;
    attron(COLOR_PAIR(4));
    move(getcury(stdscr) + 1, 0); // Перемещение курсора в начало строки
    printw("password: ");
    attroff(COLOR_PAIR(4));
    refresh();
    noecho();
    read_input(password, sizeof password);
    echo();

    if (SSL_write(ssl, username, strlen(username)) <= 0) {
        return 1;
    }

    // Send password
    if (SSL_write(ssl, password, strlen(password)) <= 0) {
        return 1;
    }
    
    SSL_read(ssl, buf, BUFFER_SIZE);

    move(getcury(stdscr) + 1, 0); // Перемещение курсора в начало строки
    printw("%s\n", buf);
    refresh();

    if (strcmp(buf, "Authentication failed") == 0) {
        move(getcury(stdscr) + 1, 0); // Перемещение курсора в начало строки
        printw("---Adding new user---");
        refresh();

        SSL_write(ssl, username, strlen(username));
        SSL_write(ssl, password, strlen(password));

        char buffer[BUFFER_SIZE];
        SSL_read(ssl, buffer, BUFFER_SIZE);
        
        return 1;
    }

    return 0;
}