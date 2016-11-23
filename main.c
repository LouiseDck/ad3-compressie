#include <memory.h>
#include "comprimeer1.h"
#include "comprimeer2.h"

int main(int argc, char *argv[]) {
    char* inputfile = argv[2];
    char* outputfile = argv[3];
    if(strcmp(argv[1], "-d") == 0){
        decodeer(inputfile, outputfile);

    } else if(strcmp(argv[1], "-c") == 0){
        encodeer(read_file(inputfile), outputfile);
    }

    return 0;
}