#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "greyscale.h"
#include "mango-maths.h"
#include "gaussian.h"
#include "sobel-operate.h"
#include "suppression.h"

// Kill me 
extern unsigned int sleep(unsigned int seconds);
// Kill me no more




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


bool recurseCheck(int i, char* label, int width, int height){
    sleep(1);
    printf("\n\nNEW RECUrSE %d", i);
    // return true if it is a high or connected to a high, false if it is a low or is not connected to a high
    if (label[i]== 'H'){
        printf("I'm high");
        return true;
    } else if (label[i] == 'L'){
        printf("I'm low");
        return false;
    }
    // Calculate the following 3x3 kernel
    int offsets[8] = {-width - 1, -width, -width + 1, -1, 1, width-1, width, width+1};
    for(int j = 0; j < 8; j++){
        int k = i + offsets[j];
        // Check if k is out of bounds
        if (!(k < 0 || k >= height || (k >= width ? (k % (width * (k / width)) < 0) : false) || (k >= width ? (k % (width * (k / width)) > width) : false))){
            // k is in bounds
            printf(" In bounds");
            // set our value to a low
            label[i] = 'L';
            if (recurseCheck(k, label, width, height)){
                label[i] = 'H';
                return true;
            } else {
                printf("found a low for %d", i);
            }
        } else {
            printf(" out of bounds");
        }
    }
    printf("I'm single");
    label[i] = 'L';
    return false;

}


void hysteresisThresholding(const char* inputFile){

    // FILE WRITING ---------------------------

    FILE* inputBMP = fopen(inputFile, "rb");

    if (!inputBMP){
        perror("Error opening file, are you sure it exists?");
        exit(EXIT_FAILURE);
    }
    // Extract the header file data 
    BMPheader bmpHeader;
    fread(&bmpHeader, sizeof(BMPheader), 1, inputBMP);

    // Check the type of the file
    if (bmpHeader.type != 0x4D42) {
        perror("Incorrect file type");
        fclose(inputBMP);
        exit(EXIT_FAILURE);
    }
    printf("Rizzler");
    DIBheader dibHeader;
    fread(&dibHeader, sizeof(DIBheader), 1, inputBMP);
    // Check we are in 24 bit
    if (dibHeader.bitCount != 24){
        perror("24 bit BMP files are only supported at the moment :(");
        fclose(inputBMP);
        exit(EXIT_FAILURE);
    }
    // Create the output file
    char* outputFileName = (char*)malloc(strlen(inputFile) + 24); // size of inputfile + "_hysteresisThresholding" + \0
    strncpy(outputFileName, inputFile, (strlen(inputFile) - 4));
    strcat(outputFileName, "_hysteresisThresholding.bmp\0");
    printf("big booms");
    FILE* outputBMP = fopen(outputFileName, "wb");
    free(outputFileName);

    fwrite(&bmpHeader, sizeof(BMPheader), 1, outputBMP);
    fwrite(&dibHeader, sizeof(DIBheader), 1, outputBMP);


    // Move the file pointer to the correct location to begin reading and writing
    fseek(inputBMP, bmpHeader.offset, SEEK_SET);
    fseek(outputBMP, bmpHeader.offset, SEEK_SET);
    
    int height = dibHeader.height;
    int width = dibHeader.width;
    printf("ohio");
    // For this approach we will read the image row by row and then write it to the structure used to store each pixel level
    int rowSize = (((width * 3) + 3) & ~3);
    unsigned char* row = malloc(rowSize);
    printf("fanun");
    if (!row){
        perror("Row memory allocation failed");
        fclose(inputBMP);
        fclose(outputBMP);
        exit(EXIT_FAILURE);
    }

    // // We read the image row by row and then translate this into the H, M, L  1D array
    // Values will probably need tweaking later on
    unsigned char lowerThreshold =  50;
    unsigned char upperThreshold =  150;

    // Creating a memory structure to store the level of each pixel
    // Divide width by 3 as each pixel takes up 3 values of r g b and we don't need that, use unsigned char to label them
    int labelSize = abs(width * height) * sizeof(char);
    char* label = malloc(labelSize);
    printf("Image is this fat: %d", abs(width * height));
    if (!label){
        perror("Label memory allocation failed");
        fclose(inputBMP);
        fclose(outputBMP);
        free(row);
        exit(EXIT_FAILURE);
    }
    //sleep(1);
    // First for loop reads the entire file and just sorts everything 
    for (int i = 0 ; i < (abs(height)); i++){
        //printf("day");
        fread(row, rowSize, 1, inputBMP);
        //printf("Emma");
        for (int j = 0; j < abs(width); j++){
            //printf("my baby");
            if (row[j * 3] >= upperThreshold){
                //printf("uppper");
                label[(i * width + j)] = 'U';
            } else if (row[j * 3] >= lowerThreshold){
                // Middle threshold met
                //printf("middle");
                label[(i * width + j)] = 'M';
            } else {
                //printf("\nlower i: %d, j: %d", i, j);
                label[(i * width + j)] = 'L';

            }
        }
     
    }

    // Making up data to test functionality
    int label2Size = 5 * 5 * sizeof(char);
    char* label2 = malloc(label2Size);

    printf("Gyatt %d, %d", width, height);
    // Second iteration used perform thresholding
    for(int i = 0; i < abs(height * width); i++){
        if (label[i] == 'M'){
            // Medium found
            printf("scooby do");
            recurseCheck(i, label, abs(width), abs(height));
        }

    }

    // printing it all for tests
    // for (int i = 0; i < abs(height * width); i++){
    //     if (i % (width * (i / width)))
    //     printf("%c", label[i]);
    // }
    

    fclose(inputBMP);
    fclose(outputBMP);
    
    printf("\nHysteris thresholding has been written successfully!");

}



int main(int argc, char* argv[]){
    setbuf(stdout, NULL);
    if (argc != 4){
        fprintf(stderr, "Usage: %s <input .bmp file> <int kernel width> <standard deviation>", argv[0]);
        return EXIT_FAILURE;
    }
    // greyscaleConvert(argv[1]);

    // char* outputFileName = (char*)malloc(strlen(argv[1]) + 11); // size of inputfile + "greyscale" + \0
    // strncpy(outputFileName, argv[1], (strlen(argv[1]) - 4));
    // strcat(outputFileName, "_greyscale.bmp\0");




    // gaussianConvert(outputFileName, atoi(argv[2]), atof(argv[3]));


    // char* outputFileName = (char*)malloc(strlen(argv[1]) + 20); // size of inputfile + "_gaussian" + \0
    // strncpy(outputFileName, argv[1], (strlen(argv[1]) - 4));
    // strcat(outputFileName, "_gaussian.bmp\0");


    // free(outputFileName);
    // char* outputFileName = (char*)malloc(strlen() + 23); // size of inputfile + "_localMaximumSuppression" + \0
    // strncpy(outputFileName, mInputFile, (strlen(mInputFile) - 14));
    // strcat(outputFileName, "_localMaximumSuppression.bmp\0");
    
    

    //printf("%f", (arctan(atof(argv[1]))));
    //printf("%f", round((squareRoot(atof(argv[1]), atof(argv[2]))), 0));
    //sobelConvert(argv[1]);
    //localMaximumSuppressionConvert(argv[1], argv[2]);
    hysteresisThresholding(argv[1]);
    //printf("%f", Q_rsqrt(1 / atof(argv[1])));
    
    // //printf("%f", power(atoi(argv[1]), atof(argv[2])));
    // printf("\n%d", atof(argv[1]));
    // double x = 5.0;
    // printf("\n%lf", approximateExponential(atof(argv[1])));
    // //greyscaleConvert(argv[1]);
    // printf("\n%lf", round(atof(argv[1]), atoi(argv[2])));
    // printf("\n%lf", truncate(atof(argv[1]), atoi(argv[2])));
    // printf("\n%lf", power(atof(argv[1]), atoi(argv[2])));
    // printf("\n%lf", (atof(argv[1]) * power(10.0, 3) - truncate(atof(argv[1]), 3) * power(10.0, 3)));
    // printf("\n%lf", (truncate(atof(argv[1]), 3) * power(10.0, 3)));
    return EXIT_SUCCESS;

}