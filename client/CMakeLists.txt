cmake_minimum_required(VERSION 3.0)
project(FTPSclient)

# Set the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the -Wall, -g, and -Os flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -g -Os")

# Find the OpenSSL package
find_package(OpenSSL REQUIRED)

# Add the executable
add_executable(client main.cpp client.cpp)

# Link the OpenSSL libraries with your executable
target_link_libraries(client OpenSSL::SSL OpenSSL::Crypto)