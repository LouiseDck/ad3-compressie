//
// Created by louise on 05/10/16.
//

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include "comprimeer1.h"

#define BUFSIZE 1024

int main(int argc, char *argv[]) {
    char* inputfile = argv[2];
    char* outputfile = argv[3];
    if(strcmp(argv[1], "-d") == 0){
        decodeer(inputfile, outputfile);

    } else if(strcmp(argv[1], "-c") == 0){
        encodeer(inputfile, outputfile);
    }

    return 0;
}

int fpeek(FILE* fp) {
    int c = getc(fp);
    if(c == EOF) return EOF;
    ungetc(c, fp);
    return 0;
}

void encodeer(const char* inputfilename, const char* outputfilename) {
    //Zorgt ervoor dat de outputfile leeg is, want in de rest van de code wordt geappend aan deze file.
    FILE* clear = fopen(outputfilename, "w");
    fclose(clear);

    FILE *fp = fopen(inputfilename, "r");
    // Zolang er nog een int in de file is.
    while(!fpeek(fp)) {
        //Plaats voorzien voor de in te lezen niet gecomprimeerde tekst.
        char *text = calloc(BUFSIZE + 1, sizeof(char));
        size_t size = fread(text, sizeof(char), BUFSIZE, fp);
        text[size] = '\0';

        //Opbouwen lijst met frequenties van de voorgekomen karakters
        List *list = make_freq_list(text);

        // De huffman boom wordt opgesteld, samen met een array van de bladeren om gemakkelijker te kunnen zoeken.
        int aantal_char = list->number_of_items;
        struct encode_Item encode_array[aantal_char];
        make_encoding_huffman_tree(encode_array, list);

        // Het effectieve encoderen van de tekst aan de hand van de boom en de array van bladeren
        int textlength = (int) strlen(text);
        int current_size = 0;
        char *complete = calloc(sizeof(char), 256);
        char *complete_code = encode_text(textlength, text, encode_array, &current_size, &complete);

        // het aantal nodige bytes is current_size/8 naar boven afgerond
        int number_of_chars_to_write = (current_size + 7) / 8;

        // om gemakkelijk te kunnen decoderen wordt het aantal originele gedecodeerde karakters gecodeerd,
        // net als het aantal geschreven bytes.
        FILE *ofp = fopen(outputfilename, "a+b");
        if (fp != NULL) {
            fwrite(&textlength, 4, 1, ofp);
            fwrite(&number_of_chars_to_write, 4, 1, ofp);
        }
        fclose(ofp);

        // Nu wordt de huffman boom uitgeschreven
        char *buffer = (char *) calloc(1, sizeof(char));
        write_tree(outputfilename, list->firstitem->leaf, buffer);


        FILE *outfp = fopen(outputfilename, "a+b");
        char to_encode[(number_of_chars_to_write * 8) + 1];
        strncpy(to_encode, complete_code, (size_t) number_of_chars_to_write * 8);
        // zorgt ervoor dat, indien het aantal uit te schrijven bits geen veelvoud van 8 is,
        // dat de uitgeschreven byte nog steeds correct is door de overblijvende bits op 0 te zetten.
        for (int i = current_size; i < number_of_chars_to_write * 8 + 1; i++) {
            to_encode[i] = 48;
        }

        // Samenstellen van de uit te schrijven byte & het uitschrijven
        int already_written = 0;
        for (int temp = 0; temp < number_of_chars_to_write; temp++) {

            int cur_sum = 0;
            cur_sum += 128 * (to_encode[0 + already_written] - '0');
            cur_sum += 64 * (to_encode[1 + already_written] - '0');
            cur_sum += 32 * (to_encode[2 + already_written] - '0');
            cur_sum += 16 * (to_encode[3 + already_written] - '0');
            cur_sum += 8 * (to_encode[4 + already_written] - '0');
            cur_sum += 4 * (to_encode[5 + already_written] - '0');
            cur_sum += 2 * (to_encode[6 + already_written] - '0');
            cur_sum += 1 * (to_encode[7 + already_written] - '0');

            fwrite(&cur_sum, sizeof(char), 1, outfp);
            already_written += 8;
        }
        fclose(outfp);
        free_tree(list->firstitem->leaf);
        remove_list(list);
        free(text);
        free(complete);
        free(buffer);
    }
    fclose(fp);

}

/* Er wordt een lijst opgesteld geordend op frequenties van het voorkomen van een karakter */
List* make_freq_list(char* text){
    // 256, aangezien dit alle mogelijke karakters zijn.
    int freq[256] = {0};

    for (int i = 0; i < BUFSIZE; i++) {
        char character = text[i];
        freq[character] = freq[character] + 1;
    }

    List* list = new_list();
    for(int i = 0; i < 256; i++){
        if(freq[i] != 0 && i != NULL){
            Item* item = new_item(freq[i], (unsigned char) i, NULL);
            add_element_freq(list, item);
        }
    }
    return list;
}

/* Er wordt een array die wijst naar de juiste toppen opgesteld, samen met de eigenlijke boom. */
void make_encoding_huffman_tree(encode_Item* encode_array, List* list){

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
}

/* Methode die de originele tekst omzet in huffmancodes */
char* encode_text(int aantal_char, const char* text, encode_Item* encode_array, int* cur_siz, char** complete_code){
    int current_size = *cur_siz;
    for(int i = 0; i < aantal_char; i++){
        char current_char  = text[i];
        char code[255];
        // het juiste item in de lijst met pointers naar de juiste toppen wordt gezocht.
        int j = 0;
        while(current_char != encode_array[j].data){
            j++;
        }
        encode_Item current_item = encode_array[j];
        Leaf* current_leaf = current_item.code;

        // De code wordt in de tijdelijke code char array gestopt. Dit is in de omgekeerde volgorde dan ze
        // gebruikt moet worden
        int iterations = 255;
        while(current_leaf->parent != NULL){
            if(current_leaf->zero_or_one_child == 0){
                code[iterations] = 48;
            } else{
                code[iterations] = 49;
            }
            current_leaf = current_leaf->parent;
            iterations--;
        }
        // Hier wordt de  code omgedraaid zodat deze klaar is voor uitschrijven.
        current_size += (255-iterations);
        *complete_code = (char*) realloc(*complete_code, (current_size) * sizeof(int));

        int end_code = iterations + 1;
        for (int k = current_size - ((255) - iterations); k < current_size; k++) {
            (*complete_code)[k] = code[end_code];
            end_code++;
        }
    }
    (*complete_code)[current_size] = '\0';
    *cur_siz = current_size;
    return *complete_code;
}

// Methode die recursief de huffman boom in bytes codeert om in de file mee te geven.
// Dit kan efficienter geimplementeerd worden door in plaats van een 1 en een 0 als byte weg te schrijven
// een 1 of 0 bit weg te schrijven.
void encode_nodes(Leaf* leaf, char** buffer, int *bitnr, int *buffersize){
    // geen blad -> schrijf 0 naar buffer & resize
    // schrijf kinderen ook weg
    if(leaf->zero_child != NULL && leaf->one_child != NULL){
        *buffer = realloc(*buffer, *buffersize + sizeof(char));
        *buffersize += sizeof(char);
        (*buffer)[*bitnr] = 48;
        (*bitnr)++;
        encode_nodes(leaf->zero_child, buffer, bitnr, buffersize);
        encode_nodes(leaf->one_child, buffer, bitnr, buffersize);
    } else{ //Wel een blad -> schrijf een 1 en het karakter zelf weg naar buffer en resize eerst om plaats te maken
        *buffer = realloc(*buffer, *buffersize + (2*sizeof(char)));
        *buffersize += (2*sizeof(char));
        (*buffer)[*bitnr] = 49;
        (*buffer)[*bitnr + 1] = leaf->character;
        (*bitnr) += 2;
    }
}

//Methode die de boom wegschrijft na het encoderen
void write_tree(const char* filename, Leaf* parent_leaf, char* buffer){
    int buffersize = sizeof(char);
    int bitnr = 0;
    encode_nodes(parent_leaf, &buffer, &bitnr, &buffersize);
    FILE *fp = fopen(filename, "a+b");
    if(fp != NULL) {
        fwrite(&bitnr, sizeof(char), 1, fp);
        fwrite(buffer, sizeof(char), (size_t) bitnr, fp);
    }
    fclose(fp);
}

/* Zorgt voor het decoderen van het gecomprimeerde bestand */
void decodeer(const char* filename, const char* output_filename){
    // Zorgt ervoor dat het output bestand leeg is aangezien er telkens wordt geappend aan dit bestand
    FILE* clear = fopen(output_filename, "w");
    fclose(clear);

    FILE *fp = fopen(filename, "r");
    if(fp != NULL) {
        while(!fpeek(fp)) {
            // Leest in hoeveel karakters het originele deel van dit bestand bevatte en hoeveel karakters nodig
            // waren voor het encoderen hiervan.
            int aantal = 0;
            int aantalchar = 0;
            fread(&aantal, 4, 1, fp);
            fread(&aantalchar, 4, 1, fp);

            // De huffmanboom wordt ingelezen & oppnieuw opgebouwd
            Leaf *parent = read_huffman_tree(fp);
            unsigned char *text = calloc((size_t) aantalchar + 1, sizeof(char));
            fread(text, sizeof(char), (size_t) aantalchar, fp);
            // De tekst wordt gedecodeerd
            unsigned char *decoded = decode_text(text, parent, aantal);
            // De gedecodeerde tekst wordt uitgeschreven
            write_text(decoded, output_filename);

            free_tree(parent);
            free(text);
            free(decoded);
        }
    }
    fclose(fp);
}

/* Hulpfunctie die telkens uit de tekst de volgende byte haalt */
unsigned char readbyte(unsigned char* text, size_t max, size_t* place){
    if(*place >= max){
        return 0;
    }
    unsigned char cur = text[*place];
    (*place)++;
    return cur;
}

/* De boom wordt gedecodeerd en opnieuw opgebouwd */
Leaf* decode_tree(unsigned char* text, size_t max, size_t* place){
    char current = readbyte(text, max, place);
    if(current == 49){
        return make_only_leaf(NULL, NULL, readbyte(text, max, place), 2);
    } else{
        Leaf* zerochild = decode_tree(text, max, place);
        Leaf* onechild = decode_tree(text,max, place);
        return make_only_leaf(zerochild, onechild, 0, 2);
    }
}

/* De tekst nodig om de huffman boom opnieuw op te bouwen wordt ingelezen */
Leaf* read_huffman_tree(FILE *fp){
    unsigned char *text;
    char size_tree = 0;
    fread(&size_tree, sizeof(char), 1, fp);
    text = calloc((size_t) size_tree + 1, sizeof(char));
    size_t size = fread(text, sizeof(char), (size_t) size_tree, fp);
    text[size] = '\0';

    make_only_leaf(NULL, NULL, 0, 2);
    size_t index = 0;
    Leaf* result = decode_tree(text, (size_t) size_tree, &index);
    free(text);
    return result;
}

/* De tekst wordt gedecodeerd aan de hand van de opnieuw opgebouwde huffmanboom */
unsigned char* decode_text(unsigned char* text, Leaf* leaf, int aantal){
    unsigned char* decoded_text = (unsigned char*) calloc(sizeof(char),(size_t) aantal + 1);
    Leaf* cur_leaf = leaf;
    int nr_decoded = 0;
    int nr_char = 0;
    size_t shift = 7;
    unsigned char mask = 255;
    while(nr_decoded!=aantal){
        while(!cur_leaf->character) {
            //decode 1 teken
            unsigned char cur_char = text[nr_char];
            unsigned char cur_byte = cur_char & mask;
            unsigned int cur_bit = cur_byte >> shift;
            if (cur_bit == 1) {
                cur_leaf = cur_leaf->one_child;
            } else if(cur_bit == 0){
                cur_leaf = cur_leaf->zero_child;
            }
            // De mask en shift en nr_char waardes worden aangepast indien nodig
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
    }
    return decoded_text;
}

/* De gedecodeerde tekst wordt opnieuw uitgeschreven */
void write_text(unsigned char* decoded_text, const char* output_file){
    FILE *fp = fopen(output_file, "a+b");

    if(fp != NULL) {
        fwrite(decoded_text, sizeof(unsigned char), strlen(decoded_text), fp);
    }
    fclose(fp);
}

