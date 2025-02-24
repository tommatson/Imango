#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "greyscale.h"
#include "mango-maths.h"
#include "gaussian.h"
#include "sobel-operate.h"
#include "suppression.h"
#include "hysteresis.h"




char* applyCanny(const char* inputFile){
    char* greyscaleName = greyscaleConvert(inputFile);
    // By default use 3 for kernel width and 1 for standev, if the user wants to change this then they can not use the applyCanny function and instead apply each step manually
    char* gaussianName = gaussianConvert(greyscaleName, 3, 1);
    remove(greyscaleName);
    
    sobelConvert(gaussianName);
    remove(gaussianName);
    // The sobel one is weird as it produces 2 files
    // Create the name for the magnitude file
    char* mOutputFileName = (char*)malloc(strlen(gaussianName) + 11); // size of inputfile + "_magnitude" + \0
    strncpy(mOutputFileName, gaussianName, (strlen(gaussianName) - 4));
    strcat(mOutputFileName, "_magnitude.bmp\0");

    // Create the name for the angle file
    char* aOutputFileName = (char*)malloc(strlen(gaussianName) + 7); // size of inputfile + "_angle" + \0
    strncpy(aOutputFileName, gaussianName, (strlen(gaussianName) - 4));
    strcat(aOutputFileName, "_angle.bmp\0");

    char* suppressionName  = localMaximumSuppressionConvert(mOutputFileName, aOutputFileName);
    remove(mOutputFileName);
    remove(aOutputFileName);
    free(mOutputFileName);
    free(aOutputFileName);
    char* hysteresisName = hysteresisThresholding(suppressionName);
    remove(suppressionName);
    
    // Create the name for the output file
  
    char* outputFileName = (char*)malloc(strlen(inputFile) + 7); // size of inputfile + "_canny" + \0
    strncpy(outputFileName, inputFile, (strlen(inputFile) - 4));
    strcat(outputFileName, "_canny.bmp\0");
    rename(hysteresisName, outputFileName);

    return outputFileName;

}

