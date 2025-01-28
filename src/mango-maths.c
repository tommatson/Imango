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
    return (((number * power(10.0, places) - truncate(number, places) * power(10.0, places))) > 0.5 ? ((int) (number * power(10.0, places) + 1) / power(10.0, places)) : truncate(number, places) );
}