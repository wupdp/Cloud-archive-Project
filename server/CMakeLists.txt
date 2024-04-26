cmake_minimum_required(VERSION 3.0)
project(FTPSserver)  # Замените "YourProjectName" на имя вашего проекта

# Найти пакет OpenSSL
find_package(OpenSSL REQUIRED)

# Добавить исполняемый файл
add_executable(server src/main.cpp
        src/server.cpp
        src/ftp_commands.cpp
        src/socket.cpp
        src/utilities.cpp)

# Связать библиотеки OpenSSL с вашим исполняемым файлом
target_link_libraries(server OpenSSL::SSL OpenSSL::Crypto)
