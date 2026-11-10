cmake_minimum_required(VERSION 3.16)
project(ChefWaiterSystem C)

set(CMAKE_C_STANDARD 11)

find_package(Threads REQUIRED)

add_executable(ChefWaiterSystem main.c)

target_link_libraries(ChefWaiterSystem PRIVATE Threads::Threads)
