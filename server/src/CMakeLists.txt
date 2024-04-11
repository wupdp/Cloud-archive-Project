cmake_minimum_required(VERSION 3.10)
project(server)

set(CMAKE_CXX_STANDARD 11)

# Добавляем исполняемый файл сервера
add_executable(server main.cpp classes/server.cpp classes/network.cpp classes/connection.cpp classes/command.cpp)

# Указываем, что нужно подключить библиотеки для работы с сетью
target_link_libraries(server PRIVATE -lsocket -lnsl)
