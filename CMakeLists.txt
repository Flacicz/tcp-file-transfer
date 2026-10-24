cmake_minimum_required(VERSION 4.3)
project(tcp_file_transfer C)

set(CMAKE_C_STANDARD 11)
add_compile_options(-Wall -Wextra -Wpedantic)

add_executable(client client/main.c)

add_executable(server server/main.c)
