cmake_minimum_required(VERSION 3.27)

project(udp_demo LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(udp_server udp_server.cpp)
add_executable(udp_client udp_client.cpp)

target_compile_options(udp_server PRIVATE -Wall -Wextra)
target_compile_options(udp_client PRIVATE -Wall -Wextra)