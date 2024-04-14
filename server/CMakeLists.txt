cmake_minimum_required(VERSION 3.10)
project(server)

set(CMAKE_CXX_STANDARD 17)

add_executable(server src/server.cpp src/command_handler.cpp src/file_handler.cpp src/error_handler.cpp src/socket_utils.cpp)

# Поиск библиотеки для сокетов
find_package(Threads REQUIRED)
target_link_libraries(server PRIVATE Threads::Threads)