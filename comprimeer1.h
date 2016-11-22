//
// Created by louise on 05/10/16.
//

#ifndef COMPRESSIE_COMPRIMEER1_H
#define COMPRESSIE_COMPRIMEER1_H

#include "linkedlist.h"
#include <stdio.h>

char* read_file(const char* filename);

void encodeer(char *text, const char* filename);
List* make_freq_list(char* text, int* number_not_zero);
encode_Item* make_encoding_huffman_tree(encode_Item* encode_array, List* list);
char* encode_text(int aantal_char, const char* text, encode_Item* encode_array, int* cur_size, char** code);
void write_tree(const char* filename, Leaf* parent_leaf, char* buffer);
void write_encoded_text();

void decodeer(const char* filename, const char* output_filename);
Leaf* read_huffman_tree(FILE *fp);
char* read_text(FILE *fp);
char* decode_text(char* text, Leaf* leaf, int aantal);
void write_text(char* decoded_text, const char* output_file);


#endif //COMPRESSIE_COMPRIMEER1_H
