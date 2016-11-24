//
// Created by louise on 21/11/16.
//
#include "stdio.h"

#ifndef COMPRESSIE_COMPRIMEER2_H
#define COMPRESSIE_COMPRIMEER2_H

void spec_encodeer(const char* filename, const char* output_file);
unsigned long long int * text_split(char *text, const char delimiter, unsigned int *number, int round,
                                    char *prev_buffer, int *prev_buffer_size);
unsigned long long int * calculate_differences(unsigned long long int *ints, int number);

void spec_decodeer(const char* filename, const char* output_filename);
void spec_decode_text(unsigned char *text, unsigned int number, unsigned long long int *decoded_longs);
void spec_write_text(char** decoded_text, int* sizes, FILE* outputfp, int number, int round);

#endif //COMPRESSIE_COMPRIMEER2_H
