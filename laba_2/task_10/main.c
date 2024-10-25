#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <math.h>
#include <string.h>

typedef enum errCodes {
    MALLOC_ERR,

    SUCCESS
} errCodes;

double fak(int n) {
    double res = 1.0;
    for (int i = 2; i <= n; ++i) res *= i;
    return res;
}

double coef_C(int n, int k) {
    if (k > n) return 0;
    return fak(n) / (fak(k) * fak(n - k));
}

void calc_binomial_coeffs(int n, double* coeffs) {
    for (int k = 0; k <= n; ++k) {
        coeffs[k] = coef_C(n, k);
    }
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

    double* initial_poly = (double*)malloc(sizeof(double) * (degree + 1));
    if (!initial_poly) {
        free(*result_coeffs);
        return MALLOC_ERR;
    }

    va_list args;
    va_start(args, degree);
    for (int i = degree; i >= 0; --i) {
        initial_poly[i] = va_arg(args, double);
    }
    va_end(args);

    double* binom_coeffs = (double*)malloc(sizeof(double) * (degree + 1));
    if (!binom_coeffs) {
        free(initial_poly);
        free(*result_coeffs);
        return MALLOC_ERR; 
    }
    calc_binomial_coeffs(degree, binom_coeffs);

    for (int i = 0; i <= degree; ++i) {
        (*result_coeffs)[i] = binom_coeffs[i] * fast_pow(a, i) * initial_poly[0];
    }

    for (int current_deg = 1; current_deg <= degree; ++current_deg) {
        double next_coeff = initial_poly[current_deg] - (*result_coeffs)[current_deg];
        
        for (int i = current_deg; i <= degree; ++i) {
            (*result_coeffs)[i] += next_coeff * binom_coeffs[i - current_deg] * fast_pow(a, i - current_deg);
        }
    }


    return SUCCESS;
}

int main() {
    int degree = 5;
    double a = 36;
    double* result_coeffs = NULL;
    double epsilon = 1e-9;

    errCodes status = recomposition(epsilon, a, &result_coeffs, degree, 2.0, 1.0, 1.0);
    
    if (status != SUCCESS) {
        printf("Ошибка выделения памяти\n");
        return 1;
    }
    printf("Пересложение:\n");
    for (int i = degree; i >= 0; --i) {
        printf("+ %.5f * (x + %.2f)^%d ", result_coeffs[i], a, degree - i);
    }
    printf("\n");

    free(result_coeffs);
    return 0;
}
