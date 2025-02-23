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
    //hysteresisThresholding(argv[1]);
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
}

