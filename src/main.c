#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "greyscale.h"


#define pi 3.1415926535897932

long double power(double a, int b){
    long double c = 1;
    if (b==1){
        return  (long double) a;
    }
    while (b > 1){
        if((b % 2) == 1){
            // b is odd
            c *= a;
            b--;
        } 
        c *= (a * a);
        b /= 2;
        
    }
    return c;
}
long double factorial(int a){
    long double c = 1;
    for(int i = 2; i < (a+1); i++){
        c*=i;
    }
    return c;
}

double approximateExponential(double squaredPart){
    double exponentialSum = 1;
    for (int j = 1; j < 100; j++){
        exponentialSum += power(squaredPart, j) / factorial(j);
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

    for (int i = 0; i < (kernelWidth * kernelWidth); i++){
        kernel[i] = calculateKernelItem(stanDev, i, kernelWidth);
    }




}



int main(int argc, char* argv[]){

    if (argc != 4){
        fprintf(stderr, "Usage: %s <input .bmp file> <int kernel width> <standard deviation>", argv[0]);
        return EXIT_FAILURE;
    }
    //gaussianConvert(argv[1], atoi(argv[2]), atof(argv[3]));
    //printf("%f", power(atoi(argv[1]), atof(argv[2])));
    printf("\n%d", atof(argv[1]));
    double x = 5.0;
    printf("\n%lf", approximateExponential(x));
    //greyscaleConvert(argv[1]);
    return EXIT_SUCCESS;

}