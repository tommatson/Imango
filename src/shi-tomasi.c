#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "mango-maths.h"

#define pi 3.1415926535897932

// Constants to be tweaked
#define kappa 0.06
#define THRESHOLD_PROPORTION 0.01

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

int shiPixelFinal(float pixel){
    pixel = abs(pixel);
    if (pixel > 255){
        pixel = 255;
    }
    return round(pixel, 0);

}


bool calculateCorner(RGB* uncalcxKernel[], RGB* uncalcyKernel[], long int maxMc){
    // 1. We calculate structure tensor matrix
    // 2. We compute eigenvalues for said structure tensor matrix
    // 3. Compare eigenvalues and determine whether they constitute a corner or not
    // uncalcKernel s store our pixel rgb values, we need to create the structure tensory matrix then use this to calculate the eigenvalues
    long long int sumIxIy = 0;

    long long int sumIx2 = 0;
    long long int sumIy2 = 0;
    for (int i = 0; i < (9); i++){
        //printf("x: %d, y: %d\n", uncalcxKernel[i]->red, uncalcyKernel[i]->red);
        sumIxIy += ((uncalcxKernel[i]->red + uncalcxKernel[i]->green + uncalcxKernel[i]->blue) / 3) * ((uncalcyKernel[i]->red + uncalcyKernel[i]->green + uncalcyKernel[i]->blue) / 3);
        sumIx2 += ((uncalcxKernel[i]->red + uncalcxKernel[i]->green + uncalcxKernel[i]->blue) / 3) * ((uncalcxKernel[i]->red + uncalcxKernel[i]->green + uncalcxKernel[i]->blue) / 3);
        sumIy2 += ((uncalcyKernel[i]->red + uncalcyKernel[i]->green + uncalcyKernel[i]->blue) / 3) * ((uncalcyKernel[i]->red + uncalcyKernel[i]->green + uncalcyKernel[i]->blue) / 3);
    }


    long int Mc = sumIx2 * sumIy2 - sumIxIy * sumIxIy - kappa * (sumIx2 + sumIy2) * (sumIx2 + sumIy2);
    // Check if it above a certain proportion of our maximum value for Mc
    if (Mc > THRESHOLD_PROPORTION * maxMc){
        return true;
    }
    
    return false;
}



long double calculateMaximumMc(RGB* uncalcxKernel[], RGB* uncalcyKernel[], long double maxMc){
    // 1. We calculate structure tensor matrix
    // 2. We compute eigenvalues for said structure tensor matrix
    // 3. Compare eigenvalues and determine whether they constitute a corner or not
    // uncalcKernel s store our pixel rgb values, we need to create the structure tensory matrix then use this to calculate the eigenvalues
    long long int sumIxIy = 0;

    long long int sumIx2 = 0;
    long long int sumIy2 = 0;
    for (int i = 0; i < (9); i++){
        //printf("x: %d, y: %d\n", uncalcxKernel[i]->red, uncalcyKernel[i]->red);
        sumIxIy += ((uncalcxKernel[i]->red + uncalcxKernel[i]->green + uncalcxKernel[i]->blue) / 3) * ((uncalcyKernel[i]->red + uncalcyKernel[i]->green + uncalcyKernel[i]->blue) / 3);
        sumIx2 += ((uncalcxKernel[i]->red + uncalcxKernel[i]->green + uncalcxKernel[i]->blue) / 3) * ((uncalcxKernel[i]->red + uncalcxKernel[i]->green + uncalcxKernel[i]->blue) / 3);
        sumIy2 += ((uncalcyKernel[i]->red + uncalcyKernel[i]->green + uncalcyKernel[i]->blue) / 3) * ((uncalcyKernel[i]->red + uncalcyKernel[i]->green + uncalcyKernel[i]->blue) / 3);
    }
    //printf("IxIy: %d\nIx2: %d\nIy2: %d", sumIxIy, sumIx2, sumIy2);
    // // Now we have calculated all the parts of the structure tensor matrix, we can use these to calculate the eigenvalues of the matrix 
    // long int eigenvalue1 = ((sumIx2 + sumIy2) + Q_rsqrt(1.0 / (((sumIx2 + sumIy2) * (sumIx2 + sumIy2)) - 4 * (sumIy2 * sumIx2 - (sumIxIy * sumIxIy))))) / 2;
    // long int eigenvalue2 = ((sumIx2 + sumIy2) - Q_rsqrt(1.0 / (((sumIx2 + sumIy2) * (sumIx2 + sumIy2)) - 4 * (sumIy2 * sumIx2 - (sumIxIy * sumIxIy))))) / 2;
    // // This value is used to compare eigenvalues to see corner or not
    long double Mc = sumIx2 * sumIy2 - sumIxIy * sumIxIy - kappa * (sumIx2 + sumIy2) * (sumIx2 + sumIy2);
    if (Mc > maxMc){
        return Mc;
    } 

    
    return maxMc;
}

char* cornerDetect(char* xInputFile, char* yInputFile){

    // FILE WRITING ---------------------------

    FILE* xInputBMP = fopen(xInputFile, "rb");
    if (!xInputBMP){
        perror("Error opening x gradient file, are you sure it exists?");
        exit(EXIT_FAILURE);
    }
    FILE* yInputBMP = fopen(yInputFile, "rb");
    if(!yInputBMP){
        perror("Error opening y gradient file, are you sure it exists?");
        exit(EXIT_FAILURE);
    }

    // Only read the headers from the x gradient file as we assume x and y have been written by the same script (at least, they should have been)
    BMPheader bmpHeader;
    fread(&bmpHeader, sizeof(BMPheader), 1, xInputBMP);

    // Check the type of the file
    if (bmpHeader.type != 0x4D42) {
        perror("Incorrect file type");
        fclose(xInputBMP);
        fclose(yInputBMP);
        exit(EXIT_FAILURE);
    }
    DIBheader dibHeader;
    fread(&dibHeader, sizeof(DIBheader), 1, xInputBMP);
    // Check we are in 24 bit
    if (dibHeader.bitCount != 24){
        perror("24 bit BMP files are only supported at the moment :(");
        fclose(xInputBMP);
        fclose(yInputBMP);
        exit(EXIT_FAILURE);
    }

    char* outputFileName = (char*)malloc(strlen(xInputBMP) + 6); // size of inputfile + "_corner" + \0
    strncpy(outputFileName, xInputFile, (strlen(xInputFile) - 6));
    strcat(outputFileName, "_corner.bmp\0");

    FILE* outputBMP = fopen(outputFileName, "wb");

    fwrite(&bmpHeader, sizeof(BMPheader), 1, outputBMP);
    fwrite(&dibHeader, sizeof(DIBheader), 1, outputBMP);

    // Move the file pointer to the correct location to begin reading and writing
    fseek(xInputBMP, bmpHeader.offset, SEEK_SET);
    fseek(yInputBMP, bmpHeader.offset, SEEK_SET);
    fseek(outputBMP, bmpHeader.offset, SEEK_SET);
    
    int height = dibHeader.height;
    int width = dibHeader.width;

    int rowSize = (((width * 3) + 3) & ~3);// Times by 3 because 3 bytes per pixel, add on 3 to account for padding (padding can be 0-3) then & with !3 to round to a multiple of 4
    unsigned char* row = malloc(rowSize); // Hold each row read from the BMP file

    // TRANSLATED PIXEL WRITING ------------------------
    
    if(!row){
        perror("Row memory allocation failed");
        fclose(xInputBMP);
        fclose(yInputBMP);
        fclose(outputBMP);
        exit(EXIT_FAILURE);
    }
    // Constant, this is modified code from my sobel script, which is modified code from gaussain
    int kernelWidth = 3;

    unsigned char* kernelxRow;
    unsigned char* kernelyRow;
    int kernelRowSize;
    bool incrementKernel = false;

    // Again, use the x file as the reference as x and y should be the same file except the rgb values will be different
    long kernelPosition = ftell(xInputBMP);

    // Used to hold where the 'middle' of the kernel if (target pixel)
    int kernelMiddleIndex = 0;

    // Used to store the maximum value of out calculated Mc for the first iteration
    long double maxMc = 0;

    // Usual incrementing through file 
    for (int i = 0; i < (height > 0 ? height : -1 * height); i++){
        // These if statements calculate the memory required for the kernel in horizontal strips
        if (((height > 0 ? height : -1 * height) - i) <= kernelWidth / 2){
            // In this one the kernel will now go over the image size
            // Imagine a 3x3 kernel on the final row of pixels, the bottom row of the kernel is out the image right? So this code calculates that the kernel needs to be a size of 2
            kernelRowSize = rowSize * ((kernelWidth / 2) + ((height > 0 ? height : -1 * height) - i));
            kernelxRow = malloc(kernelRowSize);
            kernelyRow = malloc(kernelRowSize);
            incrementKernel = true;
            kernelMiddleIndex = kernelWidth / 2;
        } else {
            if ((kernelWidth / 2) + i + 1 >= kernelWidth){
                // Here the entire kernel can fit within the image so we continue at its full size and increment it
                kernelRowSize = rowSize * kernelWidth;
                kernelxRow = malloc(kernelRowSize);
                kernelyRow = malloc(kernelRowSize);
                incrementKernel = true;
                kernelMiddleIndex = kernelWidth / 2;
            } else {
                // Here the kernel is at the top of the image
                // Imagine a 3x3 kernel with out target row being row 0 of the image, the top of the kernel is cut off so we need the row and the row below it
                kernelRowSize = rowSize * ((kernelWidth / 2) + i + 1);
                kernelxRow = malloc(kernelRowSize);
                kernelyRow = malloc(kernelRowSize);
                incrementKernel = false;
                kernelMiddleIndex = i;
            }
        }
        
        // Save the position
        kernelPosition = ftell(xInputBMP);
        // Read what we need to read 
        for (int j = 0; j < (kernelRowSize / rowSize); j++){
            fread(kernelxRow + j * rowSize, rowSize, 1, xInputBMP);
            fread(kernelyRow + j * rowSize, rowSize, 1, yInputBMP);
        }
        
        // Go back to the position we were at 
        fseek(xInputBMP, kernelPosition, SEEK_SET);
        fseek(yInputBMP, kernelPosition, SEEK_SET);
        
        if(incrementKernel){
            // Increment the read position
            fseek(xInputBMP, rowSize, SEEK_CUR);
            fseek(yInputBMP, rowSize, SEEK_CUR);
        }

        for (int j = 0; j < abs(width); j++){
            // Iterate through each pixel within the target row
            RGB* uncalcxKernel[kernelWidth * kernelWidth];
            RGB* uncalcyKernel[kernelWidth * kernelWidth];
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
                uncalcxKernel[k] = (RGB *)malloc(sizeof(RGB));
                uncalcyKernel[k] = (RGB *)malloc(sizeof(RGB));
                if (!kExists){
                    // Value does not exist, so set all values to zero
                    uncalcxKernel[k]->red = 0;
                    uncalcxKernel[k]->green = 0;
                    uncalcxKernel[k]->blue = 0;

                    uncalcyKernel[k]->red = 0;
                    uncalcyKernel[k]->green = 0;
                    uncalcyKernel[k]->blue = 0; 
                } else {
                    // Value does exist, so point to the correct row and column in kernel row to get the pixels
                    RGB* tempxPixel = kernelxRow + ((kernelMiddleIndex + (k / kernelWidth) - (kernelWidth / 2)) * rowSize) + (((k % kernelWidth) - (kernelWidth / 2) + j)* sizeof(RGB));
                    uncalcxKernel[k]->red = tempxPixel->red;
                    uncalcxKernel[k]->green = tempxPixel->green;
                    uncalcxKernel[k]->blue = tempxPixel->blue;

                    RGB* tempyPixel = kernelyRow + ((kernelMiddleIndex + (k / kernelWidth) - (kernelWidth / 2)) * rowSize) + (((k % kernelWidth) - (kernelWidth / 2) + j)* sizeof(RGB));
                    uncalcyKernel[k]->red = tempyPixel->red;
                    uncalcyKernel[k]->green = tempyPixel->green;
                    uncalcyKernel[k]->blue = tempyPixel->blue; 
                }
            }
            // Calculate the pixel values with the kernel and set it in row
            maxMc = calculateMaximumMc(uncalcxKernel, uncalcyKernel, maxMc);

            // Free the data structure as it will be defined again next iteration
            for (int k = 0; k < (kernelWidth * kernelWidth); k++){
                free(uncalcxKernel[k]);   
                free(uncalcyKernel[k]);
            }

        }
        // Free the kernel
        free(kernelxRow);
        free(kernelyRow);
    }

    // We do 2 iterations of the file, 1 to find the max value, 2 to compare everthing to that max threshold

    // Reset file pointers
    fseek(xInputBMP, bmpHeader.offset, SEEK_SET);
    fseek(yInputBMP, bmpHeader.offset, SEEK_SET);

    // Do the second file iteration
    for (int i = 0; i < (height > 0 ? height : -1 * height); i++){
        // These if statements calculate the memory required for the kernel in horizontal strips
        if (((height > 0 ? height : -1 * height) - i) <= kernelWidth / 2){
            // In this one the kernel will now go over the image size
            // Imagine a 3x3 kernel on the final row of pixels, the bottom row of the kernel is out the image right? So this code calculates that the kernel needs to be a size of 2
            kernelRowSize = rowSize * ((kernelWidth / 2) + ((height > 0 ? height : -1 * height) - i));
            kernelxRow = malloc(kernelRowSize);
            kernelyRow = malloc(kernelRowSize);
            incrementKernel = true;
            kernelMiddleIndex = kernelWidth / 2;
        } else {
            if ((kernelWidth / 2) + i + 1 >= kernelWidth){
                // Here the entire kernel can fit within the image so we continue at its full size and increment it
                kernelRowSize = rowSize * kernelWidth;
                kernelxRow = malloc(kernelRowSize);
                kernelyRow = malloc(kernelRowSize);
                incrementKernel = true;
                kernelMiddleIndex = kernelWidth / 2;
            } else {
                // Here the kernel is at the top of the image
                // Imagine a 3x3 kernel with out target row being row 0 of the image, the top of the kernel is cut off so we need the row and the row below it
                kernelRowSize = rowSize * ((kernelWidth / 2) + i + 1);
                kernelxRow = malloc(kernelRowSize);
                kernelyRow = malloc(kernelRowSize);
                incrementKernel = false;
                kernelMiddleIndex = i;
            }
        }
        
        // Save the position
        kernelPosition = ftell(xInputBMP);
        // Read what we need to read 
        for (int j = 0; j < (kernelRowSize / rowSize); j++){
            fread(kernelxRow + j * rowSize, rowSize, 1, xInputBMP);
            fread(kernelyRow + j * rowSize, rowSize, 1, yInputBMP);
        }
        
        // Go back to the position we were at 
        fseek(xInputBMP, kernelPosition, SEEK_SET);
        fseek(yInputBMP, kernelPosition, SEEK_SET);
        
        if(incrementKernel){
            // Increment the read position
            fseek(xInputBMP, rowSize, SEEK_CUR);
            fseek(yInputBMP, rowSize, SEEK_CUR);
        }

        for (int j = 0; j < abs(width); j++){
            // Iterate through each pixel within the target row
            RGB* uncalcxKernel[kernelWidth * kernelWidth];
            RGB* uncalcyKernel[kernelWidth * kernelWidth];
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
                uncalcxKernel[k] = (RGB *)malloc(sizeof(RGB));
                uncalcyKernel[k] = (RGB *)malloc(sizeof(RGB));
                if (!kExists){
                    // Value does not exist, so set all values to zero
                    uncalcxKernel[k]->red = 0;
                    uncalcxKernel[k]->green = 0;
                    uncalcxKernel[k]->blue = 0;

                    uncalcyKernel[k]->red = 0;
                    uncalcyKernel[k]->green = 0;
                    uncalcyKernel[k]->blue = 0; 
                } else {
                    // Value does exist, so point to the correct row and column in kernel row to get the pixels
                    RGB* tempxPixel = kernelxRow + ((kernelMiddleIndex + (k / kernelWidth) - (kernelWidth / 2)) * rowSize) + (((k % kernelWidth) - (kernelWidth / 2) + j)* sizeof(RGB));
                    uncalcxKernel[k]->red = tempxPixel->red;
                    uncalcxKernel[k]->green = tempxPixel->green;
                    uncalcxKernel[k]->blue = tempxPixel->blue;

                    RGB* tempyPixel = kernelyRow + ((kernelMiddleIndex + (k / kernelWidth) - (kernelWidth / 2)) * rowSize) + (((k % kernelWidth) - (kernelWidth / 2) + j)* sizeof(RGB));
                    uncalcyKernel[k]->red = tempyPixel->red;
                    uncalcyKernel[k]->green = tempyPixel->green;
                    uncalcyKernel[k]->blue = tempyPixel->blue; 
                }
            }
            // Calculate the pixel values with the kernel and set it in row
            if (calculateCorner(uncalcxKernel, uncalcyKernel, maxMc)){
                // BGR
                row[j * 3] = 00;
                row[(j * 3) + 1] = 0;
                row[(j * 3) + 2] = 255;
            } else {
                // row[j * 3] = shiPixelFinal(Q_rsqrt(uncalcxKernel[4]->red * uncalcxKernel[4]->red + uncalcyKernel[4]->red * uncalcyKernel[4]->red));
                // row[(j * 3) + 1] = shiPixelFinal(Q_rsqrt(uncalcxKernel[4]->green * uncalcxKernel[4]->green + uncalcyKernel[4]->green * uncalcyKernel[4]->green));
                // row[(j * 3) + 2] = shiPixelFinal(Q_rsqrt(uncalcxKernel[4]->blue * uncalcxKernel[4]->blue + uncalcyKernel[4]->blue * uncalcyKernel[4]->blue));
                row[j * 3] = 0;
                row[(j * 3) + 1] = 0;
                row[(j * 3) + 2] = 0;
            }
            
            // Free the data structure
            for (int k = 0; k < (kernelWidth * kernelWidth); k++){
                free(uncalcxKernel[k]);   
                free(uncalcyKernel[k]);
            }

        }
        // Write the sobel rows to the output files
        fwrite(row, rowSize, 1, outputBMP);
        // Free the kernel
        free(kernelxRow);
        free(kernelyRow);
    }

    free(row);

    fclose(xInputBMP);
    fclose(yInputBMP);
    fclose(outputBMP);
    
    printf("\nCorner detection has been written successfully!");
    return outputFileName;

}
