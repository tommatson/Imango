#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "mango-maths.h"


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


RGB* suppressNonMax(RGB* uncalcKernel[], int angle){
    // Use the angle to compare to the right pixels (angle will be between 0-255 which we can then map to 0-360)
    angle = (angle / 255 ) * 360;
    if (angle > 180) angle -= 180;

    int pixel1;
    int pixel2;
    int targetPixel = uncalcKernel[4]->red;

    if ((angle >=0 && angle < 22.5) || (angle >= 157.5)) {
        // horizontal
        pixel1 = uncalcKernel[3]->red;
        pixel2  = uncalcKernel[5]->red;
    } else if (angle < 67.5 && angle >= 22.5){
        // top left diagonal
        pixel1 = uncalcKernel[0]->red;
        pixel2 = uncalcKernel[8]->red;
    } else if (angle >= 67.5 && angle < 112.5){
        // vertical
        pixel1 = uncalcKernel[1]->red;
        pixel2 = uncalcKernel[7]->red;
    } else if (angle >= 112.5 && angle < 157.5){
        // top right diagonal
        pixel1 = uncalcKernel[2]->red;
        pixel2 = uncalcKernel[6]->red;
    }
    RGB* pixel = (RGB *)malloc(3 * sizeof(RGB));
    pixel->red = 0;
    pixel->green = 0;
    pixel->blue = 0;
    if(targetPixel >= pixel1 && targetPixel >= pixel2){
        pixel->red = uncalcKernel[4]->red;
        pixel->green = uncalcKernel[4]->green;
        pixel->blue = uncalcKernel[4]->blue;
        // pixel->red = 255;
        // pixel->green = 255;
        // pixel->blue = 255;
    } 
    return pixel;
}

char* localMaximumSuppressionConvert(const char* mInputFile, const char* aInputFile){

    // FILE WRITING ---------------------------

    FILE* mInputBMP = fopen(mInputFile, "rb");
    FILE* aInputBMP = fopen(aInputFile, "rb");
    if (!mInputBMP || !aInputBMP){
        perror("Error opening file, are you sure it exists?");
        exit(EXIT_FAILURE);
    }
    // All header values are taken from the x input file, as we assume the x and y files have the same header data
    BMPheader bmpHeader;
    fread(&bmpHeader, sizeof(BMPheader), 1, mInputBMP);

    // Check the type of the file
    if (bmpHeader.type != 0x4D42) {
        perror("Incorrect file type");
        fclose(mInputBMP);
        exit(EXIT_FAILURE);
    }
    DIBheader dibHeader;
    fread(&dibHeader, sizeof(DIBheader), 1, mInputBMP);
    // Check we are in 24 bit
    if (dibHeader.bitCount != 24){
        perror("24 bit BMP files are only supported at the moment :(");
        fclose(mInputBMP);
        exit(EXIT_FAILURE);
    }
    // Create the output file
    char* outputFileName = (char*)malloc(strlen(mInputFile) + 23); // size of inputfile + "_localMaximumSuppression" + \0
    strncpy(outputFileName, mInputFile, (strlen(mInputFile) - 14));
    strcat(outputFileName, "_localMaximumSuppression.bmp\0");
    
    FILE* outputBMP = fopen(outputFileName, "wb");
    fwrite(&bmpHeader, sizeof(BMPheader), 1, outputBMP);
    fwrite(&dibHeader, sizeof(DIBheader), 1, outputBMP);


    // Move the file pointer to the correct location to begin reading and writing
    fseek(mInputBMP, bmpHeader.offset, SEEK_SET);
    fseek(aInputBMP, bmpHeader.offset, SEEK_SET);
    fseek(outputBMP, bmpHeader.offset, SEEK_SET);
    
    int height = dibHeader.height;
    int width = dibHeader.width;

    int rowSize = (((width * 3) + 3) & ~3);// Times by 3 because 3 bytes per pixel, add on 3 to account for padding (padding can be 0-3) then & with !3 to round to a multiple of 4
    unsigned char* oRow = malloc(rowSize); // For the output to be written to
    unsigned char* aRow = malloc(rowSize); // Used to read the angle from the angle BMP file

    // TRANSLATED PIXEL WRITING ------------------------
    
    if(!oRow || !aRow){
        perror("Memory allocation failed");
        fclose(mInputBMP);
        fclose(aInputBMP);
        fclose(outputBMP);
        exit(EXIT_FAILURE);
    }

    int kernelWidth = 3;

    unsigned char* kernelRow;
    int kernelRowSize;
    bool incrementKernel = false;

    long kernelPosition = ftell(mInputBMP);

    int kernelMiddleIndex = 0;

    for (int i = 0; i < (height > 0 ? height : -1 * height); i++){
        // These if statements calculate the memory required for the kernel in horizontal strips
        // The kernel is used to store the 3x3 grid of pixels surrounding the target pixel only for the magnitude
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
        
        // Save the position for the magnitude file
        kernelPosition = ftell(mInputBMP);
        // Read what we need to read from the magnitude file
        for (int j = 0; j < (kernelRowSize / rowSize); j++){
            fread(kernelRow + j * rowSize, rowSize, 1, mInputBMP);
        }
        
        // Go back to the position we were at inside the magnitude file 
        fseek(mInputBMP, kernelPosition, SEEK_SET);
        
        if(incrementKernel){
            // Increment the read position
            fseek(mInputBMP, rowSize, SEEK_CUR);
            
        }

        // For the angle just read the file line by line into aRow
        fread(aRow, rowSize, 1, aInputBMP);

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
            RGB* pixelAngle = (RGB *)&aRow[j * sizeof(RGB)];
            // Calculate the pixel values with the kernel and set it in row
            RGB* calculatedPixel = suppressNonMax(uncalcKernel, pixelAngle->red);

            oRow[j * 3] = calculatedPixel->red;
            oRow[(j * 3) + 1] = calculatedPixel->green;
            oRow[(j * 3) + 2] = calculatedPixel->blue;
            free(calculatedPixel);
   
            // Free the data structure
            for (int k = 0; k < (kernelWidth * kernelWidth); k++){
                free(uncalcKernel[k]);   
            }
        }
        // Write the sobel rows to the output files
        fwrite(oRow, rowSize, 1, outputBMP);
        free(kernelRow);
    }
    free(aRow);
    free(oRow);
    fclose(mInputBMP);
    fclose(aInputBMP);
    fclose(outputBMP);
    
    printf("\nLocal non maximum suppression has been written successfully!");
    return outputFileName;

}