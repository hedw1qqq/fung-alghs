cmake_minimum_required(VERSION 3.21)
project(task4 C)

set(CMAKE_C_STANDARD 11)

add_executable(task4 main.c
        funcs.c
        funcs.h)
target_link_libraries(task4 m)