#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <math.h>
#include <limits.h>

typedef enum errs {
    MALLOC_ERR,
    REALLOC_ERR,
    LIMITS_ERR,
    RANGE_ERR,
    BASE_ERR,
    SUCCESS
} errs;

errs prime_factors(int n, int** out_list){
    int* list = malloc(sizeof(int) * 3);
    if (list == NULL) return MALLOC_ERR;
    int malloc_size = 2;
    int count = 0;

    for (int i = 2; i <= n; i++){
        if (count + 1 >= malloc_size){ 
            malloc_size *= 2;
            int* temp = realloc(list, sizeof(int) * (malloc_size + 1));
            if (temp == NULL) {
                free(list);
                return REALLOC_ERR;
            }
            list = temp;
        }
        if(n % i == 0) list[count++] = i;
        while(n % i == 0){
            n /= i;
        }
    }
    list[count] = -1;
    *out_list = list;
    return SUCCESS;
}

int gcd(int a, int b) {
    while (b != 0) {
        int temp = b;
        b = a % b;
        a = temp;
    }
    return a;
}

errs get_fraction(double value, int* numerator, int* denominator) {
    double eps = 0.0001;
	double fractionalPart = value;
	int prevNumerator = 0, curNumerator = 1;
	int prevDenominator = 1, curDenominator = 0;
	double quot;

	while (fabs(value - (double)curNumerator / curDenominator) > eps) {
		quot = floor(fractionalPart);
		double tempNumerator = curNumerator;
		double tempDenominator = curDenominator;

		curNumerator = quot * curNumerator + prevNumerator;
		curDenominator = quot * curDenominator + prevDenominator;

		if (curNumerator > INT_MAX || curDenominator > INT_MAX) return LIMITS_ERR;

		prevNumerator = tempNumerator;
		prevDenominator = tempDenominator;

		if (fractionalPart == quot) break;

		fractionalPart = 1.0 / (fractionalPart - quot);
	}

	*numerator = curNumerator;
	*denominator = curDenominator;

	int commonDivisor = gcd(*numerator, *denominator);
	*numerator /= commonDivisor;
	*denominator /= commonDivisor;

    return SUCCESS;
}

errs convert(double** answer, int base, int count, ...){
    double *ans = malloc(sizeof(double) * (count + 1)); 
    if (ans == NULL) return MALLOC_ERR;

    va_list nums;
    va_start(nums, count);
    for(int i = 0; i < count; i++){
        double n = va_arg(nums, double);
        if (n >= 1 || n <= 0) {
            va_end(nums);
            return RANGE_ERR;
        }
        if (base > 36 || base < 2){
            va_end(nums);
            return BASE_ERR;
        } 
        
        int numerator;
        int denominator; // numerator / denominator

        errs frac_status = get_fraction(n, &numerator, &denominator);
        if (frac_status != SUCCESS) {
            va_end(nums);
            return frac_status;
        }
        
        int* base_factors;
        int* denom_factors;
        errs base_status = prime_factors(base, &base_factors);
        errs denom_status = prime_factors(denominator, &denom_factors);
        if (base_status != SUCCESS) {
            va_end(nums);
            return base_status;
        }
        if (denom_status != SUCCESS){
            va_end(nums);
            return denom_status;
        } 

        int is_subset = 1;
        for (int i = 0; denom_factors[i] > 0; i++){
            int is_found = 0;
            for (int j = 0; base_factors[j] > 0; j++){
                if(denom_factors[i] == base_factors[j]){
                    is_found = 1;
                    break;
                }
            }
            if (!is_found){
                is_subset = 0;
                break;
            }
        }

        ans[i] = is_subset ? n : -n;
        free(base_factors);
        free(denom_factors);
    }
    va_end(nums);
    ans[count] = -1;

    *answer = ans;

    return SUCCESS;
}

int main(){
    double* answer; 
    errs status = convert(&answer, 16, 7, 0.125, 0.88, 0.34593, 0.99999999, 0.81731, 0.0008, 0.0000000000000000001);
    if (status != SUCCESS){
        switch (status){
        case MALLOC_ERR:
            printf("Malloc error occured!\n");
            break;
        case REALLOC_ERR:
            printf("Realloc error occured!\n");
            break;
        case LIMITS_ERR:
            printf("Limit overflow!\n");
            break;
        case RANGE_ERR:
            printf("Numbers must be between 0 and 1\n");
            break;
        case BASE_ERR:
            printf("Base must be from 2 to 36\n");
            break;
        default:
            break;
        }
        return 1;
    }

    for (int i = 0; answer[i] != -1; i++){
        double num = answer[i];
        if(num > 0) printf("Число %.8lf имеет конечное представление\n", fabs(num));
        else printf("Число %.8lf не имеет конечного представления\n", fabs(num));
    }
    free(answer);
    
    return 0;
}