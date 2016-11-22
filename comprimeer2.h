//
// Created by louise on 21/11/16.
//
#include "stdio.h"

#ifndef COMPRESSIE_COMPRIMEER2_H
#define COMPRESSIE_COMPRIMEER2_H

long long int* spec_read_file(const char* filename, unsigned int* number);
void spec_encodeer(const char* filename, const char* output_file);
long long int* text_split(char* text, const char delimiter, unsigned int* number);
long long int* calculate_differences(long long* ints, unsigned int number);

void spec_decodeer(const char* filename, const char* output_filename);
char* spec_read_text(FILE *fp);
char* spec_decode_text(char* text);
void spec_write_text(char* decoded_text, const char* output_file);

#endif //COMPRESSIE_COMPRIMEER2_H
