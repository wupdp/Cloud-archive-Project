#include "../include/authentification.h"
#include <cstring>

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
        SSL_write(ssl, "Authentication passed", strlen("Authentication passed"));
        return true;
    } else {
        SSL_write(ssl, "Authentication failed", strlen("Authentication failed"));
        return false;
    }
}

void saveUsersToFile() {
    const std::string filename = "/mnt/c/users/настя/desktop/curs/cloud-archive-project/server/users.txt";
    //FILE* fd = fopen("users.txr", "r");
    std::ofstream outFile(filename); // Открыть файл для записи
    if (!outFile) {
        std::cerr << "Error opening file for writing: " << filename << std::endl;
        return;
    }

    for (const auto& user : users) {
        outFile << user.username << " " << user.password << std::endl;
    }

    outFile.close(); // Закрыть файл
    if (!outFile) {
        std::cerr << "Error closing file: " << filename << std::endl;
    }
}

void loadUsersFromFile() {
    const std::string filename = "/mnt/c/users/настя/desktop/curs/cloud-archive-project/server/users.txt";
    std::ifstream inFile(filename); // Открыть файл для чтения
    if (!inFile) {
        std::cerr << "Error opening file for reading: " << filename << std::endl;
        return;
    }

    users.clear(); // Очистить текущий список пользователей

    std::string username;
    std::string password;
    while (inFile >> username >> password) { // Чтение пар username и password
        users.push_back({username, password});
    }

    inFile.close(); // Закрыть файл
    if (!inFile) {
        std::cerr << "Error closing file: " << filename << std::endl;
    }
}