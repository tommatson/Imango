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
#include "canny.h"
#include "shi-tomasi.h"
#include "corner-detection.h"
#include "DoG.h"
// Kill me 
extern unsigned int sleep(unsigned int seconds);
// Kill me no more





int main(int argc, char* argv[]){
    setbuf(stdout, NULL);
    if (argc != 4){
        fprintf(stderr, "Usage: %s <input .bmp file> <int kernel width> <standard deviation>", argv[0]);
        return EXIT_FAILURE;
    }
    // greyscaleConvert(argv[1]);

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
    //sobelConvert(argv[1], 0);
    //sobelConvert(argv[1], 1);
    //applyCanny(argv[1]);
    //cornerDetect(argv[1], argv[2]);
    //sobelConvert(argv[1], 1);
    //printf("%f", Q_rsqrt(1 / atof(argv[1])));
    
    // differenceOfGaussians(argv[1]);
    applyCorner(argv[1]);



    //cornerDetect(argv[1], argv[2]);
    //applyCanny(argv[1]);
    
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
    return EXIT_SUCCESS;

}