//
// Created by louise on 21/11/16.
//
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include "comprimeer2.h"

#define BUFSIZE 1023

long long* spec_read_file(const char* filename, unsigned int* number){
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

long long int* text_split(char* text, const char delimiter, unsigned int* number){
    long long int* result = 0;
    unsigned int delim_count = 1; // 1 teken na laatste delim

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
    unsigned char count = 0;
    while(nr > 0){
        count++;
        nr >>= 1;
    }
    return count;
}

unsigned char* nr_bits(long long* difs, size_t number, unsigned char* bits){
    for(int i = 0; i < number - 1; i++){
        long long current_nr = difs[i];
        unsigned char nr_bits = number_of_bits(current_nr);
        bits[i] = nr_bits;
    }
    return bits;
}

void write_bit(FILE* fp, long long** buffer, size_t* number, long long bit){
    if(*number != 7){
        (*buffer)[*number] = bit;
        (*number)++;
    } else{
        (*buffer)[*number] = bit;
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

        for(int i = 0; i < 8; i++){
            (*buffer)[i] = 0;
        }
    }
}

void nr_to_bits(long long* difs, unsigned char* bits, size_t number, FILE* fp){
    long long* buffer = calloc(sizeof(long long), 8);
    size_t nr_bits = 0;
    for(int i = 0; i < number + 1; i++){
        long long current = difs[i];
        unsigned char current_size = bits[i];
        int size = current_size;
        // eerste 6 bits uitschrijven
        unsigned char mask = 1;
        int shift = 5;
        for(int j = 0; j < 6; j++) {
            unsigned char cur_bit= current_size >> shift;
            cur_bit &= mask;
            shift--;
            write_bit(fp, &buffer, &nr_bits, cur_bit);
        }
        long long long_mask = 1;
        int shift2 = size - 1;
        for(int j = 0; j < size; j++){
            long long cur_bit = current >> shift2;
            cur_bit &= long_mask;
            shift2--;
            write_bit(fp, &buffer, &nr_bits, cur_bit);
        }
    }
}

void spec_encodeer(const char* filename, const char* output_file){
    unsigned int number_of_numbers = 0;
    long long* ints = spec_read_file(filename, &number_of_numbers);
    long long* difs = calculate_differences(ints, number_of_numbers);
    unsigned char* bits = calloc(sizeof(unsigned char), number_of_numbers);
    nr_bits(difs, number_of_numbers + 1, bits);
    FILE* fp = fopen(output_file, "wb");

    fwrite(&number_of_numbers, sizeof(unsigned int), 1, fp);

    for(int i = 0; i < number_of_numbers; i++){
        printf("%lli\n", difs[i]);
        printf("%lli\n", ints[i]);
        printf("%c\n\n", bits[i] + '0');
    }

    nr_to_bits(difs, bits, number_of_numbers, fp);
}

long long* calculate_differences(long long* ints, unsigned int number){
    long long* difs = calloc(sizeof(long long), number);
    difs[0] = ints[0];
    for(size_t i = 1; i < number; i++){
        difs[i] = ints[i] - ints[i - 1];
    }
    return difs;
}

void spec_decodeer(const char* filename, const char* output_filename){
    FILE* fp = fopen(filename, "rb");
    unsigned int number;
    fread(&number, sizeof(int), 1, fp);

    unsigned char* encoded_text = calloc(sizeof(unsigned char), number + 1);
    spec_read_text(fp, number, encoded_text);
    spec_decode_text(encoded_text, number);

    fclose(fp);
}
unsigned char* spec_read_text(FILE *fp, unsigned int number, unsigned char* encoded_text){
    fread(encoded_text, sizeof(unsigned char), number, fp);
    encoded_text[number] = '\0';
    return encoded_text;
}

int read_bit(unsigned char* text, int* byte_index, int* bit_index){

}

int read_size_bits(unsigned char* text, int* byte_index, int* bit_index){
    char cur_sum = 0;
    cur_sum += 32 * read_bit(text, byte_index, bit_index);
    cur_sum += 16 * read_bit(text, byte_index, bit_index);
    cur_sum += 8 * read_bit(text, byte_index, bit_index);
    cur_sum += 4 * read_bit(text, byte_index, bit_index);
    cur_sum += 2 * read_bit(text, byte_index, bit_index);
    cur_sum += 1 * read_bit(text, byte_index, bit_index);

    return cur_sum;
}

int read_long_bits(char* text, int* byte_index, int* bit_index){

}

char* spec_decode_text(unsigned char* text, unsigned int number){
    // lees 6 bits in voor u size
    int byte_index = 0;
    int bit_index = 0;
    for(int i = 0; i < number; i++){
        int size = read_size_bits(text, &byte_index, &bit_index);
        long long = read_long_bits(text, &byte_index, &bit_index);
    }
    // lees size aantal bits in voor tekst
}
void spec_write_text(char* decoded_text, const char* output_file){

}
