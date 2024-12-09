#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <string.h>

typedef enum errCodes {

    SUCCESS,
    MALLOC_ERR,
    DOUBLE_OVERFLOW,
} errCodes;

double fak(int n) {
    double res = 1.0;
    for (int i = 2; i <= n; ++i) res *= i;
    return res;
}

double fast_pow(double base, int exp) {
    double result = 1.0;
    while (exp > 0) {
        if (exp % 2 == 1) result *= base;
        base *= base;
        exp /= 2;
    }
    return result;
}

errCodes recomposition(double epsilon, double a, double** result_coeffs, int degree, ...) {
    *result_coeffs = (double*)malloc(sizeof(double) * (degree + 1));
    if (*result_coeffs == NULL) {
        return MALLOC_ERR;
    }

    double* init_coeffs = (double*)malloc(sizeof(double) * (degree + 1));
    if (init_coeffs == NULL) {
        free(*result_coeffs);
        return MALLOC_ERR;
    }

    va_list init_coefs;
    va_start(init_coefs, degree);
    for (int i = 0; i <= degree; ++i){
        init_coeffs[i] = va_arg(init_coefs, double);
    }
    va_end(init_coefs);

    for (int k = 0; k <= degree; ++k) {
        double new_coef_numerator = 0.0;

        for (int i = k; i <= degree; ++i) {
            double term = init_coeffs[i];
            for (int j = 0; j < k; ++j) {
                term *= (i - j); 
            }
            new_coef_numerator += term * pow(a, i - k);
        }

        (*result_coeffs)[k] = new_coef_numerator / fak(k);
        if (!isfinite((*result_coeffs)[k])){
            free(result_coeffs);
            free(init_coeffs);
            return DOUBLE_OVERFLOW;
        }

        if (fabs((*result_coeffs)[k]) < epsilon) {
            (*result_coeffs)[k] = 0.0;
        }
    }

    free(init_coeffs);
    return SUCCESS;
}

int main() {
    int degree = 1;
    double epsilon = 1e-6;
    double a = -1.0;
    double* result_coeffs = NULL;

    errCodes status = recomposition(epsilon, a, &result_coeffs, degree, 6.0, 5.0, 2, 4);
    
    if (status != SUCCESS) {
        switch (status){
        case MALLOC_ERR:
            printf("Memory allocation error!\n");
            break;
        case DOUBLE_OVERFLOW:
            printf("Type double overflow occured while calculating coefficients!\n");
            break;
        default:
            break;
        }
        return 1;
    }

    printf("Переразложение:\n");
    // for (int i = degree; i >= 0; --i) {
    //     if (i == 0) printf("%.5f * (x + %.2f)^%d ", result_coeffs[i], a, degree - i); 
    //     printf("+ %.5f * (x + %.2f)^%d ", result_coeffs[i], a, degree - i);
    // }
    // printf("\n");
    for (int i = 0; i <= degree; i++) {
        printf("%f(x - %f)^%d ", result_coeffs[i], a, i);
        if (i != degree)
            printf("+ ");
    }
    printf("\n");

    free(result_coeffs);
    return 0;
}
