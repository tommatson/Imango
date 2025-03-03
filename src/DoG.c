#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "mango-maths.h"
#include "gaussian.c"

#define pi 3.1415926535897932

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



char* differenceOfGaussians(const char* inputFile, int kernelWidth, float stanDev){


    // FILE WRITING ---------------------------

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
    char* outputFileName = (char*)malloc(strlen(inputFile) + 5); // size of inputfile + "_gaussian" + \0
    strncpy(outputFileName, inputFile, (strlen(inputFile) - 4));
    strcat(outputFileName, "_DoG.bmp\0");
    
    FILE* outputBMP = fopen(outputFileName, "wb");

    fwrite(&bmpHeader, sizeof(BMPheader), 1, outputBMP);
    fwrite(&dibHeader, sizeof(DIBheader), 1, outputBMP);

    // Move the file pointer to the correct location to begin reading and writing
    fseek(inputBMP, bmpHeader.offset, SEEK_SET);
    fseek(outputBMP, bmpHeader.offset, SEEK_SET);
    
    int height = dibHeader.height;
    int width = dibHeader.width;

    // Create the filenames for the gaussians

    char* input1FileName = (char*)malloc(strlen(inputFile) + 2); // size of inputfile + "1" + \0
    strncpy(input1FileName, inputFile, (strlen(inputFile) - 4));
    strcat(input1FileName, "1.bmp\0");

    char* input2FileName = (char*)malloc(strlen(inputFile) + 2); // size of inputfile + "2 + \0
    strncpy(input2FileName, inputFile, (strlen(inputFile) - 4));
    strcat(input2FileName, "2.bmp\0");

    char* gaussianOut1 = gaussianConvert(input1FileName, 3, 1);
    char* gaussianOut2 = gaussianConvert(input2FileName, 3, 2);

    free(input1FileName);
    free(input2FileName);

    // Buffer creation for efficiency
    int imageSize = abs(height * width) * 3; 
    // Buffer to hold output
    char* outputBuffer = (char *)malloc(imageSize);
    // Buffer to hold pixel values
    char* gaussian1Buffer = (char *)malloc(imageSize);
    char* gaussian2Buffer = (char *)malloc(imageSize);

    if(!outputBuffer || !gaussian1Buffer || !gaussian2Buffer){
        perror("Error allocating memory, file is not large enough for heap");
        exit(EXIT_FAILURE);
        fclose(inputBMP);
        fclose(outputBMP);
    }

    // Open gaussian files
    FILE* gaussian1File = fopen(gaussianOut1, "rb");
    FILE* gaussian2File = fopen(gaussianOut2, "rb");

    if(!gaussian1File || !gaussian2File){
        perror("Error opening gaussian files");
        exit(EXIT_FAILURE);
        fclose(inputBMP);
        fclose(outputBMP);
        free(outputBuffer);
        free(gaussian1Buffer);
        free(gaussian2Buffer);
    }

    // Seek to the correct positions
    fseek(gaussian1File, bmpHeader.offset, SEEK_SET);
    fseek(gaussian2File, bmpHeader.offset, SEEK_SET);

    // Read the gaussian files into the buffers
    fread(gaussian1Buffer, imageSize, 1, gaussian1File);
    fread(gaussian2Buffer, imageSize, 1, gaussian2File);


    // Now iterate throw the gaussians and subtract the pixel values
    for (int i = 0; i < abs(height * width); i++){
        outputBuffer[i * 3] = (gaussian1Buffer[i * 3] - gaussian2Buffer[i * 3] > 0 ?  gaussian1Buffer[i * 3] - gaussian2Buffer[i * 3] : 0);
        outputBuffer[(i * 3) + 1] = (gaussian1Buffer[(i * 3) + 1] - gaussian2Buffer[(i * 3) + 1] > 0 ?  gaussian1Buffer[(i * 3) + 1] - gaussian2Buffer[(i * 3) + 1] : 0);
        outputBuffer[(i * 3) + 2] = (gaussian1Buffer[(i * 3) + 2] - gaussian2Buffer[(i * 3) + 2] > 0 ?  gaussian1Buffer[(i * 3) + 2] - gaussian2Buffer[(i * 3) + 2] : 0);
    }
    fwrite(outputBuffer, imageSize, 1, outputBMP);

    free(outputBuffer);
    free(gaussian1Buffer);
    free(gaussian2Buffer);


    fclose(inputBMP);
    fclose(outputBMP);
    fclose(gaussian1File);
    fclose(gaussian2File);

    remove(gaussianOut1);
    remove(gaussianOut2);


    printf("\nDifference of gaussians has been written successfully!");
    return outputFileName;

}

