cmake_minimum_required(VERSION 3.6)
project(compressie)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")

set(SOURCE_FILES main.c comprimeer1.h comprimeer1.c linkedlist.h linkedlist.c huffman_tree.h huffman_tree.c comprimeer2.c comprimeer2.h)
add_executable(compressie ${SOURCE_FILES})