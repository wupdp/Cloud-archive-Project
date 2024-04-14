#include "../include/file_handler.h"

void handle_ls_command(int connfd, int data_port, const char* root_dir) {
    // Реализация команды ls
}

void handle_pwd_command(int connfd) {
    // Реализация команды pwd
}

void handle_cd_command(int connfd, const char* directory) {
    // Реализация команды cd
}

void handle_put_command(int connfd, int data_port, const char* filename) {
    // Реализация команды put
}

void handle_get_command(int connfd, int data_port, const char* filename) {
    // Реализация команды get
}
