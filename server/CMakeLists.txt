cmake_minimum_required(VERSION 3.10)
project(CloudArchiveServer)

set(CMAKE_CXX_STANDARD 14)

add_executable(CloudArchiveServer src/main.cpp src/server.cpp src/socket.cpp src/ftp_commands.cpp src/utilities.cpp)

target_include_directories(CloudArchiveServer PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)