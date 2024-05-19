#ifndef FTPSSERVER_AUTHENTIFICATION_H
#define FTPSSERVER_AUTHENTIFICATION_H

#include <iostream>
#include <vector>
#include <string>
#include <openssl/ssl.h>
#include <fstream>

struct User {
    std::string username;
    std::string password;
};

extern std::vector<User> users;

bool authenticateUserSSL(SSL* ssl);
void addUser(const std::string& username, const std::string& password);
void saveUsersToFile();
void loadUsersFromFile();

#endif //FTPSSERVER_AUTHENTIFICATION_H