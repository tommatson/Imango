#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef struct {
    unsigned short type;
    unsigned int size;
    unsigned short reserve1;
    unsigned short reserve2;
    unsigned int offset;
} __attribute__((packed)) BMPheader;

typedef struct {
    unsigned int size;
    int width; // width can be negative
    int height; // height can be negative
    unsigned short planes;
    unsigned short bitCount;
    unsigned int compression;
    unsigned int imageSize;
    int hRez; // horizontal resolution
    int vRex; // vertical resolution
    unsigned int colourCount;
    unsigned int impColourCount;
} __attribute__((packed)) DIBheader;

typedef struct 
{
    unsigned char red;
    unsigned char green;
    unsigned char blue;
} RGB;


void greyscaleConvert(const char* inputFile){
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
    DIBheader dibHeader;
    fread(&dibHeader, sizeof(DIBheader), 1, inputBMP);
    // Check we are in 24 bit
    if (dibHeader.bitCount != 24){
        perror("24 bit BMP files are only supported at the moment :(");
        fclose(inputBMP);
        exit(EXIT_FAILURE);
    }
    // Create the output file
    char* outputFileName = (char*)malloc(strlen(inputFile) + 11); // size of inputfile + "greyscale" + \0
    strncpy(outputFileName, inputFile, (strlen(inputFile) - 4));
    strcat(outputFileName, "_greyscale.bmp\0");
    
    FILE* outputBMP = fopen(outputFileName, "wb");
    fwrite(&bmpHeader, sizeof(BMPheader), 1, outputBMP);
    fwrite(&dibHeader, sizeof(DIBheader), 1, outputBMP);

    // Move the file pointer to the correct location to begin writing
    fseek(inputBMP, bmpHeader.offset, SEEK_SET);
    fseek(outputBMP, bmpHeader.offset, SEEK_SET);
    
    int height = dibHeader.height;
    int width = dibHeader.width;

    int rowSize = (((width * 3) + 3) & ~3);// Times by 3 because 3 bytes per pixel, add on 3 to account for padding (padding can be 0-3) then & with !3 to round to a multiple of 4
    unsigned char* row = malloc(rowSize); // Used top hold the row read from the BMP file
    
    if(!row){
        perror("Memory allocation failed");
        fclose(inputBMP);
        fclose(outputBMP);
        exit(EXIT_FAILURE);
    }
    printf("Pixels in kernel: %d", ((width > 0 ? width : -1 * width)));
    for (int i = 0; i < (height > 0 ? height : -1 * height); i++){
      
        fread(row, rowSize, 1, inputBMP);
        for (int j = 0; j < (width > 0 ? width : -1 * width); j++){
            RGB* individualPixel = (RGB *)&row[j*3]; // Set each pixel as a pointer to the pixel in row
            // Rewrite the RGB values for each pixel in row
            unsigned char grey = (unsigned char)(0.299 * individualPixel->red + 0.587 * individualPixel->green + 0.114 * individualPixel->blue);
            individualPixel->red = grey;
            individualPixel->green = grey;
            individualPixel->blue = grey;
            
        }
        // Write the grayscale row to the output file
        fwrite(row, rowSize, 1, outputBMP);
    }
    fclose(inputBMP);
    fclose(outputBMP);
    free(row);
    printf("\nGreyscale has been written successfully!");
}


