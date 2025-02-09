#include <stdbool.h>

double power(double base, int exponent){
    if (exponent < 0){
        // Not implemented yet
    } else if (exponent == 0 ){
        return 1.0;
    } else if (exponent == 1){
        return base;
    } 
    else {
        long double c = 1;

        while (exponent > 1){
            if((exponent % 2) == 1){
                // exponent is odd
                c *= base;
                exponent--;
            } 
            c *= (base * base);
            exponent /= 2;
            
        }
        return c;
    }
}

double truncate(double number, int places){
    return (int) (number * power(10.0, places)) / power(10.0, places);
}

double round(double number, int places){
    // Long ah return statement
    if (number > 0) return (((number * power(10.0, places) - truncate(number, places) * power(10.0, places))) > 0.5 ? ((int) (number * power(10.0, places) + 1) / power(10.0, places)) : truncate(number, places));
    else if (number == 0) return 0.0;
    else if (number < 0) return (((-1 * number * power(10.0, places) + truncate(number, places) * power(10.0, places))) > 0.5 ? ((int) (number * power(10.0, places) - 1) / power(10.0, places)) : truncate(number, places));
}

double squareRoot(double number, float accuracy)
{
    // Square root using newtons method
    // Credit for the solution goes to geeksforgeeks
    double x = number;
    double root;
 
    int count = 0;
    
    while (1) { 
        count++;
        root = 0.5 * (x + (number  / x));
        if ((root - x > 0 ? root - x : x - root) < accuracy){
            break;
        }
        x = root;
    }
 
    return root;
}

double arctan(double x) {
    if (x > 1) {
        return 1.57079632679 - arctan(1 / x);  // pi / 2 - arctan(1/x) transformation
    } else if (x < -1) {
        return -1.57079632679 - arctan(1 / x);
    }

    double sum = x;
    double term = x;
    
    for (int j = 3; j < 100; j += 2) {
        term *= (x * x);
        sum += (j % 4 == 1) ? term / j : -term / j;  // Alternating signs
    }

    return sum;
}


// Fast square root, borrowed from quake
float Q_rsqrt( float number )
{
	long i;
	float x2, y;
	const float threehalfs = 1.5F;

	x2 = number * 0.5F;
	y  = number;
	i  = * ( long * ) &y;						// evil floating point bit level hacking
	i  = 0x5f3759df - ( i >> 1 );               // what the fuck?
	y  = * ( float * ) &i;
	y  = y * ( threehalfs - ( x2 * y * y ) );   // 1st iteration
//	y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed

	return y;
}