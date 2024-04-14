#include "../include/command_handler.h"

void handle_command(int connfd, char* command, int data_port, const char* root_dir) {
    char* token = strtok(command, " \n");

    if (token == nullptr) {
        send_error_message(connfd, "Invalid command");
        return;
    }

    if (strcmp("quit", token) == 0) {
        cout << "Client has quit" << endl;
    }
    else if (strcmp("ls", token) == 0) {
        handle_ls_command(connfd, data_port, root_dir);
    }
    else if (strcmp("pwd", token) == 0) {
        handle_pwd_command(connfd);
    }
    else if (strcmp("cd", token) == 0) {
        token = strtok(nullptr, " \n");
        handle_cd_command(connfd, token);
    }
    else if (strcmp("put", token) == 0) {
        token = strtok(nullptr, " \n");
        handle_put_command(connfd, data_port, token);
    }
    else if (strcmp("get", token) == 0) {
        token = strtok(nullptr, " \n");
        handle_get_command(connfd, data_port, token);
    }
    else {
        send_error_message(connfd, "Invalid command");
    }
}
