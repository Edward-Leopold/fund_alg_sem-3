#include <stdio.h>
#include <math.h>
#include "../err.h"

errorCodes getOpts(int argc, char** argv, double* epsilon){
    if (argc != 2){
        if (argc < 2){
            return NOT_ENOUGH_ARGUEMENTS;
        } else{
            return TOO_MANY_ARGUEMENTS; // to much arguements
        }
    }

    const char* number_option = argv[1];
    double int_num = 0;
    int flag = 0;
    int digits_after_dot = 0;

    for (int i = 0; number_option[i]; i++){
        char ch = number_option[i];
        if (ch >= '0' || ch <= '9' || ch == '.'){
            if (ch == '.' && flag) return 201;
            if (ch == '.'){
                flag = 1;
                continue;
            }
            if (flag){
                digits_after_dot++;
            }
            int_num *= 10;
            int_num += ch - '0';
        } else {
            return INVALID_DOUBLE;
        }
    }

    *epsilon = int_num / pow(10, digits_after_dot);
    
    if (*epsilon == 0){
        return INVALID_ARGUEMENT;
    }

    return NORMAL;
}

double factorial(double n){
    if (n == 0) return 1;
    double temp = 1;
    for(int i = 1; i <= n; i++) temp *= i;
    return temp;
}

// limits

double e_lim(double n){
    return pow(1 + (1/n), n);
}

double pi_lim(double n){
    return (pow(pow(2, n) * factorial(n), 4.0) / (n * pow(factorial(2*n), 2.0)));
}

double ln2_lim(double n){
    return n * (pow(2, 1/n) - 1);
}

double limit(double (*f)(double), double eps){
    double prev_val = f(1);
    double val = f(2);
    double n = 2;
    while (fabs(val - prev_val) > eps){
        prev_val = val;
        n++;
        val = f(n);
    }
    return f(n -= 2);
}

double lim_sqrt2(double eps){
    double prev_x;
    double x = -0.5;
    double n = 0;
    while(fabs(x - prev_x) > eps){
        prev_x = x;
        n++;
        x = prev_x - (pow(prev_x, 2) / 2) + 1;
    }
    return x;
}

// sequences

double e_seq(double n){
    return (1 / factorial(n));
}

double pi_seq(double n){
    return ((int)n % 2 == 0) ? (( -4 / (2*n - 1))) : (( 4 / (2*n - 1))) ; 
}

double ln2_seq(double n){
    return pow(-1, n - 1) / n;
}

double sqrt2_seq(double k){
    return pow(2, pow(2, -k));
}

double sum(double (*f)(double), double start_n, double eps){
    double prev_val = f(start_n);
    start_n++;
    double val = f(start_n);
    double n = start_n;
    while (fabs(val) > eps){
        prev_val += val;
        n++;
        val = f(n);
    }
    return val + prev_val;
}

double product(double (*f)(double), double start_k, double eps){
    double prev_val = f(start_k);
    double val = prev_val * f(++start_k);
    double k = start_k;
    while (fabs(val - prev_val) > eps){
        prev_val = val;
        k++;
        val = prev_val * f(k);
    }
    return val;
}

// equations

double e_equ(double x){
    return log(x) - 1;
}

double pi_equ(double x){
    return tan(x);
}

double ln2_equ(double x){
    return exp(x) - 2;
}

double sqrt2_equ(double x){
    return pow(x, 2) - 2;
}

double dichotomy(double (*f)(double), double left, double right, double eps){
    double center;
    while(fabs(right - left) > eps){
        center = (right + left) / 2.0;
        if(f(left) * f(center) > 0){
            left = center;
        } else{
            right = center;
        }
    }
    return center;
}

int main(int argc, char** argv){
    double eps = 0;
    errorCodes err_status = getOpts(argc, argv, &eps);

    if(err_status != NORMAL){ // handling errors from cli input
        switch (err_status)
        {
        case NOT_ENOUGH_ARGUEMENTS:
            printf("%s \n", "Not enough number of argumnets. You have to pass one arguement.");
            break;
        case TOO_MANY_ARGUEMENTS:
            printf("%s \n", "Too many argumnets. You have to pass one arguement.");
            break;
        case INVALID_DOUBLE:
            printf("%s \n", "Invalid value of first arguement. The first arguement must be a valid float number.");
            break;
        case INVALID_ARGUEMENT:
            printf("%s \n", "Invalid value of first arguement. The first arguement must be more than 0.");
            break;
        }
        return 1;
    }
    
    printf("Число e:\n");
    printf("Предел последовательности: %20.20lf \n", limit(e_lim, eps));
    printf("Сумма ряда: %20.20lf \n", sum(e_seq, 0, eps));
    printf("Уравнение: %20.20lf \n", dichotomy(e_equ, 2.0, 3.0, eps));
    printf("===============================\n");

    printf("Число pi:\n");
    printf("Предел последовательности: %20.20lf \n", limit(pi_lim, eps));
    printf("Сумма ряда: %20.20lf \n", sum(pi_seq, 1, eps));
    printf("Уравнение: %20.20lf \n", dichotomy(pi_equ, 3.0, 4.0, eps));
    printf("===============================\n");

    printf("Число ln2:\n");
    printf("Предел последовательности: %20.20lf \n", limit(ln2_lim, eps));
    printf("Сумма ряда: %20.20lf \n", sum(ln2_seq, 1, eps));
    printf("Уравнение: %20.20lf \n", dichotomy(ln2_equ, 0.0, 1.0, eps));
    printf("===============================\n");

    printf("Число sqrt(2):\n");
    printf("Предел последовательности: %20.20lf \n", lim_sqrt2(eps));
    printf("Произведение ряда: %20.20lf \n", product(sqrt2_seq, 2, eps));
    printf("Уравнение: %20.20lf \n", dichotomy(sqrt2_equ, 1.0, 2.0, eps));
    printf("===============================\n");

    return 0;
}