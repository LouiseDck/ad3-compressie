//
// Created by louise on 21/11/16.
//
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <unistd.h>
#include "comprimeer2.h"

#define BUFSIZE 2028

unsigned long long int * text_split(char *text, const char delimiter, unsigned int *number, int round,
                                    char *prev_buffer, int *prev_buffer_size){
    if(!round) {
        text++;
    }
    unsigned long long int* result = 0;
    unsigned int delim_count = 1; // 1 teken na laatste delim

    char* temp_text = text;
    while(*temp_text){
        if(delimiter == temp_text[0]){
            delim_count++;
        }
        temp_text++;
    }

    result = calloc(sizeof(unsigned long long int), delim_count);
    char delim[2] = {delimiter, 0};
    int lastcomma = 0;
    if(text[strlen(text) - 1] == ',' || text[strlen(text) - 1] == ']' || text[strlen(text) - 2] == ']') {
        lastcomma = 1;
    }

    if(result){
        size_t i = 0;
        char* piece = strtok(text, delim);
        while(piece){
            unsigned long long curr = atoll(piece);
            //if(piece[strlen(piece) - 1] != '')
            result[i] = curr;
            i++;
            piece = strtok(0, delim);
            int ding = 0;
        }
        *number = i;
        if(!lastcomma) {
            (*number) = i - 1;
            *prev_buffer_size= sprintf(prev_buffer, "%llu", result[i - 1]);
            int ding = 0;
        }
    }

    return result;
}

unsigned char number_of_bits(unsigned long long nr){
    unsigned char count = 0;
    while(nr > 0){
        count++;
        nr >>= 1;
    }
    return count;
}

int fpeek(FILE* fp) {
    int c = getc(fp);
    if(c == EOF){
        return EOF;
    }
    ungetc(c, fp);
    return 0;
}

unsigned char* nr_bits(unsigned long long int *difs, size_t number, unsigned char *bits){
    for(int i = 0; i < number - 1; i++){
        unsigned long long current_nr = difs[i];
        unsigned char nr_bits = number_of_bits(current_nr);
        bits[i] = nr_bits;
    }
    return bits;
}

void write_bit(FILE *fp, unsigned long long int **buffer, size_t *number, unsigned long long int bit, int* bit_written){
    if(*number != 7 ){
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
        (*bit_written)++;

        for(int i = 0; i < 8; i++){
            (*buffer)[i] = 0;
        }
    }
}

void write_final_bit(FILE* fp, unsigned long long int** buffer, int* byte_written, int* nr_bits){
    char cur_sum = 0;
    cur_sum += 128 * (*buffer)[0];
    cur_sum += 64 * (*buffer)[1];
    cur_sum += 32 * (*buffer)[2];
    cur_sum += 16 * (*buffer)[3];
    cur_sum += 8 * (*buffer)[4];
    cur_sum += 4 * (*buffer)[5];
    cur_sum += 2 * (*buffer)[6];
    cur_sum += 1 * (*buffer)[7];

    if((*nr_bits) != 0) {
        fwrite(&cur_sum, sizeof(char), 1, fp);
        (*byte_written)++;
    }
}

void nr_to_bits(unsigned long long int *difs, unsigned char *bits, size_t number, FILE *fp, int* byte_written){
    unsigned long long* buffer = calloc(sizeof(unsigned long long), 8);
    size_t nr_bits = 0;
    for(int i = 0; i < number; i++){
        unsigned long long current = difs[i];
        unsigned char current_size = bits[i];
        int size = current_size;
        // eerste 6 bits uitschrijven
        unsigned char mask = 1;
        int shift = 5;
        for(int j = 0; j < 6; j++) {
            unsigned char cur_bit= current_size >> shift;
            cur_bit &= mask;
            shift--;
            write_bit(fp, &buffer, &nr_bits, cur_bit, byte_written);
        }
        unsigned long long long_mask = 1;
        int shift2 = size - 1;
        //printf("%i\n", size);
        for(int j = 0; j < size; j++){
            unsigned long long cur_bit = current >> shift2;
            cur_bit &= long_mask;
            shift2--;
            write_bit(fp, &buffer, &nr_bits, cur_bit, byte_written);
        }
    }
    write_final_bit(fp, &buffer, byte_written, &nr_bits);
    int ding = 0;
}

void spec_encodeer(const char* filename, const char* output_file){
    FILE* clear = fopen(output_file, "w");
    fclose(clear);
    size_t size = BUFSIZE;
    FILE *fp1 = fopen(filename, "r");
    char* prev_buffer = calloc(sizeof(char), 9);
    int prev_buffer_size = 0;

    int round = 0;

    while(!fpeek(fp1) || prev_buffer_size != 0){
        unsigned int number_of_numbers = 0;
        char *text = calloc(BUFSIZE + 1, sizeof(char));
        if (fp1 != NULL) {
            prev_buffer = realloc(prev_buffer, prev_buffer_size + BUFSIZE);
            size = fread(text, sizeof(char), BUFSIZE, fp1);
            if(prev_buffer_size != 0) {
                strcat(prev_buffer, text);
            } else{
                strcpy(prev_buffer, text);
            }

            prev_buffer[size + prev_buffer_size] = '\0';
            prev_buffer_size = 0;

            unsigned long long *ints = text_split(prev_buffer, ',', &number_of_numbers, round, prev_buffer, &prev_buffer_size);

            unsigned long long *difs = calculate_differences(ints, number_of_numbers);
            unsigned char *bits = calloc(sizeof(unsigned char), number_of_numbers);
            nr_bits(difs, number_of_numbers + 1, bits);

            FILE *fp = fopen(output_file, "a+b");
            fwrite(&number_of_numbers, sizeof(unsigned int), 1, fp);
            //bereken aantal bytes
            int number_bits = 0;
            for (int i = 0; i < number_of_numbers; i++) {
                number_bits += bits[i];
                number_bits += 6;
            }
            //for (int i = 0; i < number_of_numbers; i++) {
            //    printf("%i\n", bits[i]);
            //}
            //printf("\n");
            int number_bytes = (number_bits + 7) / 8;
            fwrite(&number_bytes, sizeof(int), 1, fp);
            int byte_written = 0;
            nr_to_bits(difs, bits, number_of_numbers, fp, &byte_written);
            //printf("---------------------------------\n");
            int ding = 0;
            fclose(fp);
        }
        round++;
    }
    fclose(fp1);
}

unsigned long long int * calculate_differences(unsigned long long int *ints, int number){
    unsigned long long* difs = calloc(sizeof(unsigned long long), number);
    difs[0] = ints[0];
    for(size_t i = 1; i < number; i++){
        long long ding1 = ints[i];
        long long ding2 = ints[2];
        difs[i] = ints[i] - ints[i - 1];
    }
    return difs;
}

void longlong_to_char(unsigned long long int *ints, char **buffer, int *written, int number){
    for(int i = 0; i < number; i++) {
        char *cur = calloc(sizeof(char), 9);
        int w = sprintf(cur, "%llu", ints[i]);
        buffer[i] = cur;
        written[i] = w;
    }
}

void add_up_numbers(unsigned long long int *decoded_longs, unsigned long long int *actual_numbers, int number){
    actual_numbers[0] = decoded_longs[0];
    for(int i = 1; i <= number; i++){
        unsigned long long ding1 = actual_numbers[i-1];
        unsigned long long ding2 = actual_numbers[i];
        actual_numbers[i] = actual_numbers[i-1] + decoded_longs[i];
    }
}



void spec_decodeer(const char* filename, const char* output_filename){
    int round = 0;
    FILE* clear = fopen(output_filename, "w");
    fclose(clear);
    FILE* outputfp = fopen(output_filename, "a+b");
    char open = '[';
    char close = ']';
    fwrite(&open, sizeof(char), 1, outputfp);

    FILE* fp = fopen(filename, "rb");
    while(!fpeek(fp)){
        if(fp) {

            unsigned int number;
            size_t size_1 = fread(&number, sizeof(int), 1, fp);
            unsigned int number_bytes;
            size_t size_2 = fread(&number_bytes, sizeof(int), 1, fp);

            unsigned char *encoded_text = calloc(sizeof(unsigned char), number_bytes + 1);
            fread(encoded_text, sizeof(unsigned char), number_bytes, fp);
            encoded_text[number_bytes] = '\0';

            unsigned long long *decoded_longs = calloc(sizeof(unsigned long long), number + 1);
            spec_decode_text(encoded_text, number, decoded_longs);

            unsigned long long *actual_numbers = calloc(sizeof(unsigned long long), number + 1);
            add_up_numbers(decoded_longs, actual_numbers, number);

            char **buffer = calloc(sizeof(char *), number);
            for (int i = 0; i < number; i++) {
                buffer[i] = calloc(sizeof(char), 9);
            }
            int *written = calloc(sizeof(int), number + 1);
            longlong_to_char(actual_numbers, buffer, written, number);

            //for (int i = 0; i < number; i++) {
            //    printf("%i, %s\n", number, buffer[i]);
            //}
            spec_write_text(buffer, written, outputfp, number, round);

            free(encoded_text);
            free(decoded_longs);
            free(actual_numbers);
            for (int i = 0; i < number; i++) {
                free(buffer[i]);
            }
            free(written);
            round++;
            fflush(outputfp);
        }
    }
    fclose(fp);

    fwrite(&close, sizeof(char), 1, outputfp);
    fclose(outputfp);
}

int read_bit(unsigned char* text, int* byte_index, int* bit_index){
    if(*bit_index == 8) {
        *bit_index = 0;
        (*byte_index)++;
    }
    unsigned char current_byte = text[*byte_index];
    // shift 7 - bit_index
    int current_bit = current_byte >> (7 - *bit_index);
    current_bit &= 1;
    (*bit_index)++;
    return current_bit;
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

unsigned long long int read_long_bits(unsigned char *text, int *byte_index, int *bit_index, unsigned int size){
    unsigned long long cur_sum = 0;
    for(unsigned int i = 1; i <= size; i++){
        int shift_nr = size - i;
        unsigned long long shift = ((unsigned long long) 1) << shift_nr;
        int cur_bit = read_bit(text, byte_index, bit_index);
        cur_sum += shift * cur_bit;
        int ding = 0;
    }
    return cur_sum;
}

void spec_decode_text(unsigned char *text, unsigned int number, unsigned long long int *decoded_longs){

    int byte_index = 0;
    int bit_index = 0;
    for(int i = 0; i < number; i++){
        int size = read_size_bits(text, &byte_index, &bit_index);
        unsigned long long current = read_long_bits(text, &byte_index, &bit_index, size);
        decoded_longs[i] = current;
    }
}
void spec_write_text(char** decoded_text, int* sizes, FILE* outputfp, int number, int round){
    char delim = ',';
    if(round > 0){
        fwrite(&delim, sizeof(char), 1, outputfp);
    }
    for(int i = 0; i < number - 1; i++){
        int size = sizes[i];
        fwrite(decoded_text[i], sizeof(char), size, outputfp);
        fwrite(&delim, sizeof(char), 1, outputfp);
    }
    int size = sizes[number - 1];
    fwrite(decoded_text[number - 1], sizeof(char), size, outputfp);

}
