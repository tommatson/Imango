#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "mango-maths.h"
#include "DoG.h"


#define MAGNITUDE_AND_ANGLE 0
#define X_AND_Y 1



char* applyBlobDetection(const char* inputFile){
    // Pass the input file into the DoG (Difference of Gaussians)
    // This converts the image to greyscale then applies 2 Gaussian kernels and subtracts pixel values
    char* DoGOut = differenceOfGaussians(inputFile);
    

    // return outputFileName;

}

