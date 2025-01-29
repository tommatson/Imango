#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "greyscale.h"
#include "mango-maths.h"


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

typedef struct 
{
    unsigned char red;
    unsigned char green;
    unsigned char blue;
} RGB;


double approximateExponential(double squaredPart){
    double exponentialSum = 1;
    double term = 1.0;
    for (int j = 1; j < 100; j++){
        term *= squaredPart / j;
        // Stop if its getting really small (we don't need that amount of accuracy)
        if((term > 0 ? term : (-1 * term)) < 1e-10) break;
        exponentialSum += term;
    } 
    return exponentialSum;
}


float calculateKernelItem(float stanDev, int i, int kernelWidth){
    // Calculate x value within the kernel
    int x = (kernelWidth / 2) - (i - (kernelWidth * (i / kernelWidth)));
    // Calculate y value within the kernel
    int y = (kernelWidth / 2) - (i / kernelWidth);
    // The power of the exponential
    double squaredPart = -((x * x + y * y) / (2 * stanDev * stanDev));
    // Estimating the exponential using the taylor series
    double exponentialPart = approximateExponential(squaredPart);
    // Finally, calculate the gaussian value
    printf("\nGauss: %lf", truncate(((1/(2 * pi * stanDev * stanDev)) * exponentialPart), 3));
    return round(((1/(2 * pi * stanDev * stanDev)) * exponentialPart), 3);
    
}



void gaussianConvert(const char* inputFile, int kernelWidth, float stanDev){
    if (kernelWidth % 2 != 1){
        perror("Kernel width must be an odd integer");
        exit(EXIT_FAILURE);
    }
    // For canny edge detection image should first be converted to greyscale
    // the stanDev parameter stores the standard deviation for the gaussian blur function, I recommend using 1
    unsigned int kernelSpace = sizeof(float) * (kernelWidth * kernelWidth);
    // malloc the kernel
    float* kernel = (float*)malloc(kernelSpace);

    if (!kernel){
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }
    // Store the magnitude sum ready for normalisation
    double kernelMagnitude = 0.0;
    // Calculate the kernel with the given parameters
    for (int i = 0; i < (kernelWidth * kernelWidth); i++){
        kernel[i] = calculateKernelItem(stanDev, i, kernelWidth);
        kernelMagnitude += kernel[i];
        printf("\n::%lf", kernel[i]);
    }
    printf("\nsum:%lf", kernelMagnitude);


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
    printf("BMP bits:%d", dibHeader.bitCount);
    // Check we are in 24 bit
    if (dibHeader.bitCount != 24){
        perror("24 bit BMP files are only supported at the moment :(");
        fclose(inputBMP);
        exit(EXIT_FAILURE);
    }
    // Create the output file
    char* outputFileName = (char*)malloc(strlen(inputFile) + 11); // size of inputfile + "greyscale" + \0
    strncpy(outputFileName, inputFile, (strlen(inputFile) - 4));
    strcat(outputFileName, "_gaussian.bmp\0");
    
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

    int kernelRowSize = rowSize * kernelWidth;
    unsigned char* kernelRow = malloc(kernelRowSize);
    
    if(!row){
        perror("Memory allocation failed");
        fclose(inputBMP);
        fclose(outputBMP);
        exit(EXIT_FAILURE);
    }
    if(!kernelRow){
        perror("Memory allocation failed");
        fclose(inputBMP);
        fclose(outputBMP);
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < (height > 0 ? height : -1 * height); i++){
      
        fread(row, rowSize, 1, inputBMP);
        for (int j = 0; j < (width > 0 ? width : -1 * width); j++){
            RGB* individualPixel = (RGB *)&row[j*3]; // Set each pixel as a pointer to the pixel in row
            // Rewrite the RGB values for each pixel in row
            // Do the kernel stuff
            
        }
        // Write the gaussian row to the output file
        fwrite(row, rowSize, 1, outputBMP);
    }
    fclose(inputBMP);
    fclose(outputBMP);
    free(row);
    printf("\nGaussian blur has been written successfully!");

}



int main(int argc, char* argv[]){

    if (argc != 4){
        fprintf(stderr, "Usage: %s <input .bmp file> <int kernel width> <standard deviation>", argv[0]);
        return EXIT_FAILURE;
    }
    gaussianConvert(argv[1], atoi(argv[2]), atof(argv[3]));
    // //printf("%f", power(atoi(argv[1]), atof(argv[2])));
    // printf("\n%d", atof(argv[1]));
    // double x = 5.0;
    // printf("\n%lf", approximateExponential(atof(argv[1])));
    // //greyscaleConvert(argv[1]);
    // printf("\n%lf", round(atof(argv[1]), 3));
    // printf("\n%lf", (atof(argv[1]) * power(10.0, 3) - truncate(atof(argv[1]), 3) * power(10.0, 3)));
    // printf("\n%lf", (truncate(atof(argv[1]), 3) * power(10.0, 3)));
    return EXIT_SUCCESS;

}