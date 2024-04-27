#include "../include/authentification.h"

std::vector<User> users;

void addUser(const std::string& username, const std::string& password) {
    users.push_back({username, password});
}

bool authenticateUser(const std::string& username, const std::string& password) {
    for (const auto& user : users) {
        if (user.username == username && user.password == password) {
            return true;
        }
    }
    return false;
}

bool authenticateUserSSL(SSL* ssl) {
    char username[256];
    char password[256];

    int bytes_read = SSL_read(ssl, username, sizeof(username));
    if (bytes_read <= 0) {
        SSL_write(ssl, "Error reading username", strlen("Error reading username"));
        return false;
    }
    username[bytes_read] = '\0';

    bytes_read = SSL_read(ssl, password, sizeof(password));
    if (bytes_read <= 0) {
        SSL_write(ssl, "Error reading password", strlen("Error reading password"));
        return false;
    }
    password[bytes_read] = '\0';

    if (authenticateUser(username, password)) {
        return true;
    } else {
        SSL_write(ssl, "Authentication failed", strlen("Authentication failed"));
        return false;
    }
}