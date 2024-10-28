cmake_minimum_required(VERSION 3.27)
project(lab3 C)

set(CMAKE_C_STANDARD 11)

add_executable(lab3t1 task1/main.c
        task1/funcs.h
        task1/funcs.c)


add_executable(lab3t2 task2/main.c
)
add_executable(lab3t3 task3/main.c
)