cmake_minimum_required(VERSION 3.0)
project(FTPSclient)  # Замените "YourProjectName" на имя вашего проекта

# Найти пакет OpenSSL
find_package(OpenSSL REQUIRED)

# Добавить исполняемый файл
add_executable(client main.cpp
        client.cpp)

# Связать библиотеки OpenSSL с вашим исполняемым файлом
target_link_libraries(client OpenSSL::SSL OpenSSL::Crypto)