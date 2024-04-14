#ifndef COMMAND_HANDLER_H
#define COMMAND_HANDLER_H

#include "../include/file_handler.h"
#include "../include/error_handler.h"

void handle_command(int connfd, char* command, int data_port, const char* root_dir);

#endif /* COMMAND_HANDLER_H */
