#ifndef FTPSSERVER_AUTHENTIFICATION_H
#define FTPSSERVER_AUTHENTIFICATION_H

#include <iostream>
#include <vector>
#include <string>
#include <openssl/ssl.h>

struct User {
    std::string username;
    std::string password;
};

bool authenticateUserSSL(SSL* ssl);
void addUser(const std::string& username, const std::string& password);

#endif //FTPSSERVER_AUTHENTIFICATION_H
