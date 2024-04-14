#include "../include/error_handler.h"

void send_error_message(int connfd, const char* message) {
    cout << "Error: " << message << endl;
    send(connfd, message, strlen(message), 0);
}