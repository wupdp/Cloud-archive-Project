#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

void handle_ls_command(int connfd, int data_port, const char* root_dir);
void handle_pwd_command(int connfd);
void handle_cd_command(int connfd, const char* directory);
void handle_put_command(int connfd, int data_port, const char* filename);
void handle_get_command(int connfd, int data_port, const char* filename);

using namespace std;

#endif /* FILE_HANDLER_H */
