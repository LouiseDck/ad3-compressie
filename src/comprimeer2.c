//
// Created by louise on 21/11/16.
//
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include "comprimeer2.h"

#define BUFSIZE 2028

int main(int argc, char *argv[]) {
    char* inputfile = argv[2];
    char* outputfile = argv[3];
    if(strcmp(argv[1], "-d") == 0){
        spec_decodeer(inputfile, outputfile);

    } else if(strcmp(argv[1], "-c") == 0){
        spec_encodeer(inputfile, outputfile);
    }

    return 0;
}

/* De ingelezen tekst wordt gesplitst */
long long int * text_split(char *text, const char delimiter, unsigned int *number, int round,
                           char *prev_buffer, int *prev_buffer_size) {
    // Indien het de eerste keer is dat deze functie wordt opgeroepen, mag de eerste byte '[' niet worden gelezen
    if (!round) {
        text++;
    }

    long long int* result = 0;
    unsigned int delim_count = 1; // mogelijks 1 teken na laatste delim
    char* temp_text = text;
    while (*temp_text) {
        if (delimiter == temp_text[0]) {
            delim_count++;
        }
        temp_text++;
    }


    result = calloc(sizeof(long long int), delim_count);
    char delim[2] = {delimiter, 0};

    // Indien het laatste karakter van de tekst een komma of ] is hoeft er geen buffer bijgehouden te worden
    // In het andere geval wel, aangezien we niet zeker zijn of het gehele getal al is ingelezen.
    int lastcomma = 0;
    if(text[strlen(text) - 1] == ',' || text[strlen(text) - 1] == ']' || text[strlen(text) - 2] == ']') {
        lastcomma = 1;
    }

    if(result){
        size_t i = 0;
        char* piece = strtok(text, delim);
        while(piece){
            long long curr = atoll(piece);
            result[i] = curr;
            i++;
            piece = strtok(0, delim);
        }
        *number = (int) i;
        if(!lastcomma) {
            (*number) = (unsigned int) i - 1;
            *prev_buffer_size = sprintf(prev_buffer, "%lli", result[i - 1]);
        }
    }
    return result;
}

/* Hulpfunctie om het aantal bits waarmee een long long kan voorgesteld worden te tellen */
unsigned char number_of_bits(long long nr){
    unsigned char count = 0;
    while(nr > 0){
        count++;
        nr >>= 1;
    }
    return count;
}

/* Hulpfunctie om te bepalen of er nog een int te lezen valt in de tekst */
int fpeek(FILE* fp) {
    int c = getc(fp);
    if(c == EOF) return EOF;
    ungetc(c, fp);
    return 0;
}

/* Hiermee wordt bepaald voor elk verschil tussen 2 longs hoeveel bits nodig zijn om ze binair te kunnen voorstellen */
unsigned char* nr_bits(long long int *difs, size_t number, unsigned char *bits){
    for(int i = 0; i < number - 1; i++){
        long long current_nr = difs[i];
        unsigned char nr_bits = number_of_bits(current_nr);
        bits[i] = nr_bits;
    }
    return bits;
}

/* Hulpfunctie die telkens wanneer de buffer vol zit een byte zal uitschrijven naar de fp */
void write_bit(FILE *fp, long long int **buffer, size_t *number, long long int bit, int* bit_written){
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

/* Methode die de eventuele niet volle buffer zal uitschrijven */
void write_final_bit(FILE* fp, long long int** buffer, int* byte_written, size_t* nr_bits){
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

/* Hier wordt een verschil tussen 2 longs vertaald naar een vast aantal bits die de grootte van het daaropvolgende
 * getal weergeeft. Dit wordt hier ook uitgeschreven */
void nr_to_bits(long long int *difs, unsigned char *bits, size_t number, FILE *fp, int* byte_written){
    long long* buffer = calloc(sizeof(long long), 8);
    size_t nr_bits = 0;
    for(int i = 0; i < number; i++){
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
            write_bit(fp, &buffer, &nr_bits, cur_bit, byte_written);
        }
        long long long_mask = 1;
        int shift2 = size - 1;
        //printf("%i\n", size);
        for(int j = 0; j < size; j++){
            long long cur_bit = current >> shift2;
            cur_bit &= long_mask;
            shift2--;
            write_bit(fp, &buffer, &nr_bits, cur_bit, byte_written);
        }
    }
    write_final_bit(fp, &buffer, byte_written, &nr_bits);
}

/* Encodeert een bestand */
void spec_encodeer(const char* filename, const char* output_file){
    // Zorgt ervoor dat de outputfile leeg is.
    FILE* clear = fopen(output_file, "w");
    fclose(clear);
    FILE *fp1 = fopen(filename, "r");
    char* prev_buffer = calloc(sizeof(char), 9);
    int prev_buffer_size = 0;
    int round = 0;

    while(!fpeek(fp1) || prev_buffer_size != 0){
        unsigned int number_of_numbers = 0;
        char *text = calloc(BUFSIZE + 1, sizeof(char));
        if (fp1 != NULL) {

            // De vorige buffer en de huidige text worden samengenomen
            prev_buffer = realloc(prev_buffer, (size_t) prev_buffer_size + BUFSIZE);
            size_t size = fread(text, sizeof(char), BUFSIZE, fp1);
            if(prev_buffer_size != 0) {
                strcat(prev_buffer, text);
            } else{
                strcpy(prev_buffer, text);
            }

            prev_buffer[size + prev_buffer_size] = '\0';
            prev_buffer_size = 0;

            // De tekst wordt gesplits op , en  de karakters worden omgezet naar long long ints.
            long long *ints = text_split(prev_buffer, ',', &number_of_numbers, round, prev_buffer, &prev_buffer_size);

            // Het verschil tussen de long longs wordt berekend en het aantal bits nodig om deze verschillen binair voor te stellen
            long long *difs = calculate_differences(ints, number_of_numbers + 1);
            unsigned char *bits = calloc(sizeof(unsigned char), number_of_numbers + 1);
            nr_bits(difs, number_of_numbers + 1, bits);

            FILE *fp = fopen(output_file, "a+b");
            fwrite(&number_of_numbers, sizeof(unsigned int), 1, fp);
            //bereken aantal bytes
            int number_bits = 0;
            for (int i = 0; i < number_of_numbers; i++) {
                number_bits += bits[i];
                number_bits += 6;
            }

            int number_bytes = (number_bits + 7) / 8;
            fwrite(&number_bytes, sizeof(int), 1, fp);
            int byte_written = 0;
            nr_to_bits(difs, bits, number_of_numbers, fp, &byte_written);

            fclose(fp);
        }
        round++;
    }
    fclose(fp1);
}

long long int * calculate_differences(long long int *ints, int number){
    long long* difs = calloc(sizeof(long long), (size_t) number);
    difs[0] = ints[0];
    for(size_t i = 1; i < number; i++){
        difs[i] = ints[i] - ints[i - 1];
    }
    return difs;
}

/* Functie die ervoor zorgt dat long long ints worden omgezet naar een char array */
void longlong_to_char(long long int *ints, char **buffer, int *written, int number){
    for(int i = 0; i < number; i++) {
        char *cur = calloc(sizeof(char), 9);
        int w = sprintf(cur, "%llu", ints[i]);
        buffer[i] = cur;
        written[i] = w;
    }
}

/* Aangezien de getallen verschillen van elkaar zijn, moeten ze opgeteld worden */
void add_up_numbers(long long int *decoded_longs, long long int *actual_numbers, int number){
    actual_numbers[0] = decoded_longs[0];
    for(int i = 1; i <= number; i++){
        actual_numbers[i] = actual_numbers[i-1] + decoded_longs[i];
    }
}

/* Functie die een gecomprimeerd bestand decomprimeerd */
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
            fread(&number, sizeof(int), 1, fp);
            unsigned int number_bytes;
            fread(&number_bytes, sizeof(int), 1, fp);

            unsigned char *encoded_text = calloc(sizeof(unsigned char), number_bytes + 1);
            fread(encoded_text, sizeof(unsigned char), number_bytes, fp);
            encoded_text[number_bytes] = '\0';

            long long *decoded_longs = calloc(sizeof(long long), number + 1);
            spec_decode_text(encoded_text, number, decoded_longs);

            long long *actual_numbers = calloc(sizeof(long long), number + 1);
            add_up_numbers(decoded_longs, actual_numbers, number);

            char **buffer = calloc(sizeof(char *), number);
            for (int i = 0; i < number; i++) {
                buffer[i] = calloc(sizeof(char), 9);
            }
            int *written = calloc(sizeof(int), number + 1);
            longlong_to_char(actual_numbers, buffer, written, number);

            spec_write_text(buffer, written, outputfp, number, round);

            free(encoded_text);
            free(decoded_longs);
            free(actual_numbers);
            for (int i = 0; i < number; i++) {
                free(buffer[i]);
            }
            free(buffer);
            free(written);
            round++;
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

unsigned int read_size_bits(unsigned char* text, int* byte_index, int* bit_index){
    unsigned char cur_sum = 0;
    cur_sum += 32 * read_bit(text, byte_index, bit_index);
    cur_sum += 16 * read_bit(text, byte_index, bit_index);
    cur_sum += 8 * read_bit(text, byte_index, bit_index);
    cur_sum += 4 * read_bit(text, byte_index, bit_index);
    cur_sum += 2 * read_bit(text, byte_index, bit_index);
    cur_sum += 1 * read_bit(text, byte_index, bit_index);

    return cur_sum;
}

long long int read_long_bits(unsigned char *text, int *byte_index, int *bit_index, unsigned int size){
    long long cur_sum = 0;
    for(unsigned int i = 1; i <= size; i++){
        int shift_nr = size - i;
        long long shift = ((long long) 1) << shift_nr;
        int cur_bit = read_bit(text, byte_index, bit_index);
        cur_sum += shift * cur_bit;
    }
    return cur_sum;
}

void spec_decode_text(unsigned char *text, unsigned int number, long long int *decoded_longs){

    int byte_index = 0;
    int bit_index = 0;
    for(int i = 0; i < number; i++){
        unsigned int size = read_size_bits(text, &byte_index, &bit_index);
        long long current = read_long_bits(text, &byte_index, &bit_index, size);
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
        fwrite(decoded_text[i], sizeof(char), (size_t) size, outputfp);
        fwrite(&delim, sizeof(char), 1, outputfp);
    }
    int size = sizes[number - 1];
    fwrite(decoded_text[number - 1], sizeof(char), (size_t) size, outputfp);

}
