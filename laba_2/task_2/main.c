#include <stdio.h>
#include <limits.h>
#include <stdarg.h>
#include <math.h>
#include <float.h>


typedef enum errCodes {
    OVERFLOW,
    NEGATIVE_SQRT,
    SUCCESS,
} errCodes;

errCodes geom_mean(double *res, int count, ...){
    double product = 1.0;

    va_list list;
    va_start(list, count);
    for(int i = 0; i < count; i++){
        double n = va_arg(list, double);
        if (n < 0) {
            va_end(list);
            return NEGATIVE_SQRT;
        }
        if (product > DBL_MAX / n) {
            va_end(list);
            return OVERFLOW;
        }
        product *= n;
    }
    va_end(list);

    *res = pow(product, 1.0 / count);
    if(!isfinite(*res)) return OVERFLOW;

    return SUCCESS;
}



int main(int argc, char** argv){
    double res1;
    errCodes status1 = geom_mean(&res1, 4, 4.5, 5.6, 7.0, 8.987);
    if(status1 != SUCCESS){
        switch (status1){
        case NEGATIVE_SQRT:
            printf("Passed arguements for geometric mean can't be negative numbers \n");
            break;
        case OVERFLOW:
            printf("Overflow occured! \n");
            break;
        default:
            break;
        }
        return 1;
    }
    printf("%lf\n", DBL_MAX);
    printf("Geometric mean: %lf\n", res1);

    return 0;
}