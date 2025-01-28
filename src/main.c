#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "greyscale.h"
#include "mango-maths.h"


#define pi 3.1415926535897932

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
    return truncate(((1/(2 * pi * stanDev * stanDev)) * exponentialPart), 3);
    
}



void gaussianConvert(const char* inputFile, int kernelWidth, float stanDev){
    // TO DO - make sure kernelWidth is odd


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
    }
    // Kernel normalise to preserve image brightness
    // for (int i = 0; i < (kernelWidth * kernelWidth); i++){
    //     kernel[i] /= kernelMagnitude;
    //     printf("\n%lf:%d", kernel[i], i);
    // }
    


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
    printf("\n%lf", round(atof(argv[1]), 3));
    printf("\n%lf", (atof(argv[1]) * power(10.0, 3) - truncate(atof(argv[1]), 3) * power(10.0, 3)));
    printf("\n%lf", (truncate(atof(argv[1]), 3) * power(10.0, 3)));
    return EXIT_SUCCESS;

}