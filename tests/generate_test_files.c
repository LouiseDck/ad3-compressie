#include <stdlib.h>
#include <stdio.h>
#include "generate_test_files.h"

int main(int argc, char *argv[]) {
    int i = 0;
    int number = 100;
    char filename[7];
    while(i < 20) {
        sprintf(filename, "test_%d", i);;
        generate_files(number, filename);
        number *= 2;
        i++;
    }
    return 0;
}

int compare( const void* a, const void* b) {
    unsigned long long ll_a = * ( (unsigned long long*) a );
    unsigned long long ll_b = * ( (unsigned long long*) b );

    if ( ll_a == ll_b ) return 0;
    else if ( ll_a < ll_b ) return -1;
    else return 1;
}

void longlong_to_char(long long int *ints, char **buffer, int *written, int number){
    for(int i = 0; i < number; i++) {
        char *cur = calloc(sizeof(char), 9);
        int w = sprintf(cur, "%lli", ints[i]);
        buffer[i] = cur;
        written[i] = w;
    }
}

void spec_write_text(char** decoded_text, int* sizes, FILE* outputfp, int number){
    char delim = ',';
    for(int i = 0; i < number - 1; i++){
        int size = sizes[i];
        fwrite(decoded_text[i], sizeof(char), (size_t) size, outputfp);
        fwrite(&delim, sizeof(char), 1, outputfp);
    }
    int size = sizes[number - 1];
    fwrite(decoded_text[number - 1], sizeof(char), (size_t) size, outputfp);

}

void generate_files(int number_of_numbers, char* filename){
    long long* nrs = calloc(sizeof(long long), number_of_numbers);
    if(nrs) {
        for (int i = 0; i < number_of_numbers; i++) {
            // genereer x aantal getallen
            long long r = rand();
            r >>= 31;
            r += rand();
            if (r > 9223372036854775807) {
                i--;
            } else {
                nrs[i] = r;
            }
        }
        // steek ze in een set (omv dubbels) (event)

        // sorteer en voeg toe aan een array
        qsort(nrs, (size_t) number_of_numbers, sizeof(long long), compare);
        // schrijf ze uit

        FILE *fp = fopen(filename, "w");

        char open = '[';
        char close = ']';
        fwrite(&open, sizeof(char), 1, fp);
        int *written = calloc(sizeof(int), number_of_numbers + 1);
        char **buffer = calloc(sizeof(char *), number_of_numbers);
        for (int i = 0; i < number_of_numbers; i++) {
            buffer[i] = calloc(sizeof(char), 9);
        }
        longlong_to_char(nrs, buffer, written, number_of_numbers);
        spec_write_text(buffer, written, fp, number_of_numbers);
        fwrite(&close, sizeof(char), 1, fp);
        fclose(fp);
        int ding = 0;
    }
}