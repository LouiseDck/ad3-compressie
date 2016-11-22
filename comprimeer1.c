//
// Created by louise on 05/10/16.
//

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "comprimeer1.h"

#define BUFSIZE 1023

char* read_file(const char* filename){
    char *text = calloc(BUFSIZE + 1, sizeof(char));
    FILE *fp = fopen(filename, "r");
    if(fp != NULL){
        size_t size = fread(text, sizeof(char), BUFSIZE, fp);
        text[size] = '\0';
    }
    fclose(fp);
    return text;
}

void encodeer(char* text, const char* filename) {
    int number_not_zero = strlen(text);
    int iets = 0;
    List* list = make_freq_list(text, &iets);
    int aantal_char = list->number_of_items;
    struct encode_Item encode_array[aantal_char];
    encode_Item* item = make_encoding_huffman_tree(encode_array, list);
    int current_size = 0;
    char* complete = calloc(sizeof(char), 256);
    char* complete_code = encode_text(number_not_zero, text, encode_array, &current_size, &complete);

    int textlength = (int) strlen(text);
    int number_of_chars_to_write = (current_size + 8 - 1)/8;

    FILE *fp = fopen(filename, "wb");
    if(fp != NULL) {
        fwrite(&textlength, 4, 1, fp);
        fwrite(&number_of_chars_to_write, 4, 1, fp);
    }
    fclose(fp);

    char* buffer = (char*) calloc(1, sizeof(char));
    write_tree(filename, list->firstitem->leaf, buffer);

    // schrijf de codes uit

    char to_encode[(number_of_chars_to_write * 8) + 1];
    for(int i = current_size; i < number_of_chars_to_write * 8 + 1; i++){
        to_encode[i] = 48;
    }
    strncpy(to_encode, complete_code, (size_t) number_of_chars_to_write * 8);
    int already_written = 0;
    for(int temp = 0; temp < number_of_chars_to_write; temp++){
        unsigned char bits;
        //add up the numbers of the char
        int cur_sum = 0;
        cur_sum += 128 * (to_encode[0 + already_written] - '0');
        cur_sum += 64 * (to_encode[1 + already_written] - '0');
        cur_sum += 32 * (to_encode[2 + already_written] - '0');
        cur_sum += 16 * (to_encode[3 + already_written] - '0');
        cur_sum += 8 * (to_encode[4 + already_written] - '0');
        cur_sum += 4 * (to_encode[5 + already_written] - '0');
        cur_sum += 2 * (to_encode[6 + already_written] - '0');
        cur_sum += 1 * (to_encode[7 + already_written] - '0');

        FILE *fp = fopen(filename, "a+b");
        if(fp != NULL) {
            fwrite(&cur_sum, sizeof(char), 1, fp);
        }
        fclose(fp);
        already_written +=8;
        int iets = 0;
    }

    int ding = 0;
}

List* make_freq_list(char* text, int* number_not_zero){
    int freq[256] = {0};

    for (int i = 0; i < BUFSIZE; i++) {
        char character = text[i];
        freq[character] = freq[character] + 1;
        if(character) (*number_not_zero)++;
    }

    //maak huffman boompje
    List* list = new_list();
    for(int i = 0; i < 256; i++){
        if(freq[i] != 0 && i != NULL){
            Item* item = new_item(freq[i], (char) i, NULL);
            add_element_freq(list, item);
        }
    }
    return list;
}
encode_Item* make_encoding_huffman_tree(encode_Item* encode_array, List* list){

    Item* current = list->firstitem;
    int where_encode_insert = 0;
    while(list->number_of_items > 1){
        Item* second = current->volgend;
        Leaf* zero_child;
        Leaf* one_child;
        if(current->leaf == NULL) {
            zero_child = make_leaf(NULL, NULL, current->data, current->freq, 0, encode_array, where_encode_insert);
            where_encode_insert++;
        } else{
            zero_child = make_leaf(current->leaf->zero_child, current->leaf->one_child, current->data, current->freq, 0,encode_array, where_encode_insert);
        }
        if(second->leaf == NULL) {
            one_child = make_leaf(NULL, NULL, second->data, second->freq, 1, encode_array, where_encode_insert);
            where_encode_insert++;
        } else{
            one_child = make_leaf(second->leaf->zero_child, second->leaf->one_child, second->data, second->freq, 1, encode_array, where_encode_insert);
        }

        Leaf* new = make_leaf(zero_child, one_child, NULL, current->freq + second->freq, 2, encode_array, where_encode_insert);
        add_element_freq(list, new_leaf_item(new));

        current = second->volgend;

        remove_element(list, 0);
        remove_element(list, 0);
    }
    return list->firstitem;
}
char* encode_text(int aantal_char, const char* text, encode_Item* encode_array, int* cur_siz, char** complete_code){
    int current_size = *cur_siz;
    //vind de nodes die je wilt
    for(int i = 0; i < aantal_char; i++){
        char current_char  = text[i];
        char code[255 * 255];
        int j = 0;
        while(current_char != encode_array[j].data){
            j++;
        }
        encode_Item current_item = encode_array[j];
        Leaf* current_leaf = current_item.code;

        int iterations = 255 * 255;
        while(current_leaf->parent != NULL){
            if(current_leaf->zero_or_one_child == 0){
                code[iterations] = 48;
            } else{
                code[iterations] = 49;
            }
            current_leaf = current_leaf->parent;
            iterations--;
        }
        current_size += ((255*255)-iterations);
        *complete_code = (char*) realloc(*complete_code, (current_size-1) * sizeof(int));
        int end_code = iterations + 1;
        for(int k = current_size - ((255*255)-iterations); k < current_size; k++){
            int ding = code[end_code];
            (*complete_code)[k] = code[end_code];
            end_code++;
        }
    }
    (*complete_code)[current_size] = '\0';
    *cur_siz = current_size;
    return *complete_code;
}

void printbuffer(int* bitnr, char* buffer){
    for(int i = 0; i < *bitnr; i++){
        printf("%i: %c \n", i, buffer[i]);
    }
}

void encode_nodes(Leaf* leaf, char** buffer, int *bitnr, int *buffersize){
    // geen blad -> schrijf 0 naar buffer & resize
    // schrijf kinderen ook weg
    if(leaf->zero_child != NULL && leaf->one_child != NULL){
        *buffer = realloc(*buffer, *buffersize + sizeof(char));
        *buffersize += sizeof(char);
        (*buffer)[*bitnr] = 48;
        (*bitnr)++;
        //printbuffer(bitnr, buffer);
        encode_nodes(leaf->zero_child, buffer, bitnr, buffersize);
        encode_nodes(leaf->one_child, buffer, bitnr, buffersize);
    } else{
        *buffer = realloc(*buffer, *buffersize + (2*sizeof(char)));
        *buffersize += (2*sizeof(char));
        (*buffer)[*bitnr] = 49;
        (*buffer)[*bitnr + 1] = leaf->character;
        (*bitnr) += 2;
        printbuffer(bitnr, *buffer);
    }
}
//Kan eventueel beter door de 0'en en 1'en met 1 bit uit te schrijven ipv 8 bits
void write_tree(const char* filename, Leaf* parent_leaf, char* buffer){
    int buffersize = sizeof(char);
    int bitnr = 0;
    encode_nodes(parent_leaf, &buffer, &bitnr, &buffersize);
    printbuffer(&bitnr, buffer);
    FILE *fp = fopen(filename, "a+b");
    if(fp != NULL) {
        fwrite(&bitnr, sizeof(char), 1, fp);
        fwrite(buffer, sizeof(char), bitnr, fp);
    }
    fclose(fp);
}

void write_encoded_text(){}

void decodeer(const char* filename, const char* output_filename){
    int size_tree = 0;
    FILE *fp = fopen(filename, "rb");

    if(fp != NULL) {
        int aantal = 0;
        int aantalchar = 0;
        fread(&aantal, sizeof(int), 1, fp);
        fread(&aantalchar, 4, 1, fp);
        Leaf *parent = read_huffman_tree(fp);
        char* text = calloc(BUFSIZE + 1, sizeof(char));
        fread(text, sizeof(char), (size_t) aantalchar, fp);
        char* decoded = decode_text(text, parent, aantal);
        write_text(decoded, output_filename);
    }
    fclose(fp);
}

static size_t place = 0;
char readbyte(char* text, size_t max){
    if(place >= max){
        return 0;
    }
    char cur = text[place];
    place++;
    return cur;
}

Leaf* decode_tree(char* text, size_t max){
    char current = readbyte(text, max);
    if(current == 49){
        return make_only_leaf(NULL, NULL, readbyte(text, max), 2);
    } else{
        Leaf* zerochild = decode_tree(text, max);
        Leaf* onechild = decode_tree(text,max);
        return make_only_leaf(zerochild, onechild, 0, 2);
    }
}

Leaf* read_huffman_tree(FILE *fp){
    char *text;
    char size_tree = 0;
    fread(&size_tree, sizeof(char), 1, fp);
    text = calloc((size_t) size_tree + 1, sizeof(char));
    size_t size = fread(text, sizeof(char), (size_t) size_tree, fp);
    text[size] = '\0';

    Leaf *root_leaf = make_only_leaf(NULL, NULL, 0, 2);
    size_t index = 0;
    Leaf* test = decode_tree(text, (size_t) size_tree);
}

char* read_text(FILE *fp);
char* decode_text(char* text, Leaf* leaf, int aantal){
    char* decoded_text = (char*) calloc(sizeof(char),(size_t) aantal + 1);
    int incomplete = 1;
    Leaf* cur_leaf = leaf;
    int nr_decoded = 0;
    int nr_char = 0;
    size_t shift = 7;
    unsigned char mask = 255;
    while(incomplete){
        while(!cur_leaf->character) {
            //decode 1 teken
            unsigned char cur_char = text[nr_char];
            unsigned char cur_byte = cur_char & mask;
            unsigned int cur_bit = cur_byte >> shift;
            if (cur_bit == 1) {
                cur_leaf = cur_leaf->one_child;
            } else {
                cur_leaf = cur_leaf->zero_child;
            }
            if(shift != 0){
                shift--;
                mask = mask >> 1;
            } else{
                shift = 7;
                mask = 255;
                nr_char++;
            }
        }
        decoded_text[nr_decoded] = cur_leaf->character;
        nr_decoded++;
        cur_leaf = leaf;
        if(nr_decoded==aantal){
            incomplete = 0;
        }
    }
    decoded_text[nr_decoded] = "\0";
    return decoded_text;
}
void write_text(char* decoded_text, const char* output_file){
    FILE *fp = fopen(output_file, "wb");

    if(fp != NULL) {
        fwrite(decoded_text, sizeof(char), strlen(decoded_text), fp);
    }
    fclose(fp);
}

