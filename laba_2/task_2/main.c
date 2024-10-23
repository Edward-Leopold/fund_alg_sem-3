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

double my_pow(errCodes *status, double base, int pow){
    if (*status != SUCCESS) return 0;

    if (pow == 0) return 1;
    if (pow < 0) return my_pow(status, 1 / base, -pow);

    if (pow % 2 == 0) {
        double res = my_pow(status, base * base, pow / 2);
        if (!isfinite(res)) {
            *status = OVERFLOW;
            return 0;
        }

        return res; // x^n = (x^2)^(n/2) = (x^(n/2)) * (x^(n/2))
    } 
    else {
        double res = base * my_pow(status, base * base, (pow - 1) / 2);
        if (!isfinite(res)) {
            *status = OVERFLOW;
            return 0;
        }

        return res;
    }
}


int main(int argc, char** argv){
    double res1;
    errCodes status1 = geom_mean(&res1, 4, 0, 5.6, 7.0, 8.987);
    if(status1 != SUCCESS){
        switch (status1){
        case NEGATIVE_SQRT:
            printf("Passed arguements for geometric mean can't be negative numbers \n");
            break;
        case OVERFLOW:
            printf("Overflow occured for geometric mean! \n");
            break;
        default:
            break;
        }
        return 1;
    }
    printf("Geometric mean: %lf\n", res1);

    errCodes status2 = SUCCESS;
    double res2 = my_pow(&status2, 2, 200);
    if(status2 != SUCCESS){
        switch (status2){
        case OVERFLOW:
            printf("Overflow occured for power operation! \n");
            break;
        default:
            break;
        }
        return 1;
    }
    printf("power: %lf\n", res2);

    return 0;
}