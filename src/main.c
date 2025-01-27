#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "greyscale.h"



int main(int argc, char* argv[]){

    if (argc != 2){
        fprintf(stderr, "Usage: %s <input .bmp file>", argv[0]);
        return EXIT_FAILURE;
    }
    //greyscaleConvert(argv[1]);
    return EXIT_SUCCESS;

}