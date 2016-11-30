cmake_minimum_required(VERSION 3.6)
project(compressie)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")

set(SOURCE_FILES src/algemeen/comprimeer1.h src/algemeen/comprimeer1.c src/algemeen/linkedlist.h src/algemeen/linkedlist.c src/algemeen/huffman_tree.h src/algemeen/huffman_tree.c)
add_executable(algemeen ${SOURCE_FILES})

set(SOURCE_FILES src/specifiek/comprimeer2.c src/specifiek/comprimeer2.h)
add_executable(specifiel ${SOURCE_FILES})

set(SOURCE_FILES tests/generate_test_files.c tests/generate_test_files.h tests/correctness_test.c tests/correctness_test.h)
add_executable(tests ${SOURCE_FILES})