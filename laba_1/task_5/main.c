#include <stdio.h>
#include <math.h>

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
            return 1;
        }
    }

    double temp = int_num / pow(10, digits_after_dot);
    *result_number = is_negative ? -temp : temp;

    return 0;
}

int getOpts(int argc, char** argv, double* epsilon, double* x){
    if (argc != 3){
        if (argc < 3){
            return 101; // not enough arguements
        } else{
            return 102; // to much arguements
        }
    }

    char* eps_option = argv[1];
    char* x_option = argv[2];
    if (parse_double(eps_option, epsilon)) return 301;
    if (parse_double(x_option, x)) return 301;

    return 0;
}

double factorial(double n){
    if (n == 0) return 1;
    double temp = 1;
    for(int i = 1; i <= n; i++) temp *= i;
    return temp;
}

double double_factorial(double n) {
    if (n == 0 || n == 1 || n == -1) return 1;
    double temp = 1;
    for (int x = n; x >= 1; x -= 2) temp *= x;
    return temp;
}


double a_func(double n, double x, double prev_val){
    if (prev_val == 0) return pow(x, n) / factorial(n);
    // if (n == 0) return 1;
    return prev_val * x/n;
}

double b_func(double n, double x, double prev_val){
    if (prev_val == 0) return (pow(-1, n) * pow(x, 2*n) / factorial(2*n));
    // if (n == 0) return 1;
    return prev_val * (-1) * x*x / (2*n * (2*n - 1));
}

// насколько я понял ряды двух последних функций расходятся при x => 1
double c_func(double n, double x, double prev_val){
    if (prev_val == 0) return ((pow(3, 3*n) * pow(factorial(n), 3) * pow(x, 2*n)) / factorial(3*n));
    // if (n == 0) return 1;
    return prev_val * ( (27 * x*x * n*n*n) / (27*n*n*n - 27*n*n + 6*n) );
}

double d_func(double n, double x, double prev_val){
    if (prev_val == 0) return (pow(-1, n) * double_factorial(2*n - 1) * pow(x, 2*n)) / double_factorial(2*n);
    // if(n == 1) return -(x * x)/2;
    return prev_val * ( ((-1)*x*x*(2*n - 1)) / (2*n) );
}

double sum(double (*f)(double, double, double), double start_n, double eps, double x){
    double prev_val = f(start_n, x, 0);
    start_n++;
    double val = f(start_n, x, prev_val);
    double n = start_n;
    while (fabs(val) > eps){
        prev_val += val;
        n++;
        val = f(n, x, val);
    }
    return val + prev_val;
}

int main(int argc, char** argv){
    double eps = 0;
    double x = 0;
    int err_status = getOpts(argc, argv, &eps, &x);
    if(err_status){ // handling errors from cli input
        switch (err_status)
        {
        case 101:
            printf("%s \n", "Not enough number of argumnets. You have to pass two arguements.");
            break;
        case 102:
            printf("%s \n", "Too many argumnets. You have to pass two arguements.");
            break;
        case 301:
            printf("%s \n", "Ivnalid arguement value (must be double).");
            break;
        }
    }
    
    
    int x_is_1 = (x >= 1) ? 1 : 0;

    printf("eps = %f, x = %f \n", eps, x);
    printf("\n");
    printf("a: %f\n", sum(a_func, 0, eps, x));
    printf("b: %f\n", sum(b_func, 0, eps, x));
    printf(x_is_1 ? "c: числовой ряд расходится, x должен быть меньше 1\n" : "c: %f\n", sum(c_func, 0, eps, x));
    printf(x_is_1 ? "d: числовой ряд расходится, x должен быть меньше 1\n" : "d: %f\n", sum(d_func, 1, eps, x));
    return 0;
}