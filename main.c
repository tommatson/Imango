#include <stdio.h>
#include <stdlib.h>


typedef struct {
    unsigned short type;
    unsigned int size;
    unsigned short reserve1;
    unsigned short reserve2;
    unsigned int offset;
} BMPheader;

void grayscale(const char* inputFile){
    FILE* inputBMP = fopen(inputFile, "rb");
    if (!inputBMP){
        perror("Error opening file, are you sure it exists?");
        exit(EXIT_FAILURE);
    }
    BMPheader bmpHeader;
    fread(&bmpHeader, sizeof(BMPheader), 1, inputBMP);

    // Check the type of the file
    if (bmpHeader.type != 0x4D42) {
        perror("Incorrect file type");
        fclose(inputBMP);
        exit(EXIT_FAILURE);
    }



}



int main(int argc, char* argv[]){
    printf("Hello world!");
    if (argc != 2){
        fprintf(stderr, "Usage: %s <input .bmp file>", argv[0]);
    }
    grayscale(argv[0]);
    return EXIT_SUCCESS;

}