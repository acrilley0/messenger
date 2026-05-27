set(CMAKE_CXX_STANDARD 23)
cmake_minimum_required(VERSION 3.15)
project(tcp_networking_example)

add_executable(tcp_client TCPClient.cpp)
add_executable(tcp_server TCPServer.cpp)
