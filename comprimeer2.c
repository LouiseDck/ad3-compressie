//
// Created by louise on 21/11/16.
//
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <math.h>
#include "comprimeer2.h"

#define BUFSIZE 1023

long long* spec_read_file(const char* filename, size_t* number){
    char *text = calloc(BUFSIZE + 1, sizeof(char));
    FILE *fp = fopen(filename, "r");
    if(fp != NULL){
        size_t size = fread(text, sizeof(char), BUFSIZE, fp);
        text[size] = '\0';
    }
    fclose(fp);
    long long* result = text_split(text, ',', number);

    return result;

}

long long int* text_split(char* text, const char delimiter, size_t* number){
    long long int* result = 0;
    size_t delim_count = 1; // 1 teken na laatste delim

    char* temp_text = text;
    while(*temp_text){
        if(delimiter == temp_text[0]){
            delim_count++;
        }
        temp_text++;
    }

    result = calloc(sizeof(long long int), delim_count);
    char delim[2] = {delimiter, 0};

    if(result){
        size_t i = 0;
        char* piece = strtok(text, delim);
        while(piece){
            long long curr = atoll(piece);
            result[i] = curr;
            i++;
            piece = strtok(0, delim);
        }
    }
    *number = delim_count;
    return result;
}

unsigned char number_of_bits(long long nr){
    unsigned char number = 0;
    while(nr >>= 1) number++;
    return number;
}

unsigned char* nr_bits(long long* difs, size_t number, unsigned char* bits){
    for(int i = 0; i < number; i++){
        long long current_nr = difs[i];
        char nr_bits = number_of_bits(current_nr);
        bits[i] = nr_bits;
    }
    return bits;
}

void write_bit(FILE* fp, long long** buffer, size_t* number, long long bit){
    if(*number != 7){
        (*buffer)[*number] = bit;
        (*number)++;
    } else{
        int ding1 = (*buffer)[0];
        int ding2 = (*buffer)[1];
        int ding3 = (*buffer)[2];
        //maak char van alle 8
        char cur_sum = 0;
        cur_sum += 128 * (*buffer)[0];
        cur_sum += 64 * (*buffer)[1];
        cur_sum += 32 * (*buffer)[2];
        cur_sum += 16 * (*buffer)[3];
        cur_sum += 8 * (*buffer)[4];
        cur_sum += 4 * (*buffer)[5];
        cur_sum += 2 * (*buffer)[6];
        cur_sum += 1 * (*buffer)[7];

        fwrite(&cur_sum, sizeof(char), 1, fp);
        *number = 0;
        (*buffer)[*number] = bit;
    }
}

void nr_to_bits(long long* difs, unsigned char* bits, size_t number, FILE* fp){
    long long* buffer = calloc(sizeof(long long), 8);
    size_t nr_bits = 0;
    for(int i = 0; i < number; i++){
        long long current = difs[i];
        unsigned char current_size = bits[i];
        int size = current_size;
        // eerste 6 bits uitschrijven
        unsigned char mask = 1;
        for(int j = 0; j < 6; j++) {
            unsigned char cur_bit = current_size & mask;
            mask = (mask <<= 1) + (unsigned char) 1;
            current_size >>= 1;
            write_bit(fp, &buffer, &nr_bits, cur_bit);
            int dint = 0;
        }
        long long long_mask = 1;
        for(int j = 0; j < size; j++){
            long long cur_bit = current & long_mask;
            long_mask = (long_mask <<= 1) + 1;
            current >>= 1;
            write_bit(fp, &buffer, &nr_bits, cur_bit);
            int dino = 0;
        }

        int ding = 0;
    }
}

void spec_encodeer(const char* filename, const char* output_file){
    size_t number_of_numbers = 0;
    long long* ints = spec_read_file(filename, &number_of_numbers);
    long long* difs = calculate_differences(ints, number_of_numbers);
    unsigned char* bits = calloc(sizeof(unsigned char), number_of_numbers);
    nr_bits(difs, number_of_numbers, bits);
    FILE* fp = fopen(output_file, "wb");

    nr_to_bits(difs, bits, number_of_numbers, fp);
}

//TODO: fix eerste wel bijhouden
long long* calculate_differences(long long* ints, size_t number){
    long long* difs = calloc(sizeof(long long), number);
    for(size_t i = 0; i < number - 1; i++){
        long long ding = ints[i+1];
        long long ding2 = ints[i];
        difs[i] = ints[i+1] - ints[i];
    }
    return difs;
}

void spec_decodeer(const char* filename, const char* output_filename);
char* spec_read_text(FILE *fp);
char* spec_decode_text(char* text);
void spec_write_text(char* decoded_text, const char* output_file);
