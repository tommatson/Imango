#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "greyscale.h"
#include "sobel-operate.h"
#include "shi-tomasi.h"
#include "gaussian.h"


#define MAGNITUDE_AND_ANGLE 0
#define X_AND_Y 1


char* applyCorner(const char* inputFile){
    char* greyscaleName = greyscaleConvert(inputFile);
    // By default use 3 for kernel width and 1 for standev, if the user wants to change this then they can not use the applyCorner function and instead apply each step manually
    char* gaussianName = gaussianConvert(greyscaleName, 3, 1);
    remove(greyscaleName);
    
    returnNames myReturnNames = sobelConvert(gaussianName, X_AND_Y);
    remove(gaussianName);

    char* xName = myReturnNames.name1;
    char* yName = myReturnNames.name2;

    char* cornerName = cornerDetect(xName, yName);    
    
    remove(xName);
    remove(yName);

    return cornerName;
}

