cmake_minimum_required(VERSION 4.2)
project(code C)

set(CMAKE_C_STANDARD 11)

add_executable(code main.c
        ProblemOne.c
        ProblemOne.h
        ProblemTwo.c
        ProblemTwo.h)
