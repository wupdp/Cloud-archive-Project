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

void read_input(char* buffer, int size)
{
    char* nl = NULL;
    memset(buffer, 0, size);

    if (fgets(buffer, size, stdin) != NULL ) {
        nl = strchr(buffer, '\n');
        if (nl) *nl = '\0'; //ovewriting newline
    }
}

int read_command_client(char* buf, int size, struct command *cstruct)
{
    memset(cstruct->code, 0, sizeof(cstruct->code));
    memset(cstruct->arg, 0, sizeof(cstruct->arg));

    printf("client-input> ");	// prompt for input
    fflush(stdout);

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
    char buff[BUFFER_SIZE], check[BUFFER_SIZE]="1", port[BUFFER_SIZE];
    SSL_read(ssl, port, BUFFER_SIZE);
    while(strcmp("1", check) == 0) {
        SSL_read(ssl, check, BUFFER_SIZE);
        if(strcmp("0", check) == 0)
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

void client_get_file(SSL* ssl, char* server_ip, struct command cmd) {
    char port[BUFFER_SIZE], buffer[BUFFER_SIZE];
    char char_num_blks[BUFFER_SIZE], char_num_last_blk[BUFFER_SIZE], message[BUFFER_SIZE];
    int num_blks, num_last_blk, i;
    FILE* fp;
    SSL_read(ssl, port, BUFFER_SIZE);
    SSL_read(ssl, message, BUFFER_SIZE);
    if (strcmp("1", message) == 0) {
        if ((fp = fopen(cmd.arg, "w")) == NULL)
            cout << "Error in creating file\n";
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
            cout << "File <" << cmd.arg << "> downloaded successfully." << endl;
        }
    }
    else {
        cerr << "Error in opening file. Check filename\nUsage: put filename" << endl;
    }
}

void client_put_file(SSL* ssl, char* server_ip, struct command cmd) {
    char port[BUFFER_SIZE], buffer[BUFFER_SIZE], char_num_blks[BUFFER_SIZE];
    char char_num_last_blk[BUFFER_SIZE];
    int  lSize, num_blks, num_last_blk;
    FILE* fp;
    SSL_read(ssl, port, BUFFER_SIZE);
    if ((fp = fopen(cmd.arg, "r")) != NULL) {
        SSL_write(ssl, "1", BUFFER_SIZE);
        fseek(fp, 0, SEEK_END);
        lSize = ftell(fp);
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
        cout << "File " << cmd.arg << " uploaded successfully.\n";
    }
    else {
        SSL_write(ssl, "0", BUFFER_SIZE);
        cerr << "Error in opening file. Check filename\nUsage: put filename" << endl;
    }
}

void client_cd_action(SSL* ssl, struct command cmd) {
    char check[BUFFER_SIZE];
    cout << "Given path: " << cmd.arg << endl;
    SSL_read(ssl, check, BUFFER_SIZE);
    if (strcmp("0", check) == 0) {
        cerr << "Directory doesn't exist. Check Path" << endl;
    } else {
        cout << "Directory changed successfully" << endl;
    }
}

void client_pwd_action(SSL* ssl) {
    char current_directory[BUFFER_SIZE];
    SSL_read(ssl, current_directory, BUFFER_SIZE);
    cout << current_directory << endl;
}