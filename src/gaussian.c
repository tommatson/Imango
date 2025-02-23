#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "mango-maths.h"


// This is the code used to do gaussian blur, this code can be improved as I have noticed artifacts appear for larger kernels and smaller standard deviations
// I have some idea why these errors are occuring but for now it functions

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
    // Use the taylor series for e^x
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



double calculateKernelItem(float stanDev, int i, int kernelWidth){
    // Calculate x value within the kernel
    int x = (kernelWidth / 2) - (i - (kernelWidth * (i / kernelWidth)));
    // Calculate y value within the kernel
    int y = (kernelWidth / 2) - (i / kernelWidth);
    // The power of the exponential
    double squaredPart = -((x * x + y * y) / (2 * stanDev * stanDev));
    // Estimating the exponential using the taylor series
    double exponentialPart = approximateExponential(squaredPart);
    // Finally, calculate the gaussian value
    return round(((1/(2 * pi * stanDev * stanDev)) * exponentialPart), 3);
    
}


RGB* calculatePixel(RGB* uncalcKernel[], double* kernel, int kernelWidth){
    // uncalcKernel stores our pixel rgb values, whilst kernel stores the values we multiply the kernel with
    double newR = 0;
    double newG = 0;
    double newB = 0;
    for (int i = 0; i < (kernelWidth * kernelWidth); i++){
        newR += uncalcKernel[i]->red * kernel[i];
        newG += uncalcKernel[i]->green * kernel[i];
        newB += uncalcKernel[i]->blue * kernel[i];
    }
    RGB* pixel  = (RGB*)malloc(sizeof(RGB));
    pixel->red = round(newR, 0);
    pixel->green = round(newG, 0);
    pixel->blue = round(newB, 0);
    return pixel;
}


char* gaussianConvert(const char* inputFile, int kernelWidth, float stanDev){
    if (kernelWidth % 2 != 1){
        perror("Kernel width must be an odd integer");
        exit(EXIT_FAILURE);
    }
    // For canny edge detection image should first be converted to greyscale
    // the stanDev parameter stores the standard deviation for the gaussian blur function, I recommend using 1
    unsigned int kernelSpace = sizeof(double) * (kernelWidth * kernelWidth);
    // malloc the kernel
    double* kernel = (double*)malloc(kernelSpace);

    if (!kernel){
        perror("Memory allocation failed");
        exit(EXIT_FAILURE);
    }
    
    // Calculate the kernel with the given parameters
    for (int i = 0; i < (kernelWidth * kernelWidth); i++){
        kernel[i] = calculateKernelItem(stanDev, i, kernelWidth);
    }

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
    char* outputFileName = (char*)malloc(strlen(inputFile) + 10); // size of inputfile + "_gaussian" + \0
    strncpy(outputFileName, inputFile, (strlen(inputFile) - 4));
    strcat(outputFileName, "_gaussian.bmp\0");
    
    FILE* outputBMP = fopen(outputFileName, "wb");
    fwrite(&bmpHeader, sizeof(BMPheader), 1, outputBMP);
    fwrite(&dibHeader, sizeof(DIBheader), 1, outputBMP);

    // Move the file pointer to the correct location to begin reading and writing
    fseek(inputBMP, bmpHeader.offset, SEEK_SET);
    fseek(outputBMP, bmpHeader.offset, SEEK_SET);
    
    int height = dibHeader.height;
    int width = dibHeader.width;

    int rowSize = (((width * 3) + 3) & ~3);// Times by 3 because 3 bytes per pixel, add on 3 to account for padding (padding can be 0-3) then & with !3 to round to a multiple of 4
    unsigned char* row = malloc(rowSize); // Used top hold the row read from the BMP file

    // TRANSLATED PIXEL WRITING ------------------------
    
    if(!row){
        perror("Memory allocation failed");
        fclose(inputBMP);
        fclose(outputBMP);
        exit(EXIT_FAILURE);
    }

    unsigned char* kernelRow;
    int kernelRowSize;
    bool incrementKernel = false;

    long kernelPosition = ftell(inputBMP);

    int kernelMiddleIndex = 0;

    for (int i = 0; i < (height > 0 ? height : -1 * height); i++){
        // These if statements calculate the memory required for the kernel in horizontal strips
        if (((height > 0 ? height : -1 * height) - i) <= kernelWidth / 2){
            // In this one the kernel will now go over the image size
            // Imagine a 3x3 kernel on the final row of pixels, the bottom row of the kernel is out the image right? So this code calculates that the kernel needs to be a size of 2
            kernelRowSize = rowSize * ((kernelWidth / 2) + ((height > 0 ? height : -1 * height) - i));
            kernelRow = malloc(kernelRowSize);
            incrementKernel = true;
            kernelMiddleIndex = kernelWidth / 2;
        } else {
            if ((kernelWidth / 2) + i + 1 >= kernelWidth){
                // Here the entire kernel can fit within the image so we continue at its full size and increment it
                kernelRowSize = rowSize * kernelWidth;
                kernelRow = malloc(kernelRowSize);
                incrementKernel = true;
                kernelMiddleIndex = kernelWidth / 2;
            } else {
                // Here the kernel is at the top of the image
                // Imagine a 3x3 kernel with out target row being row 0 of the image, the top of the kernel is cut off so we need the row and the row below it
                kernelRowSize = rowSize * ((kernelWidth / 2) + i + 1);
                kernelRow = malloc(kernelRowSize);
                incrementKernel = false;
                kernelMiddleIndex = i;
            }
        }
        
        // Save the position
        kernelPosition = ftell(inputBMP);
        // Read what we need to read 
        for (int j = 0; j < (kernelRowSize / rowSize); j++){
            fread(kernelRow + j * rowSize, rowSize, 1, inputBMP);
        }
        
        // Go back to the position we were at 
        fseek(inputBMP, kernelPosition, SEEK_SET);
        
        if(incrementKernel){
            // Increment the read position
            fseek(inputBMP, rowSize, SEEK_CUR);
            
        }

        for (int j = 0; j < ((width > 0 ? width : -1 * width)); j++){
            // Iterate through each pixel within the target row
            RGB* uncalcKernel[kernelWidth * kernelWidth];
            for (int k = 0; k < (kernelWidth * kernelWidth); k++){
                // Check is the location exists for each value of k
                bool kExists = true;
                // Horizontal check
                if (k % kernelWidth > width - j){
                    // Out of right horizontal bound
                    kExists = false;
                }
                if (kExists && (j - (kernelWidth / 2) + (k % kernelWidth) + 1 <= 0)){
                    // Out of the left horizontal bound
                    kExists = false;
                }
                // Vertical check
                if (kExists && (kernelWidth / 2) - (k / kernelWidth) > i){
                    // Out of the top vertical bound
                    kExists = false;

                }
                if (kExists && (kernelWidth / 2) - (k / kernelWidth) <= i - (height > 0 ? height : -1 * height)){
                    // Out of the bottom vertical bound
                    kExists = false;
                }
                uncalcKernel[k] = (RGB *)malloc(sizeof(RGB));
                if (!kExists){
                    // Value does not exist, so set all values to zero
                    uncalcKernel[k]->red = 0;
                    uncalcKernel[k]->green = 0;
                    uncalcKernel[k]->blue = 0; 
                } else {
                    // Value does exist, so point to the correct row and column in kernel row to get the pixels
                    RGB* tempPixel = kernelRow + ((kernelMiddleIndex + (k / kernelWidth) - (kernelWidth / 2)) * rowSize) + (((k % kernelWidth) - (kernelWidth / 2) + j)* sizeof(RGB));
                    uncalcKernel[k]->red = tempPixel->red;
                    uncalcKernel[k]->green = tempPixel->green;
                    uncalcKernel[k]->blue = tempPixel->blue; 
                }
            }
            // Calculate the pixel values with the kernel and set it in row
            RGB * calculatedPixel = calculatePixel(uncalcKernel, kernel, kernelWidth);
            row[j * 3] = calculatedPixel->red;
            row[(j * 3) + 1] = calculatedPixel->green;
            row[(j * 3) + 2] = calculatedPixel->blue;
            // Free the data structure
            for (int k = 0; k < (kernelWidth * kernelWidth); k++){
                free(uncalcKernel[k]);   
            }
        }
        // Write the gaussian row to the output file
        fwrite(row, rowSize, 1, outputBMP);
        // Free kernel row
        free(kernelRow);
    }
    free(row);
    free(kernel);
    fclose(inputBMP);
    fclose(outputBMP);
    
    printf("\nGaussian blur has been written successfully!");
    return outputFileName;

}

