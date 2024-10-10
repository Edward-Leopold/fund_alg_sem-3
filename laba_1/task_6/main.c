#include <stdio.h>
#include <math.h>
#include "../err.h"

int parse_double(char* proceeding_number, double* result_number){
    int flag = 0;
    int digits_after_dot = 0;
    int int_num = 0;
    int is_negative = 0;
    if (proceeding_number[0] == '-'){ // parsing negative double number
        is_negative = 1;
    }
    for (int i = is_negative ? 1 : 0; proceeding_number[i]; i++){
        char ch = proceeding_number[i];
        if ((ch >= '0' && ch <= '9') || ch == '.'){
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
            return 1;
        }
    }

    double temp = int_num / pow(10, digits_after_dot);
    *result_number = is_negative ? -temp : temp;

    return 0;
}

errorCodes getArgs(int argc, char** argv, double* epsilon){
    if (argc != 2){
        if (argc < 2){
            return NOT_ENOUGH_ARGUEMENTS; // not enough arguements
        } else{
            return TOO_MANY_ARGUEMENTS; // too many arguements
        }
    }

    char* eps_option = argv[1];
    if (parse_double(eps_option, epsilon)) {
        return INVALID_DOUBLE;
    }
    if (*epsilon <= 0){
        return INVALID_EPSILON;
    }
    if (*epsilon < 0.000000001){
        return TOO_SMALL_EPSILON;
    }
    if (*epsilon > 1){
        return TOO_BIG_EPSILON;
    }

    return NORMAL;
}

double a_func(double x){
    return log(1 + x) / x;
}

double b_func(double x){
    return exp(-x * x/2);
}

double c_func(double x){
    // return log(1 / (1 - x));
    return -log(1 - x);
}

double d_func(double x){
    return pow(x, x);
}

// double integral(double (*f)(double), double left, double right, double eps){
//     double sum = 0;
//     double interval = right - left;
//     double n = interval / eps;
//     for (int i = 0; i < n; i++){
//         double x = (left + i * eps) + eps / 2;
//         sum += eps * f(x);
//     }
//     return sum;
// }

double integral(double (*f)(double), double left, double right, double eps){
    double sum = 0;
    double interval = right - left;
    double prev_integ;
    double integ = 0;
    int n = 2;
    do{
        prev_integ = integ;
        integ = 0;
        double part = interval / (double)n;
        for (int i = 0; i < n; i++){
            double x = (left + i * part) + part/2;
            integ += part * f(x); 
        }
        n *= 2;
    } while (fabs(prev_integ - integ) > eps);
    
    return integ;
}

int main(int argc, char** argv){
    double eps = 0;

    errorCodes err_status = getArgs(argc, argv, &eps);
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
        case INVALID_EPSILON:
            printf("%s \n", "Invalid value of first arguement. The first arguement must be greater than 0.");
            break;
        case TOO_SMALL_EPSILON:
            printf("Entered epsilon is too small, enter a bigger epsilon\n");
            break;
        case TOO_BIG_EPSILON:
            printf("Entered epsilon is too big, enter an epsilon smaller than 1\n");
            break;
        }
        return 1;
    }
    
    printf("a: %f\n", integral(a_func, 0, 1, eps));
    printf("b: %f\n", integral(b_func, 0, 1, eps));
    printf("c: %f\n", integral(c_func, 0, 1, eps));
    printf("d: %f\n", integral(d_func, 0, 1, eps));

    return 0;
}