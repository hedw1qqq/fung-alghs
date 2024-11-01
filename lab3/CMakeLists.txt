cmake_minimum_required(VERSION 3.27)
project(lab3 C)

set(CMAKE_C_STANDARD 11)

add_executable(lab3t1 task1/main.c
        task1/funcs.h
        task1/funcs.c)

add_executable(lab3t5 task5/main.c)

add_executable(lab3t2 task2/main.c)

add_executable(lab3t3 task3/main.c)



add_executable(lab3t8 task8/main.c)

add_executable(lab3t10 task10/main.c)

add_executable(lab3t7 task7/main.c)

add_executable(lab3t4 task4/main.c
        task4/string.c
        task4/string.h
        task4/app.c
        task4/app.h)

