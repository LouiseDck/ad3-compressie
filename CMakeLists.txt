cmake_minimum_required(VERSION 3.6)
project(compressie)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")

set(SOURCE_FILES src/comprimeer1.h src/comprimeer1.c src/linkedlist.h src/linkedlist.c src/huffman_tree.h src/huffman_tree.c)
add_executable(algemeen ${SOURCE_FILES})

set(SOURCE_FILES src/comprimeer2.c src/comprimeer2.h)
add_executable(specifiel ${SOURCE_FILES})