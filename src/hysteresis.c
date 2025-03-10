#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

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
    // return true if it is a high or connected to a high, false if it is a low or is not connected to a high
    if (label[i]== 'H') return true;
    else if (label[i] == 'L') return false;
    // Set our value to low so we do not get a recurse check called on ourselves
    label[i] = 'L';
    // Calculate the following 3x3 kernel
    int offsets[8] = {-width - 1, -width, -width + 1, -1, 1, width-1, width, width+1};
    for(int j = 0; j < 8; j++){
        bool outofBounds = false;
        int k = i + offsets[j];
        // Check if k is out of bounds
        // In this case I is at the top or bottom of the image
        if ((k < 0) || (k >= width * height)) outofBounds = true;
        else if(i % width == 0){
            // I is on the far left
            if (j == 0 || j == 3 || j == 5) outofBounds = true;    
        } else if (i % width == width - 1){
            // I is on the far right
            if (j == 2 || j == 4 || j == 7) outofBounds = true;
        }
        // Use the boolean too see if k is in or out of bounds
        if (!outofBounds){
            // K is in bounds
            if (recurseCheck(k, label, width, height)){
                label[i] = 'H';
                return true;
            }
        }
    }
    // If nothing has been returned yet return false as we remain low
    return false;
}


char* hysteresisThresholding(const char* inputFile){

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

    FILE* outputBMP = fopen(outputFileName, "wb");
   

    fwrite(&bmpHeader, sizeof(BMPheader), 1, outputBMP);
    fwrite(&dibHeader, sizeof(DIBheader), 1, outputBMP);


    // Move the file pointer to the correct location to begin reading and writing
    fseek(inputBMP, bmpHeader.offset, SEEK_SET);
    fseek(outputBMP, bmpHeader.offset, SEEK_SET);
    
    int height = dibHeader.height;
    int width = dibHeader.width;

    // For this approach we will read the image row by row and then write it to the structure used to store each pixel level
    int rowSize = (((width * 3) + 3) & ~3);
    unsigned char* row = malloc(rowSize);

    if (!row){
        perror("Row memory allocation failed");
        fclose(inputBMP);
        fclose(outputBMP);
        exit(EXIT_FAILURE);
    }

    // // We read the image row by row and then translate this into the H, M, L  1D array
    // Values will probably need tweaking later on
    unsigned char lowerThreshold =  25;
    unsigned char upperThreshold =  30;

    // Creating a memory structure to store the level of each pixel
    // Divide width by 3 as each pixel takes up 3 values of r g b and we don't need that, use unsigned char to label them
    int labelSize = abs(width * height) * sizeof(char);
    char* label = malloc(labelSize);

    if (!label){
        perror("Label memory allocation failed");
        fclose(inputBMP);
        fclose(outputBMP);
        free(row);
        exit(EXIT_FAILURE);
    }

    // First for loop reads the entire file and just sorts everything 
    for (int i = 0 ; i < (abs(height)); i++){
        // Read each 'row' of the file
        fread(row, rowSize, 1, inputBMP);
        for (int j = 0; j < abs(width); j++){
            if (row[j * 3] >= upperThreshold){
                // Upper threshold met
                label[(i * width + j)] = 'U';
            } else if (row[j * 3] >= lowerThreshold){
                // Middle threshold met
                label[(i * width + j)] = 'M';
            } else {
                // Lower threshold met
                label[(i * width + j)] = 'L';

            }
        }
     
    }
    // Free the row as we do not use it anymore
    free(row);
    // Second iteration used perform thresholding
    for(int i = 0; i < abs(height * width); i++){
        if (label[i] == 'M'){
            // Medium found
            recurseCheck(i, label, abs(width), abs(height));
        }

    }

    // Output row is used to read the input file 
    unsigned char* outputRow = malloc(rowSize);
    // Check the malloc has worked
    if (!outputRow){
        perror("Output row memory allocation failed");
        fclose(inputBMP);
        fclose(outputBMP);
        free(label);
        exit(EXIT_FAILURE);
    }

    // Go back to the start of the file to begin writing to output
    fseek(inputBMP, bmpHeader.offset, SEEK_SET);
    
    // Read through the entire file
    for(int i = 0; i < abs(height); i++){
        fread(outputRow, rowSize, 1, inputBMP);
        for (int j = 0; j < abs(width); j++){
            RGB * pixel = (RGB *)&outputRow[j * 3];
            // Red blue and green should be the same but just in case I will average them
            int value = (pixel->red + pixel->blue + pixel->green) / 3;
            // If the pixel is a low, remove it, if its a high, set it to a max
            if (label[i * width + j] == 'L'){
                value = 0;
            } else {
                value = 255;
            }
            outputRow[j*3] = value;
            outputRow[(j * 3) + 1] = value;
            outputRow[(j * 3) + 2] = value;
        }
        fwrite(outputRow, rowSize, 1, outputBMP);
    }
    
    // Free the final heap data
    free(outputRow);
    free(label);
    
    // Close the files
    fclose(inputBMP);
    fclose(outputBMP);
    
    // Let the user know the process has been completed
    printf("\nHysteris thresholding has been written successfully!");
    return outputFileName;

}

