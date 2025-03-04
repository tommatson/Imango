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

#define MAGNITUDE_AND_ANGLE 0
#define X_AND_Y 1

char* applyCanny(const char* inputFile){
    char* greyscaleName = greyscaleConvert(inputFile);
    // By default use 3 for kernel width and 1 for standev, if the user wants to change this then they can not use the applyCanny function and instead apply each step manually
    char* gaussianName = gaussianConvert(greyscaleName, 3, 1);
    remove(greyscaleName);
    
    returnNames myReturnNames = sobelConvert(gaussianName, MAGNITUDE_AND_ANGLE);
    
    remove(gaussianName);

    char* suppressionName  = localMaximumSuppressionConvert(myReturnNames.name1, myReturnNames.name2);

    remove(myReturnNames.name1);
    remove(myReturnNames.name2);

    char* hysteresisName = hysteresisThresholding(suppressionName);
    remove(suppressionName);
    
    // Create the name for the output file
  
    char* outputFileName = (char*)malloc(strlen(inputFile) + 7); // size of inputfile + "_canny" + \0
    strncpy(outputFileName, inputFile, (strlen(inputFile) - 4));
    strcat(outputFileName, "_canny.bmp\0");
    rename(hysteresisName, outputFileName);

    return outputFileName;

}