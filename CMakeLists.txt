cmake_minimum_required(VERSION 3.25)
project(file_sender C)

set(CMAKE_C_STANDARD 11)

add_executable(file_sender serverDir/server.c)
